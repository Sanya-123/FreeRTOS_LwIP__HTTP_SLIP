#include "theardTelnetServer.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include <stdbool.h>


#ifdef USE_CONSOLE_TELNET
#include "misc.h"
#include "microrl.h"

#include "uart.h"

// create microrl object and pointer on it
microrl_t rlm_T;
microrl_t * prl_T = &rlm_T;

uint8_t buferTerminal[4096*10] = {0};
uint32_t lenbufT = 0;

uint32_t numAbonent = 0;

SemaphoreHandle_t semaforeAbonent = NULL;//каждый абонент может пользоваться семафорой когда она свободна потомучто один обрпаботчик для telnet
bool useTelNet = false;

void printTelNet(const char * str)
{
    int i = 0;
    while (str [i] != 0) {
        buferTerminal[lenbufT++] = str[i++];//кладу в буфер данные которые отправлю по сети
    }
//    if(usenewconn != NULL)    //так почему то не получаеться
//        netconn_write(usenewconn, (void*)str, (u16_t)i, NETCONN_NOCOPY);
}

void task_abonentTelNet(void *p);

void theard_Telnet_Server(void *p)
{
    (void)p;
    //create semafore abonent
    semaforeAbonent = xSemaphoreCreateBinary();
    xSemaphoreGive(semaforeAbonent);
    vTaskDelay(500);

    //***********************************init console***********************************
    //init terminal
    microrl_init (prl_T, printTelNet);
    microrl_set_execute_callback (prl_T, execute);
#ifdef _USE_COMPLETE
    // set callback for completion
    microrl_set_complete_callback (prl_T, complet);
#endif
//	 set callback for Ctrl+C
    microrl_set_sigint_callback (prl_T, sigint);
    //set echo
    microrl_set_echo(prl_T, true);//по умолдчанию тут есть эхо

    //**********************************************************************

    //**************************Telnet port**************************
    struct netconn *conn, *newconn;
    err_t err;

    /* Create a new connection identifier. */
    /* Bind connection to well known port number 7. */
    conn = netconn_new(NETCONN_TCP);
    netconn_bind(conn, IP_ADDR_ANY, TELNET_PORT);
    LWIP_ERROR("Telnet_server: invalid conn", (conn != NULL), return;);

    /* Tell connection to go into listening mode. */
    netconn_listen(conn);
    //****************************************************

    xprintf("Init Telnet DONE!\n");

    while(1)
    {
        vTaskDelay(5);
        err = netconn_accept(conn, &newconn);
        if (err != ERR_OK)
        {
            continue;
        }
//        //else server
//        //create abonent
//        if(numAbonent < MAX_ABONENT_TELNET)
//        {
//            static uint32_t numConnect = 0;
//            char abonentN[12] = {0};
//            xsprintf(abonentN, "abonent %d", numConnect++);
//            if(xTaskCreate(task_abonentTelNet, abonentN, 2048, (void*)newconn, 2, NULL) == pdPASS)//если получилось создать таск
//            {
//                numAbonent++;
//            }
//            else//иначе закрываю соединение
//            {
//                netconn_close(newconn);
//                netconn_delete(newconn);
//            }
//        }
//        else//иначе закрываю соединение
//        {
//            xprintf("So more abonents now connect\n");
//            netconn_close(newconn);
//            netconn_delete(newconn);
//        }
        struct netbuf *inbuf;
        char *buf;
        u16_t buflen;
        err_t err;
        xprintf("Begin seanse Telnet\n");

        /* Read the data from the port, blocking if nothing yet there.
         We assume the request (the part we care about) is in one netbuf */
        do
        {
            err = netconn_recv(newconn, &inbuf);
            if (err == ERR_OK)
            {
                while(xSemaphoreTake(semaforeAbonent, portMAX_DELAY) != pdTRUE);//жду пока освободиться семафора
                useTelNet = true;//флаг говорящий куда отправлять данные
                netbuf_data(inbuf, (void **)&buf, &buflen);
                if(buflen > 2)//это он просит допуск
                {
                    netconn_write(newconn, buf, buflen, NETCONN_NOCOPY);
                    microrl_insert_char(prl_T, '\r');//иметирую энтер чтобы появилась строка
                }
                else
                {
                    //обращаюсь к обработчику
                    if(buflen == 2)
                    {
                        microrl_insert_char(prl_T, buf[0]);
                        microrl_insert_char(prl_T, buf[1]);
                    }
                    else
                        microrl_insert_char(prl_T, buf[0]);
                }
                //WARNING в той консоли и так присуствует эхо
                netconn_write(newconn, (void*)buferTerminal, (u16_t)lenbufT, NETCONN_NOCOPY);//отправяю данные сформирование microrl
                lenbufT = 0;
                useTelNet = false;
                xSemaphoreGive(semaforeAbonent);//все обработа  и отослал возвращаю семафору
            }
            else
                break;
        }
        while(1);
        //отключение от абонента
        xprintf("Close seanse Telnet\n");
        netconn_close(newconn);
        netbuf_delete(inbuf);
        netconn_delete(newconn);

        numAbonent--;
        vTaskDelete(NULL);//по закрытию абонента завершаю таск
        while(1);
    }
}

void task_abonentTelNet(void *p)//таск абоненты для каждого будет выделять новый
{
    struct netconn *newconn = (struct netconn *)p;
    struct netbuf *inbuf;
    char *buf;
    u16_t buflen;
    err_t err;
    xprintf("Begin seanse Telnet\n");

    /* Read the data from the port, blocking if nothing yet there.
     We assume the request (the part we care about) is in one netbuf */
    do
    {
        err = netconn_recv(newconn, &inbuf);
        if (err == ERR_OK)
        {
            while(xSemaphoreTake(semaforeAbonent, portMAX_DELAY) != pdTRUE);//жду пока освободиться семафора
            useTelNet = true;//флаг говорящий куда отправлять данные
            netbuf_data(inbuf, (void **)&buf, &buflen);
            if(buflen > 2)//это он просит допуск
            {
                netconn_write(newconn, buf, buflen, NETCONN_NOCOPY);
                microrl_insert_char(prl_T, '\r');//иметирую энтер чтобы появилась строка
            }
            else
            {
                //обращаюсь к обработчику
                if(buflen == 2)
                {
                    microrl_insert_char(prl_T, buf[0]);
                    microrl_insert_char(prl_T, buf[1]);
                }
                else
                    microrl_insert_char(prl_T, buf[0]);
            }
            //WARNING в той консоли и так присуствует эхо
            netconn_write(newconn, (void*)buferTerminal, (u16_t)lenbufT, NETCONN_NOCOPY);//отправяю данные сформирование microrl
            lenbufT = 0;
            useTelNet = false;
            xSemaphoreGive(semaforeAbonent);//все обработа  и отослал возвращаю семафору
        }
        else
            break;
    }
    while(1);
    //отключение от абонента
    xprintf("Close seanse Telnet\n");
    netconn_close(newconn);
    netbuf_delete(inbuf);
    netconn_delete(newconn);

    numAbonent--;
    vTaskDelete(NULL);//по закрытию абонента завершаю таск
    while(1);
}

#endif

