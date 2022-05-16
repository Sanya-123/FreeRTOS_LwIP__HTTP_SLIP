#ifndef UART_H
#define UART_H

#include "gpio_F4_macros.h"
#include <stdint.h>
#include "stm32f4xx.h"
#include <stdbool.h>
#include "common.h"

void initUART_PLL_IN();
void configureUart(uint32_t systemFreq, uint32_t baudrate, USART_TypeDef *uartC); 	// настройка Uart
void writeUartData(uint8_t data, USART_TypeDef * uartC);                                // Запись одного байта в UART
uint8_t readUartData(USART_TypeDef * uartC);						// Чтение одного байта из UART
void watyTransmitDone(USART_TypeDef * uartC);
void setRxInterupt(USART_TypeDef * uartC, bool en);
void setTxInterupt(USART_TypeDef * uartC, bool en);

void putUart6(uint8_t data);

#endif

