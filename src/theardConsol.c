#include "theardConsol.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <stdbool.h>
#include "uart.h"

#ifdef USE_CONSOLE
#include "misc.h"
#include "microrl.h"


// create microrl object and pointer on it
microrl_t rlm;
microrl_t * prl = &rlm;

QueueHandle_t queueRxConsole = NULL;

void printUart(const char * str)
{
    int i = 0;
    while (str [i] != 0) {
        putUart6(str[i++]);
    }
}

void useFromInteruptUSART6(uint8_t data)
{
    if(queueRxConsole != NULL)
    {
        if(xQueueSendFromISR(queueRxConsole, (void *)(&data), pdTRUE) != pdTRUE)
        {
            //почему то не получилось положить в очередь
        }
    }
    else
        microrl_insert_char(prl, data);
}

#ifndef USE_RING_BUFFER_USART6
void USART6_IRQHandler(void)
{
    if (USART6->SR & USART_SR_RXNE) // Прерывание по приему?
    {
        if ((USART6->SR & (USART_SR_NE | USART_SR_FE | USART_SR_PE | USART_SR_ORE)) == 0) //проверяем нет ли ошибок
        {
            uint8_t data = (uint8_t)(USART6->DR);
            useFromInteruptUSART6(data);
        }
        else
        {
            USART6->DR; // вычитываем данные и на всякий случай
            USART6->SR &= ~USART_SR_RXNE; // ещё и сбрасываем флаг прерывания
        }
    }
}
#endif


void theard_Consol(void *p)
{
    (void)p;
    queueRxConsole = xQueueCreate(DEFOULT_SIZE_QUEUE_RX, 1);

    //**************************init uart**************************
#ifdef USE_RING_BUFFER_USART6
    setTxInterupt(UART_DEBUG, true);
#endif
    setRxInterupt(UART_DEBUG, true);
    configureUart(APBPERF_SPEED, UART_DEBUG_SPEED, UART_DEBUG);
    //****************************************************

    //***********************************init console***********************************
    //init terminal
    microrl_init (prl, printUart);
    microrl_set_execute_callback (prl, execute);
#ifdef _USE_COMPLETE
    // set callback for completion
    microrl_set_complete_callback (prl, complet);
#endif
//	 set callback for Ctrl+C
    microrl_set_sigint_callback (prl, sigint);

    //**********************************************************************

    if(queueRxConsole == NULL)//если не получилось создать очередь
        vTaskDelete(NULL);

    while(1)
    {
        uint8_t dataFromUART = NULL;
        while (xQueueReceive(queueRxConsole, (void *)(&dataFromUART), portMAX_DELAY) != pdTRUE);//жду символ
        microrl_insert_char(prl, dataFromUART);
    }
}

#endif
