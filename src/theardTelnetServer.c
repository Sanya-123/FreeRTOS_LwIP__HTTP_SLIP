#include "theardTelnetServer.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include <stdbool.h>

#include <string.h>

#include "microrlthead.h"
#include "config.h"

#ifdef USE_CONSOLE_TELNET

#ifdef USE_ACCEST
typedef struct{
    char name[TN_MAX_SIZE_STR];
    char nameConsol[TN_MAX_SIZE_STR + 4 + 9];
    char passworld[TN_MAX_SIZE_STR];
    bool isAdmin;
}UserTelNet;//структура параметров пользователя
UserTelNet users[TN_SIZE_USERS];
uint8_t usersSize = 0;
#endif

enum var{LoginPrint = 0, LoginInput, PassInput, Input};//варианты обработки
typedef struct {
    int state;//состояния доступа
    int userVal;//номер вощедщего пользователя из таблицы пользователей =0
    char val[TN_MAX_SIZE_STR];//логин(пароль) который пользователь вводит = {0}
    int numS;//номер вводимого символа =0
}optionsUser;//структура настроек пользователя для доступа

#include "uart.h"

//кольцевой буфер
#define TELNET_BUFFER_SIZE 1024 //размер буфера на передачу

// Буфер на передачу
static uint8_t  buferTerminal[TELNET_BUFFER_SIZE]; // сам буфер
static uint32_t _wr_index = 0; // индекс хвоста буфера (куда писать данные)
static uint32_t _rd_index = 0; // индекс начала буфера (откуда читать данные)
static uint32_t _counter = 0; // количество данных в буфере

uint32_t numAbonent = 0;


struct netconn *newconnUse = NULL;//указатель использующего сеанса

void printTelNet(const char * str)
{
    if(newconnUse == NULL)
        return;
    int i = 0;
    while (str [i] != 0) {
        buferTerminal[_wr_index++] = str[i++];//кладу в буфер данные которые отправлю по сети
        _counter++;
        if(_wr_index == TELNET_BUFFER_SIZE)
            _wr_index = 0;

    }
//    xprintf("i=%d;lenbufT=%d\n", i, lenbufT);
//    if(newconnUse != NULL)    //так почему то не получаеться
//    {
//        err_t ok = netconn_write(newconnUse, (void*)str, (u16_t)i, NETCONN_NOCOPY);//отправяю данные сформирование microrl
//        if(ok != ERR_OK)
//            xprintf("Error send data:%d\n", ok);
//    }
}

void task_abonentTelNet(void *p);
void task_idle_SendDataTelNet(void *p);
void initUsers();

void telNetConsole(char data, optionsUser *option)//обработчик TelNet
{
#ifdef USE_ACCEST
    switch (option->state) {
    case LoginPrint://выввожу строку
        printTelNet("\rLogin:");
        option->state = LoginInput;
    case LoginInput://ввод логина
    {
        if(data)
        {
            char nt[2] = {data, 0};
            printTelNet(nt);//CalBack
            switch (data) {
            case '\r':
            case '\n'://введен энтер
                //find user
                printTelNet("\n");
                option->val[option->numS] = 0;//clear other simbols
                option->numS = 0;//по любону надо обнулять либо для следуйщего вводо либо для пароля
                for(int i = 0; i < usersSize; i++)
                {
                    if(strcmp(option->val, users[i].name) == 0)//найден пользователь
                    {
                        option->userVal = i;
                        option->state = PassInput;
                        char pr[64] = {0};
                        xsprintf(pr, "\rPassworld for %s:", users[option->userVal].name);
                        printTelNet(pr);
                        xprintf("User:%s print passworld\n\r", users[option->userVal].name);
                    }
                }
                if(option->state != PassInput)//usen not found
                {
                    printTelNet("\rusen not found\n");
                    option->state = LoginInput;
                    printTelNet("\rLogin:");
                    xprintf("user:%s not find\n\r", option->val);
                }
                break;

            case KEY_DEL://backSpase
            {
                if(option->numS)
                {
                    //стираю символ
                    char mas[4] = {255, 247, 8, 0};
                    printTelNet(mas);
                    printTelNet(" ");
                    printTelNet(mas);
                    option->numS--;
                }
                break;
            }
            case KEY_ESC:
                break;
            default://введен символ
                if(IS_CONTROL_CHAR(data))//если управляющий символ
                    break;
                if(option->numS < TN_MAX_SIZE_STR)//при допустимом размере
                {
                    option->val[option->numS++] = data;
                }
                break;
            }
        }
        break;
    }
    case PassInput:
    {
        if(data)
        {
            switch (data) {
            case '\r':
            case '\n'://введен энтер
                //find user
                printTelNet("\r\n");
                option->val[option->numS] = 0;//clear other simbols
                option->numS = 0;
                if(strcmp(option->val, users[option->userVal].passworld) == 0)//пароль правельный
                {
                    option->state = Input;
                    setConsolePrompt(users[option->userVal].nameConsol, users[option->userVal].isAdmin);
                    recymeMas("\r", 1);
                    xprintf("%s: %s enter\n\r", users[option->userVal].isAdmin ? "Admin" : "User", \
                            users[option->userVal].name);
                }
                else //Error passvorld is error
                {
                    printTelNet("\rError passvorld is false\n");
                    char pr[64] = {0};
                    xsprintf(pr, "\rPassworld for %s:", users[option->userVal].name);
                    xprintf("Passworld for %s is false\n\r", users[option->userVal].name);
                    printTelNet(pr);
                }
                break;
            case KEY_DEL://backSpase
            {
                if(option->numS)
                    option->numS--;
                break;
            }
            case KEY_ESC:
                printTelNet("\n\rLogin:");
                option->state = LoginInput;
                option->numS = 0;
                break;
            default://введен символ
                if(IS_CONTROL_CHAR(data))//если управляющий символ
                    break;
                if(option->numS < TN_MAX_SIZE_STR)//при допустимом размере
                {
                    option->val[option->numS++] = data;
                }
                break;
            }
        }
        break;
    }
    case Input:
    {
        recymeMas(&data, 1);
        break;
    }
    default:
        option->state = LoginPrint;
        break;
    }
#else
    if(data == NULL)
        data = '\r';
    recymeMas(&data, 1);
#endif
}

void theard_Telnet_Server(void *p)
{
    (void)p;
    vTaskDelay(500);

    //**************************Telnet port**************************
    struct netconn *conn, *newconn;
    err_t err;

    /* Create a new connection identifier. */
    /* Bind connection to well known port number 7. */
    conn = netconn_new(NETCONN_TCP);
    netconn_bind(conn, IP_ADDR_ANY, TELNET_PORT);
    LWIP_ERROR("Telnet_server: invalid conn", (conn != NULL), vTaskDelete(NULL););

    /* Tell connection to go into listening mode. */
    netconn_listen(conn);
    //****************************************************

    xTaskCreate(task_idle_SendDataTelNet, "Send TelNet", 128, NULL, 1, NULL);

    initUsers();

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
//            if(xTaskCreate(task_abonentTelNet, abonentN, 1024, (void*)newconn, 2, NULL) == pdPASS)//если получилось создать таск
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
        optionsUser options;//options user thats conect now
        options.numS = 0;
#ifdef USE_ACCEST
        options.state = LoginPrint;
#else
        options.state = Input;
#endif

        /* Read the data from the port, blocking if nothing yet there.
         We assume the request (the part we care about) is in one netbuf */
        newconnUse = newconn;//говорю что этот сеанс сейчас используеться
        do
        {
            err = netconn_recv(newconn, &inbuf);
            if (err == ERR_OK)
            {
                netbuf_data(inbuf, (void **)&buf, &buflen);
                if(buflen > 3)//это он просит допуск
                {
                    char bufReturn[3] = {255, 251, 1};//не использовать эхо
                    netconn_write(newconn, (void*)bufReturn, 3, NETCONN_NOCOPY);
//                    netconn_write(newconn, buf, buflen, NETCONN_NOCOPY);//отправляю оставшиеся настройки
//                    recymeMas("\r", 1);
                    telNetConsole(NULL, &options);
                    netbuf_delete(inbuf);
                }
                else if(buflen == 3)//одиночкая команда управления
                {
                    if((buf[0] == 27) && (buf[1] == 91))//символ
                    {
                        if((buf[2] > 64) && (buf[2] < 69) && (options.state == Input))//стрелки
                            for(int i = 0; i < buflen; i++)
                                telNetConsole(buf[i], &options);
                    }
                    netbuf_delete(inbuf);
                }
                else
                {
//                    recymeMas(buf, buflen);
//                    recymeMas(buf, 1);
                    telNetConsole(buf[0], &options);
                    netbuf_delete(inbuf);
                };

            }
            else
            {
                xprintf("Close seanse Telnet\n");
                netconn_close(newconn);
                netbuf_delete(inbuf);
                netconn_delete(newconn);

                //************************************info************************************
                TaskStatus_t status;
                vTaskGetInfo(NULL, &status, pdTRUE, eInvalid);

                xprintf("+--------------------------------+\n");
                xprintf("Task name:%s\n", status.pcTaskName);
                xprintf("Size FREE:%d\n", status.usStackHighWaterMark);
                xprintf("+--------------------------------+\n");
                //************************************************************************
               break;
            }
        }
        while(1);
        newconnUse = NULL;//закончил использовать сеанс
    }
}

void task_idle_SendDataTelNet(void *p)
{
    (void)p;

    while(1)
    {
        if(newconnUse != NULL)
        {
            if(_counter)
            {
                for(uint32_t i = 0; i < _counter; i++)
                {
                    netconn_write(newconnUse, (void*)(&buferTerminal[_rd_index]), (u16_t)1, NETCONN_NOCOPY);//отправяю данные сформирование microrl
                    _rd_index++;
                    if(_rd_index == TELNET_BUFFER_SIZE)
                        _rd_index = 0;
                }
            }
            _counter = 0;
        }
        else
        {
            _counter = 0;
            vTaskDelay(1000);
        }
        vTaskDelay(10);
    }
}


void initUsers()
{
#ifdef USE_ACCEST
    xsprintf(users[usersSize].name, "admin");
    xsprintf(users[usersSize].passworld, "admin");
    xsprintf(users[usersSize].nameConsol, "%s > ", users[usersSize].name);
//    xsprintf(users[usersSize].nameConsol, "%s > ", "S");
    users[usersSize].isAdmin = true;
    usersSize++;

    xsprintf(users[usersSize].name, "user");
    xsprintf(users[usersSize].passworld, "user");
    xsprintf(users[usersSize].nameConsol, "%s > ", users[usersSize].name);
    users[usersSize].isAdmin = false;
    usersSize++;
#endif

    //and read from memory
}

//void task_abonentTelNet(void *p)//таск абоненты для каждого будет выделять новый
//{
//    struct netconn *newconn = (struct netconn *)p;
//    struct netbuf *inbuf;
//    char *buf;
//    u16_t buflen;
//    err_t err;
//    xprintf("Begin seanse Telnet\n");

//    /* Read the data from the port, blocking if nothing yet there.
//     We assume the request (the part we care about) is in one netbuf */
//    do
//    {
//        err = netconn_recv(newconn, &inbuf);
//        if (err == ERR_OK)
//        {

//            while(xSemaphoreTake(semaforeAbonent, portMAX_DELAY) != pdTRUE);//жду пока освободиться семафора
//            newconnUse = newconn;//говорю что этот сеанс сейчас используеться
//            netbuf_data(inbuf, (void **)&buf, &buflen);
//            if(buflen > 3)//это он просит допуск
//            {
//                netconn_write(newconn, buf, buflen, NETCONN_NOCOPY);
//                microrl_insert_char(prl_T, '\r');//иметирую энтер чтобы появилась строка
//            }
//            else
//            {
//                if(buflen < 3)
//                    microrl_insert_char(prl_T, buf[0]);
//                //обращаюсь к обработчику
//                else
//                    for(int i = 0; i < buflen; i++)
//                        microrl_insert_char(prl_T, buf[3]);
//            }


//            //WARNING в той консоли и так присуствует эхо
//            for(uint32_t i = 0; i < _counter; i++)
//            {
//                netconn_write(newconn, (void*)(&buferTerminal[_rd_index]), (u16_t)1, NETCONN_NOCOPY);//отправяю данные сформирование microrl
//                _rd_index++;
//                if(_rd_index == TELNET_BUFFER_SIZE)
//                    _rd_index = 0;
//            }
//            netbuf_delete(inbuf);
//            _counter = 0;
//            newconnUse = NULL;//закончил использовать сеанс
//            xSemaphoreGive(semaforeAbonent);//все обработа  и отослал возвращаю семафору
//        }
//        else
//            break;
//    }
//    while(1);
//    //отключение от абонента
//    xprintf("Close seanse Telnet\n");
//    netconn_close(newconn);
//    netbuf_delete(inbuf);
//    netconn_delete(newconn);

//    numAbonent--;

//    //************************************info************************************
//    TaskStatus_t status;
//    vTaskGetInfo(NULL, &status, pdTRUE, eInvalid);

//    xprintf("+--------------------------------+\n");
//    xprintf("Task name:%s\n", status.pcTaskName);
//    xprintf("Size FREE:%d\n", status.usStackHighWaterMark);
//    xprintf("+--------------------------------+\n");
//    //************************************************************************

//    vTaskDelete(NULL);//по закрытию абонента завершаю таск
//    while(1);
//}

int print_abonentConnetcNow(void *vpThis, int argc, const char * const *argv)//
{
    (void)argc;
    (void)argv;
    microrl_t *pThis = (microrl_t *)vpThis;

    char buf[32] = {0};
    xsprintf(buf, "\nNow connetc abonent is %d\n\r", numAbonent);
    print(pThis, buf);
    return 0;
}
#else
int print_abonentConnetcNow(microrl_t *pThis)//
{
    print(pThis, "Now TelNet is OFF\n\r");
    return -1;
}

#endif

