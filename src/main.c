#include "common.h"
#include "stm32f4xx.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"

#include "uart.h"
#include "theardNetWork.h"
#include "xprintf.h"
#include "theardConsol.h"
#include "theardTelnetServer.h"
#include "microrlthead.h"
//#include "telnet_server.h"

#include "misc.h"
#include "lwipopts.h"


void initAll();

//task in main
void task_diode(void *p);

void sendRTT(char s)
{
//    SEGGER_RTT_WriteString(0, &s);
    (void)s;
}

#ifdef USE_IWDG
// Функция инициализации сторожевого таймера IWDG
// http://blog.myelectronics.com.ua/stm32-%D1%81%D1%82%D0%BE%D1%80%D0%BE%D0%B6%D0%B5%D0%B2%D1%8B%D0%B5-%D1%82%D0%B0%D0%B9%D0%BC%D0%B5%D1%80%D1%8B-wdt/
void Init_IWDG(uint16_t tw) // Параметр tw от 7мс до 26200мс
{
    RCC->CSR |= RCC_CSR_LSION; // Пуск таймера

    // Для IWDG_PR=7 Tmin=6,4мс RLR=Tмс*40/256
    IWDG->KR = 0x5555; // Ключ для доступа к таймеру
    IWDG->PR = 7; // Обновление IWDG_PR
    IWDG->RLR = tw * 40 / 256; // Загрузить регистр перезагрузки
    IWDG->KR = 0xAAAA; // Перезагрузка
    IWDG->KR = 0xCCCC; // Пуск таймера
}
void IWDG_res(void) // Функция перезагрузки сторожевого таймера IWDG
{
    IWDG->KR = 0xAAAA; // Перезагрузка
}
#define WATCH_DOG_DELAY_MS 5000

#endif

void SystemClock_Config(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    //PWR_CR Page 138
    PWR->CR |=PWR_CR_PLS | PWR_CR_PVDE | PWR_CR_VOS;

    while (!(PWR->CSR & PWR_CSR_VOSRDY));

    //включаю HSE
    RCC->CR |= RCC_CR_HSEON;
    while((RCC->CR & RCC_CR_HSERDY) == RESET);//жду пока станет готово

    //включаю буфер предвыборки FLASH
    //конфигурирую FLASH в 2 цикла ожидания
    //это нужно потомучто флеш не может работать на высоких частотах
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;

    //use for PLL sourse HSE
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;

    //множитель PLL настраеваю P=/4;M=/6;N=*252 pllclk = HSE * 252 / (4 + 6) = 84 при кварц 8 МГц
    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLP | RCC_PLLCFGR_PLLM | (((1 << 9) - 1) << 6));
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLP_0 | RCC_PLLCFGR_PLLM_2 | RCC_PLLCFGR_PLLM_1 | (252 << 6);

    //включаю PLL
    RCC->CR |= RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY) == RESET);

    //HCLK = SYSCLK/2
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

    //PCLK1 = HCLK
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

    //PCLK1 = HCLK
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

    //выбираю PLL как источник системной частоты
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while((RCC->CFGR & RCC_CFGR_SWS_PLL) == RESET);

//    SystemCoreClock = 84000000;
//    APB1Clock = APB2Clock = 42000000
//    APB1TimerClock = APB2TimerClock = 84000000
}

// char Buffer[20];

#define MY_TASK_SIZE        5
TaskHandle_t myTasksHendle[MY_TASK_SIZE] = {NULL};

void resumAllTask(void *p);

//#define RX_TX_50HZ        D,  0,  MODE_OUTPUT_PUSH_PULL, SPEED_50MHZ,    1, AF0   //
//void tmpTask(void *arg)
//{
//    (void)arg;
//    PIN_CONFIGURATION(RX_TX_50HZ);


//    while (true) {
//        vTaskDelay(10);
//        PIN_TOGGLE(RX_TX_50HZ);
//    }
//}


int main(void)
{
    initAll();
    xprintf("Hi\n");

//    xTaskCreate(tmpTask, "tmp", 64, NULL, 1, NULL);

    xTaskCreate(task_diode, "Light Dioder", 64, NULL, 1, &myTasksHendle[0]);//таску моргания диодами

    xprintf("Add task diode\n");

    //run
    xTaskCreate(theard_NetWork_Server, "SLIP Server", 256, NULL, 5, &myTasksHendle[1]);//таск обработки slip

#ifdef USE_CONSOLE
    xTaskCreate(theard_Consol, "Consol", 256, NULL, 2, &myTasksHendle[2]);
#endif
#ifdef USE_CONSOLE_TELNET
    xTaskCreate(theard_Telnet_Server, "TelnetServer", 256, NULL, 2, &myTasksHendle[4]);
//    telnet_server_init();
#endif

    xTaskCreate(taskMicroRl, "MicroRL", 256, NULL, 2, &myTasksHendle[3]);

//    for(int i = 0; i < MY_TASK_SIZE; i++)
//    {
//        vTaskSuspend(myTasksHendle[i]);
//    }

    //задача приостанавливающаа все на некоторое время и тем самым отдает весь русурс idle он запоминает сколько свободного ресурса
    TaskHandle_t resumA = NULL;
    xTaskCreate(resumAllTask, "1 sec", configMINIMAL_STACK_SIZE, (void*)(&resumA), configMAX_PRIORITIES - 1, &resumA);

    vTaskStartScheduler();

    while(1)
    {

    }
}

void resumAllTask(void *p)
{//таск по возобновлению всех тасков
    for(int i = 0; i < MY_TASK_SIZE; i++)
    {
        if(myTasksHendle[i] != NULL)
            vTaskSuspend(myTasksHendle[i]);
    }
    vTaskDelay(2200 / portTICK_PERIOD_MS);
    TaskHandle_t handle = *((TaskHandle_t*)p);
    vTaskPrioritySet(handle, configMAX_PRIORITIES - 1);
    for(int i = 0; i < MY_TASK_SIZE; i++)
    {
        if(myTasksHendle[i] != NULL)
            vTaskResume(myTasksHendle[i]);
    }
    vTaskDelete(NULL);
}

void printInfoAbouTask(microrl_t *pThis, TaskHandle_t handleTask)//функция выводящая информацию о 1 таске
{
    //************************************info************************************
    TaskStatus_t status;
    vTaskGetInfo(handleTask, &status, pdTRUE, eInvalid);

    print(pThis, "+--------------------------------+\n\r");
    char buf[32];
    xsprintf(buf, "Task name:%s\n\r", status.pcTaskName);
    print(pThis, buf);
    xsprintf(buf, "Size FREE:%d\n\r", status.usStackHighWaterMark);
    print(pThis, buf);
    print(pThis, "+--------------------------------+\n\r");
    //************************************************************************
}

int print_aboutyTask(void *vpThis, int argc, const char * const *argv)//функция для консоли показывающая информацию о тасках
{
    (void)argc;
    (void)argv;
    microrl_t *pThis = (microrl_t *)vpThis;

    print(pThis, "\nMy tasks:\n\r");
    for(int i = 0; i < MY_TASK_SIZE; i++)
    {
        if(myTasksHendle[i] == NULL)//2 тасков можен не существовать
            continue;
        printInfoAbouTask(pThis, myTasksHendle[i]);
    }
    print(pThis, "\nlwIP tasks:\n\r");
    TaskHandle_t slipTaskH = xTaskGetHandle(SLIPIF_THREAD_NAME);
    printInfoAbouTask(pThis, slipTaskH);
    TaskHandle_t tcpipTaskH = xTaskGetHandle(TCPIP_THREAD_NAME);
    printInfoAbouTask(pThis, tcpipTaskH);
    return 0;
}


static uint32_t MPU_IDLE = 100;
static uint32_t MIN_MPU_FREE = 100;
#define SIZE_CPU_FREE       20
#ifdef SIZE_CPU_FREE
static uint32_t masCpuFree[SIZE_CPU_FREE] = {0};
#endif

void vApplicationIdleHook( void )//idle task
{//вызываеться когда ничего не вызвано(свободный процесор)
    //подсчет совбодного ресурса(процентов)
    static TickType_t LastTick;
    static uint32_t count = 0, max_count = 0;

    count++;//счетчик подсчета свободный ресурс

    if((xTaskGetTickCount() - LastTick) >= configTICK_RATE_HZ)//если прошло N тиков (1 сек)
    {
        LastTick = xTaskGetTickCount();
        if(count > max_count)
            max_count = count;  //калибровка максимальноо свободного ресурса
        MPU_IDLE = 100 * count / max_count; //расчитываю свободный ресурс в процентах
        if(MPU_IDLE < MIN_MPU_FREE)
            MIN_MPU_FREE = MPU_IDLE;//максимальная занатость
#ifdef SIZE_CPU_FREE
        for(int i = 1; i < SIZE_CPU_FREE; i++)//смешеие графика
            masCpuFree[i - 1] = masCpuFree[i];
        masCpuFree[SIZE_CPU_FREE - 1] = MPU_IDLE;//последнее значение
#endif

        count = 0;//обнуляю счетчик
    }

    //reset watch dog
#ifdef USE_IWDG
    IWDG_res();
#endif
}

int print_CPU_FREE(void *vpThis, int argc, const char * const *argv)//вывод ресурсов
{
    (void)argc;
    (void)argv;
    microrl_t *pThis = (microrl_t *)vpThis;

    char mas[128] = {0};
    //"\033[35m %s \033[0m"  //31(red) 32(green) 33(yellow) 35(purpur) 36(berezoviy)
    char *color = GREE_BEGIN;//green
    if(MPU_IDLE <= 90)//berezoviy
        color = BERUZ_BEGIN;
    if (MPU_IDLE < 50)//yellow
        color = YELLOW_BEGIN;
    if(MPU_IDLE < 25)//red
        color = RED_BEGIN;
//    xsprintf(mas, "MPU IDLE(FREE):\033[%dm%d%%"END_COLOR"\n\r", color, MPU_IDLE);
    xsprintf(mas, "MPU IDLE(FREE):%s%d%%"END_COLOR"\n\r", color, MPU_IDLE);
    print(pThis, mas);
    xsprintf(mas, "MPU USE:%s%d%%"END_COLOR"\n\r", color, 100 - MPU_IDLE);
    print(pThis, mas);

    //print min MPU free
    color = GREE_BEGIN;//green
    if(MIN_MPU_FREE <= 90)//berezoviy
        color = BERUZ_BEGIN;
    if (MIN_MPU_FREE < 50)//yellow
        color = YELLOW_BEGIN;
    if(MIN_MPU_FREE < 25)//red
        color = RED_BEGIN;
    print(pThis, "\n\r");
    xsprintf(mas, "MPU Minimum IDLE(FREE):%s%d%%"END_COLOR"\n\r", color, MIN_MPU_FREE);
    print(pThis, mas);
    xsprintf(mas, "MPU Maximum USE:%s%d%%"END_COLOR"\n\r", color, 100 - MIN_MPU_FREE);
    print(pThis, mas);

    return 0;
}

int print_MAS_CPU(void *vpThis, int argc, const char * const *argv)//выввод графика ресурсов за последнии SIZE_CPU_FREE сек
{
    microrl_t *pThis = (microrl_t *)vpThis;
    (void)argc;
    (void)argv;
#ifdef SIZE_CPU_FREE
#define Y_MAS       20//количество точек в консоли по y
    char mas[SIZE_CPU_FREE + 10] = {0};//линия по оси x
    for(int i = Y_MAS; i >= 0; i--)
    {
        //set color
        char *color = GREE_BEGIN;
        if((i * 100 / Y_MAS) <= 90)//berezoviy
            color = BERUZ_BEGIN;
        if ((i * 100 / Y_MAS) < 50)//yellow
            color = YELLOW_BEGIN;
        if((i * 100 / Y_MAS) < 25)//red
            color = RED_BEGIN;
        print(pThis, color);

        xsprintf(mas, "%3d", (i * 100 / Y_MAS));
        for(int j = 0; j < SIZE_CPU_FREE; j++)
        {
            if(masCpuFree[j] == 0)
                mas[j + 3] = ' ';
            else if(masCpuFree[j] >= (i * 100 / Y_MAS))
                mas[j + 3] = '*';
            else
                mas[j + 3] = ' ';
        }
        mas[SIZE_CPU_FREE + 3] = '\n';
        mas[SIZE_CPU_FREE + 3 + 1] = '\r';
        print(pThis, mas);
        print(pThis, END_COLOR);
    }
    xsprintf(mas, "Last %d second\n\r", SIZE_CPU_FREE);
    print(pThis, mas);
    return 0;
#else
    print(pThis, "Don't remember graphic\n\r");
    return -1;
#endif
}

void writeUARTDebug(unsigned char data)
{
//    writeUartData(data, UART_DEBUG);
#ifdef USE_CONSOLE
    sendRTT(data);
#else
    putUart6(data);
#endif
}

void initAll()
{
//    SEGGER_RTT_WriteString(0, "SEGGER Real-Time-Terminal First send\r\n");

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN;
    SystemClock_Config();

    initUART_PLL_IN();
    PIN_CONFIGURATION(UART6_TX);
    PIN_CONFIGURATION(UART6_RX);
    PIN_CONFIGURATION(UART2_TX);
    PIN_CONFIGURATION(UART2_RX);

#ifndef USE_CONSOLE
#ifdef USE_RING_BUFFER_USART6
    setTxInterupt(UART_DEBUG, true);
#endif
    configureUart(APBPERF_SPEED, /*UART_DEBUG_SPEED*/460800, UART_DEBUG);
#endif

    xdev_out(writeUARTDebug);

    xprintf("init UART!\n");

    PIN_CONFIGURATION(LED_GREEN);
    PIN_CONFIGURATION(LED_ORANGE);
    PIN_CONFIGURATION(LED_RED);
    PIN_CONFIGURATION(LED_BLUE);

#ifdef USE_IWDG
    Init_IWDG(WATCH_DOG_DELAY_MS);
#endif

    __enable_irq();

    xprintf("Init DONE!\n");
}

void task_diode(void *p)
{
    (void)p;
    int i = 0;//итерации по которой определяеться какой диод включать

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    while(1)
    {
        switch (i) {//моргание диодов по очериди
        case 0:
            PIN_OFF(LED_ORANGE);
            PIN_ON(LED_GREEN);
            break;
        case 1:
            PIN_OFF(LED_GREEN);
            PIN_ON(LED_BLUE);
            break;
        case 2:
            PIN_OFF(LED_BLUE);
            PIN_ON(LED_RED);
            break;
        case 3:
            PIN_OFF(LED_RED);
            PIN_ON(LED_ORANGE);
            break;
        default:
            break;
        }

        if(i == 3)
            i = 0;
        else
            i++;

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}


/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetTimerTaskMemory() to provide the memory that is
 * used by the RTOS daemon/time task. */
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
                                     StackType_t ** ppxTimerTaskStackBuffer,
                                     uint32_t * pulTimerTaskStackSize )
{
/* If the buffers to be provided to the Timer task are declared inside this
 * function then they must be declared static - otherwise they will be allocated on
 * the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle
     * task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
 * used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                    StackType_t ** ppxIdleTaskStackBuffer,
                                    uint32_t * pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
 * function then they must be declared static - otherwise they will be allocated on
 * the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle
     * task's state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/
/**
 * @brief Warn user if pvPortMalloc fails.
 *
 * Called if a call to pvPortMalloc() fails because there is insufficient
 * free memory available in the FreeRTOS heap.  pvPortMalloc() is called
 * internally by FreeRTOS API functions that create tasks, queues, software
 * timers, and semaphores.  The size of the FreeRTOS heap is set by the
 * configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h.
 *
 */
void vApplicationMallocFailedHook()
{
    /* The TCP tests will test behavior when the entire heap is allocated. In
     * order to avoid interfering with those tests, this function does nothing. */
}

/*-----------------------------------------------------------*/
/**
 * @brief Loop forever if stack overflow is detected.
 *
 * If configCHECK_FOR_STACK_OVERFLOW is set to 1,
 * this hook provides a location for applications to
 * define a response to a stack overflow.
 *
 * Use this hook to help identify that a stack overflow
 * has occurred.
 *
 */
void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    char * pcTaskName )
{
    xfprintf(putUart6, "Memory overflow in task - \"%s\"\n", pcTaskName);
    vTaskDelete(xTask);
    portDISABLE_INTERRUPTS();

    /* Loop forever */
    for( ; ; );
}

/*-----------------------------------------------------------*/


