#include "lwip/sio.h"
#include "uart.h"
#include "netif/slipif.h"
#include "common.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

void task_rrintErrorUart(void *p);

QueueHandle_t queueUART = NULL;//очередь приемного буфера

sio_fd_t sio_open(u8_t devnum)
{
    LWIP_UNUSED_ARG(devnum);
    setRxInterupt(UART_SLIP, true);
    configureUart(APBPERF_SPEED, UART_SLIP_SPEED, UART_SLIP);
    LWIP_DEBUGF(SLIP_DEBUG, ("slipif_init:speed UART sleep = %"U32_F" \n", UART_SLIP_SPEED));

    //инициализация очериди для приема
    queueUART = xQueueCreate(/*configQUEUE_REGISTRY_SIZE*/SIZE_QUEUE_RX_IN_SIO_C, 1);
    LWIP_ASSERT("Error initialization queue for UART!!!\n(whit out queue ferst 5 bytes[2-7] don't have time to process)\n", queueUART != NULL);

    xTaskCreate(task_rrintErrorUart, "print error uart", 64, NULL, 1, NULL);

    return 1;
}

void sio_send(u8_t c, sio_fd_t fd)
{
    LWIP_UNUSED_ARG(fd);
    writeUartData(c, UART_SLIP);
}

u8_t sio_recv(sio_fd_t fd)
{
    LWIP_UNUSED_ARG(fd);
    uint8_t dataFromUART = NULL;
    while (xQueueReceive(queueUART, (void *)(&dataFromUART), portMAX_DELAY) != pdTRUE);
    return dataFromUART;
}

u32_t sio_read(sio_fd_t fd, u8_t *data, u32_t len)
{
    LWIP_UNUSED_ARG(fd);

    for(u32_t i = 0; i < len; i++)
        data[i] = sio_recv(fd);

    return len;
}

u32_t sio_tryread(sio_fd_t fd, u8_t *data, u32_t len)
{
    return sio_read(fd, data, len);
}

u32_t sio_write(sio_fd_t fd, u8_t *data, u32_t len)
{
    LWIP_UNUSED_ARG(fd);

    for(u32_t i = 0; i < len; i++)
        sio_send(data[i], fd);

    return len;
}

uint32_t errorUartSR = 0;
#define FLAG_ERROR_QUEUE        (1 << 16)

void USART2_IRQHandler(void)
{
    if (UART_SLIP->SR & USART_SR_RXNE)                     // Прерывание по приему?
    {
        if ((UART_SLIP->SR & (USART_SR_NE | USART_SR_FE | USART_SR_PE | USART_SR_ORE)) == 0) //проверяем нет ли ошибок
        {
            uint8_t data = (uint8_t)(UART_SLIP->DR); //считываем данные в буфер, инкрементируя хвост буфера
            if (queueUART != NULL) //если у меня нет очереди
            {
                if(xQueueSendFromISR(queueUART, (void *)(&data), pdTRUE) != pdTRUE)    //если есть очередь то кладу информацию в нее
                {
                    //почему то не получилось положить в очередь
                    errorUartSR |= FLAG_ERROR_QUEUE;
                }
            }
        }
        else
        {
            errorUartSR |= UART_SLIP->SR;
            (void)UART_SLIP->DR;                                 // вычитываем данные и на всякий случай
            UART_SLIP->SR &= ~USART_SR_RXNE;               // ещё и сбрасываем флаг прерывания
        }

        //по пиему данных надо делать чтобы отправлялись в функцию в uart.c
    }
}

void task_rrintErrorUart(void *p)
{
    (void)p;

    while(1)
    {
        if(errorUartSR != 0)
        {
            if(errorUartSR & FLAG_ERROR_QUEUE)
                xprintf("UART ERROR:queue can't send from ISR\n");
            if(errorUartSR & USART_SR_ORE)
                xprintf("UART ERROR:Rx bufer overflow\n");
            //... other error

            //...

            errorUartSR = 0;
        }
        vTaskDelay(100 / portTICK_RATE_MS);
    }
}

