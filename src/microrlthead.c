#include "microrlthead.h"
#include "misc.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
#include "uart.h"
#include <string.h>
//apps
#include "ebi_rw.h"

// create microrl object and pointer on it
microrl_t rlm;
microrl_t * prl = &rlm;

SemaphoreHandle_t semaforeAbonent = NULL;//каждый абонент может пользоваться семафорой когда она свободна потомучто один обрпаботчик для telnet
QueueHandle_t recyveData = NULL;

extern void printTelNet(const char * str);

void printMicroRl(const char * str)
{
#ifdef USE_CONSOLE
    int i = 0;
    while (str [i] != 0) {
        putUart6(str[i++]);//кладу в буфер данные которые отправлю по сети
    }
#endif
#ifdef USE_CONSOLE_TELNET
    printTelNet(str);
#endif
}

//************************************My CMD************************************
const char _CMD_TASK_I[] =  		"aboutTask";
int print_aboutyTask(void * vpThis, int argc, const char * const * argv);
const char  _CMD_TN_AB[] =  		"TelNetAbonentCon";
int print_abonentConnetcNow(void * vpThis, int argc, const char * const * argv);
const char  _CMD_CPU_IDLE[] =  		"CPU_Idle";
int print_CPU_FREE(void * vpThis, int argc, const char * const * argv);
const char _CMD_COU_GR[] =          "CPU_Gra";
int print_MAS_CPU(void * vpThis, int argc, const char * const * argv);

//************************************************************************
//************************************My APPS************************************
const char _CMD_EBI_RV[] =  		"ebi_rv";
int ebi_rv_cmd(void * vpThis, int argc, const char * const * argv)
{
    setOutMicroRl__ebi_rv((microrl_t *)vpThis);
    return main_ebi_rw(argc, (char **)argv);    //тупо вызываю периложение
}
const char _CMD_TIME[] =            "time";
int time_cmd(void * vpThis, int argc, const char * const * argv)//задача запускающая задачи и смотри сколько на них тратиться времени
{
    if(argc == 1)
    {
        print((microrl_t *)vpThis, "Programm 'time' incluse programm in second argument and use timer\n\r");
        print((microrl_t *)vpThis, "you second argument is empty!\n\r");
        return -1;
    }
    TickType_t LastTick = xTaskGetTickCount();
    execute(vpThis, argc - 1, &argv[1]);//вызываю 2 аргумент
    TickType_t timeUse = (xTaskGetTickCount() - LastTick) / portTICK_PERIOD_MS;//in ms
    char buf[32];
    xsprintf(buf, "%dms\n\r", timeUse);
    print((microrl_t *)vpThis, "For task(");
    print((microrl_t *)vpThis, argv[0]);
    print((microrl_t *)vpThis, ") use time: ");
    print((microrl_t *)vpThis, buf);
    return 0;
}

//************************************************************************

void recyveDat(uint8_t data)
{
    if(recyveData != NULL)
        while(xQueueSend(recyveData, (void *)(&data), portMAX_DELAY) == pdFALSE);
}

void recymeMas(uint8_t *data, uint32_t size)
{
    while(xSemaphoreTake(semaforeAbonent, portMAX_DELAY) == pdFALSE);//эту функцию может вызывать только один абонент
    for(uint32_t i = 0; i < size; i++)
        recyveDat(data[i]);
    xSemaphoreGive(semaforeAbonent);
}

void taskMicroRl(void *p)
{
    (void)p;
    //create semafore abonent
    semaforeAbonent = xSemaphoreCreateBinary();
    xSemaphoreGive(semaforeAbonent);

    recyveData = xQueueCreate(SIZE_MICRORL_QUEUE, 1);

    //***********************************init console***********************************
    //init terminal
    microrl_init (prl, printMicroRl);
    microrl_set_execute_callback (prl, execute);
#ifdef _USE_COMPLETE
    // set callback for completion
    microrl_set_complete_callback (prl, complet);
#endif
#ifdef _USE_CTLR_C
//	 set callback for Ctrl+C
    microrl_set_sigint_callback (prl, sigint);
#endif

    //init cmd
    initDefoultCMD();
    //my
    addCmd(_CMD_TASK_I, print_aboutyTask);
    addCmd(_CMD_TN_AB, print_abonentConnetcNow);
    addCmd(_CMD_CPU_IDLE, print_CPU_FREE);
    addCmd(_CMD_COU_GR, print_MAS_CPU);
    //apps
    addCmd(_CMD_EBI_RV, ebi_rv_cmd);
    addCmd(_CMD_TIME, time_cmd);

    //**********************************************************************

    if(recyveData == NULL)//если не получилось создать очередь
    {
        xprintf("can't create console\n");
        vTaskDelete(NULL);
    }

    while(1)
    {
        uint8_t rxData = (uint8_t)NULL;
        while (xQueueReceive(recyveData, (void *)(&rxData), portMAX_DELAY) != pdTRUE);//жду символ
        microrl_insert_char(prl, rxData);
    }
}

void setConsolePrompt(char * name, bool admin)
{
    microlSetPromptLen(prl, (short)strlen(name));//сдвиг по умолчанию
    char namem[TN_MAX_SIZE_STR + 4 + 9];
    if(admin)   //31+ 35+ 36+
        xsprintf(namem, PURPUR_BEGIN"%s"END_COLOR, name);
    else
        xsprintf(namem, BERUZ_BEGIN"%s"END_COLOR, name);
    strcpy(name, namem);
    microlSetPromtStr(prl, name);//имя по умолчанию
}
