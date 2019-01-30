//#pragma once
#ifndef _COMMON_H_
#define _COMMON_H_

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Warray-bounds"
#include "stm32f4xx.h"
#pragma GCC diagnostic pop

#include "gpio_F4_macros.h"
#include "gpio_F4_defines.h"

//#include "uart1.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "assert.h"
#include "xprintf.h"

#include "SEGGER_RTT.h"

#define APBPERF_SPEED       42000000
#define SYS_CLOCK_CORE      84000000

#define UART_SLIP           USART2
#define UART_DEBUG          USART6

#define UART_SLIP_SPEED     115200
#define UART_DEBUG_SPEED    115200

#define SIZE_QUEUE_RX_IN_SIO_C  1024

//#define USE_RING_BUFFER_USART6
//#define USE_CONSOLE
#define USE_CONSOLE_TELNET

#define TELNET_PORT         23
#define MAX_ABONENT_TELNET  4

//#define USE_IWDG


#endif /* _COMMON_H_ */
