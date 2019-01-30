#ifndef GPIO_DEFINES_H_
#define GPIO_DEFINES_H_

#include "gpio_F4_macros.h"

#define LED_GREEN       D,  12,  MODE_OUTPUT_PUSH_PULL, SPEED_50MHZ,    1, AF0   //
#define LED_ORANGE      D,  13,  MODE_OUTPUT_PUSH_PULL, SPEED_50MHZ,    1, AF0   //
#define LED_RED         D,  14,  MODE_OUTPUT_PUSH_PULL, SPEED_50MHZ,    1, AF0   //
#define LED_BLUE        D,  15,  MODE_OUTPUT_PUSH_PULL, SPEED_50MHZ,    1, AF0   //

#define UART2_TX        A,  2,   MODE_AF_PUSH_PULL,     SPEED_50MHZ,    0, AF7   // UART2 Tx (PA2 - 12;E5;16;(25))
#define UART2_RX        A,  3,   MODE_AF_PUSH_PULL,     SPEED_50MHZ,    0, AF7   // UART2 Rx (PA3 - 13;E4;17;(26))

#define UART6_TX        C,  6,   MODE_AF_PUSH_PULL,     SPEED_50MHZ,    0, AF8   // UART6 Tx (PC6 - ;;37;(63))
#define UART6_RX        C,  7,   MODE_AF_PUSH_PULL,     SPEED_50MHZ,    0, AF8   // UART6 Rx (PC7 - ;;37;(63))


#endif // GPIO_DEFINES_H_
