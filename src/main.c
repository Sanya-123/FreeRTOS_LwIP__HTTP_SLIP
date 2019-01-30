#include "common.h"
#include "stm32f4xx.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"

#include "uart.h"
#include "theardHTTP.h"
#include "xprintf.h"
#include "theardConsol.h"
#include "theardTelnetServer.h"

//#define USE_IWDG


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
#define WATCH_DOG_DELAY_MS 3000

void task_resetIWDG(void *p)//таск с нисшем приоритетом для сбрасывания WatchDog
{
    (void)p;
    while(1)
    {
        IWDG_res();
//        xprintf("ResetIWDS\n");
//        vTaskDelay(10);
    }
}
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

int main(void)
{
    initAll();

    xTaskCreate(task_diode, "Light Dioder", 64, NULL, 1, NULL);//таску моргания диодами

    xprintf("Add task diode\n");

    //run
    xTaskCreate(theard_HTTP_Server, "SLIP Server", 256, NULL, 6, NULL);//таск обработки slip

#ifdef USE_CONSOLE
    xTaskCreate(theard_Consol, "Consol", 1024, NULL, 2, NULL);
#endif
#ifdef USE_CONSOLE_TELNET
    xTaskCreate(theard_Telnet_Server, "TelnetServer", 2048, NULL, 2, NULL);
#endif

#ifdef USE_IWDG
    //run IWDS
    xTaskCreate(task_resetIWDG, "Reset IWDG", 32, NULL, 0, NULL);
#endif

    vTaskStartScheduler();

    while(1)
    {

    }
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
    configureUart(APBPERF_SPEED, UART_DEBUG_SPEED, UART_DEBUG);
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

    vTaskDelay(3000 / portTICK_PERIOD_MS);

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
    portDISABLE_INTERRUPTS();

    /* Loop forever */
    for( ; ; );
}

/*-----------------------------------------------------------*/


