#ifndef _GPIO_MACROS_F4_H_
#define _GPIO_MACROS_F4_H_

//#include "stm32f4xx.h"

/*
 Данный набор макросов упрощает работу с GPIO портами микроконтроллеров STM32F4xx
 Протестировано на STM32F100 STM32F103

 За основу взято:
 http://we.easyelectronics.ru/STM32/udobnaya-nastroyka-gpio-portov.html#comment114408
 http://ziblog.ru/2012/09/09/linii-vvoda-vyivoda-stm32-chast-3.html
 http://we.easyelectronics.ru/Soft/preprocessor-c.html
 https://www.opennet.ru/docs/RUS/cpp/

 Определение GPIO должно выглядить так:
 #define PIN_NAME PORT, PIN, MODE, SPEED, DEFAULT_STATE, AF
 PORT - A, B, C, D и тд
 PIN - номер бита в порте
 MODE - режим выхода
 SPEED - скорость (актуально только для выхода)
 DEFAULT_STATE - значение поумолчанию для настроенных на выход пинов
 AF - альтернативная функция

 Пример: #define PIN_LED C, 9, MODE_OUTPUT_PUSH_PULL, SPEED_2MHZ,   0
 Пример: #define BUTTON1 C, 8, MODE_INPUT_PULL_UP,    SPEED_2MHZ,   0

 Доступные операции:
 PIN_CONFIGURATION(PIN_NAME) - настройка gpio
 PIN_ON(PIN_NAME)            - включить (установка лог. 1)
 PIN_OFF(PIN_NAME)           - выключить (установка лог. 0)
 PIN_TOGGLE(PIN_NAME)        - переключить состояние
 PIN_ISSET_IN - состояние порта снаружи мирросхемы
 PIN_ISSET_OUT - состояние порта, установленное в микросхеме
 Последние два используются только в конструкциях вида if(PIN_ISSET(PIN_NAME))
*/

//--------------------------------------------------------------------------------------------------
// Доступные настройки GPIO

// Режимы
#define MODE_OUTPUT_PUSH_PULL               MODE_OUTPUT_PUSH_PULL
#define MODE_OUTPUT_OPEN_DRAIN              MODE_OUTPUT_OPEN_DRAIN
#define MODE_AF_PUSH_PULL                   MODE_AF_PUSH_PULL
#define MODE_AF_OPEN_DRAIN                  MODE_AF_OPEN_DRAIN
#define MODE_INPUT_FLOATING                 MODE_INPUT_FLOATING
#define MODE_INPUT_PULL_UP                  MODE_INPUT_PULL_UP
#define MODE_INPUT_PULL_DOWN                MODE_INPUT_PULL_DOWN
#define MODE_ANALOG                         MODE_ANALOG

// Скорости
#define SPEED_2MHZ                          2UL
#define SPEED_10MHZ                         1UL
#define SPEED_50MHZ                         3UL

// Ветви альтернативных функций
#define AF0                                 0
#define AF1                                 1
#define AF2                                 2
#define AF3                                 3
#define AF4                                 4
#define AF5                                 5
#define AF6                                 6
#define AF7                                 7
#define AF8                                 8
#define AF9                                 9
#define AF10                                10
#define AF11                                11
#define AF12                                12
#define AF13                                13
#define AF14                                14
#define AF15                                15

//--------------------------------------------------------------------------------------------------
// Установка параметров GPIO
/*
 Традиционное обнуление-присвоение:
 { GPIO##PORT->CRL &= ~(15UL << 0); GPIO##PORT->CRL |= ((MODE) << 0); }
 эквивалентно записи
 { GPIO##PORT->CRL = ( GPIO##PORT->CRL & (~(15UL << 0)) ) | ((MODE) << 0); }
  но без промежуточных состояний порта, что гораздо безопаснее
*/
#define _GPIO_PIN0_CONFIGURATION(PORT, AF) \
    GPIO##PORT->AFR[0] = ( GPIO##PORT->AFR[0] & (~(15UL << 0)) ) | ((AF) << 0);

#define _GPIO_PIN1_CONFIGURATION(PORT, AF) \
    GPIO##PORT->AFR[0] = ( GPIO##PORT->AFR[0] & (~(15UL << 4)) ) | ((AF) << 4);

#define _GPIO_PIN2_CONFIGURATION(PORT, AF) \
    GPIO##PORT->AFR[0] = ( GPIO##PORT->AFR[0] & (~(15UL << 8)) ) | ((AF) << 8);

#define _GPIO_PIN3_CONFIGURATION(PORT, AF) \
    GPIO##PORT->AFR[0] = ( GPIO##PORT->AFR[0] & (~(15UL << 12)) ) | ((AF) << 12);

#define _GPIO_PIN4_CONFIGURATION(PORT, AF) \
    GPIO##PORT->AFR[0] = ( GPIO##PORT->AFR[0] & (~(15UL << 16)) ) | ((AF) << 16);

#define _GPIO_PIN5_CONFIGURATION(PORT, AF) \
    GPIO##PORT->AFR[0] = ( GPIO##PORT->AFR[0] & (~(15UL << 20)) ) | ((AF) << 20);

#define _GPIO_PIN6_CONFIGURATION(PORT, AF) \
    GPIO##PORT->AFR[0] = ( GPIO##PORT->AFR[0] & (~(15UL << 24)) ) | ((AF) << 24);

#define _GPIO_PIN7_CONFIGURATION(PORT, AF) \
    GPIO##PORT->AFR[0] = ( GPIO##PORT->AFR[0] & (~(15UL << 28)) ) | ((AF) << 28);

#define _GPIO_PIN8_CONFIGURATION(PORT, AF) \
    GPIO##PORT->AFR[1] = ( GPIO##PORT->AFR[1] & (~(15UL << 0)) ) | ((AF) << 0);

#define _GPIO_PIN9_CONFIGURATION(PORT, AF) \
    GPIO##PORT->AFR[1] = ( GPIO##PORT->AFR[1] & (~(15UL << 4)) ) | ((AF) << 4);

#define _GPIO_PIN10_CONFIGURATION(PORT, AF) \
    GPIO##PORT->AFR[1] = ( GPIO##PORT->AFR[1] & (~(15UL << 8)) ) | ((AF) << 8);

#define _GPIO_PIN11_CONFIGURATION(PORT, AF) \
    GPIO##PORT->AFR[1] = ( GPIO##PORT->AFR[1] & (~(15UL << 12)) ) | ((AF) << 12);

#define _GPIO_PIN12_CONFIGURATION(PORT, AF) \
    GPIO##PORT->AFR[1] = ( GPIO##PORT->AFR[1] & (~(15UL << 16)) ) | ((AF) << 16);

#define _GPIO_PIN13_CONFIGURATION(PORT, AF) \
    GPIO##PORT->AFR[1] = ( GPIO##PORT->AFR[1] & (~(15UL << 20)) ) | ((AF) << 20);

#define _GPIO_PIN14_CONFIGURATION(PORT, AF) \
    GPIO##PORT->AFR[1] = ( GPIO##PORT->AFR[1] & (~(15UL << 24)) ) | ((AF) << 24);

#define _GPIO_PIN15_CONFIGURATION(PORT, AF) \
    GPIO##PORT->AFR[1] = ( GPIO##PORT->AFR[1] & (~(15UL << 28)) ) | ((AF) << 28);

//--------------------------------------------------------------------------------------------------
// Настройка режима GPIO
/*
 Традиционное обнуление-присвоение:
 { GPIO##PORT->ODR &= ~(1UL << PIN); GPIO##PORT->ODR |= ((DEFAULT_STATE & 1UL) << PIN); }
 эквивалентно записи
 { GPIO##PORT->ODR = ( GPIO##PORT->ODR & (~(1UL << PIN)) ) | ((DEFAULT_STATE & 1UL) << PIN); }
  но без промежуточных состояний порта, что гораздо безопаснее
*/
#define _SET_MODE_OUTPUT_PUSH_PULL(PORT, PIN, MODE, SPEED, DEFAULT_STATE) \
    GPIO##PORT->MODER = ( GPIO##PORT->MODER & (~(3UL << (PIN * 2))) ) | (1UL << (PIN * 2)); \
    GPIO##PORT->OTYPER = ( GPIO##PORT->OTYPER & (~(1UL << PIN )) ); \
    GPIO##PORT->OSPEEDR = ( GPIO##PORT->OSPEEDR & (~(3UL << (PIN * 2))) ) | (SPEED << (PIN * 2)); \
    GPIO##PORT->ODR = ( GPIO##PORT->ODR & (~(1UL << PIN)) ) | ((DEFAULT_STATE & 1UL) << PIN);

#define _SET_MODE_OUTPUT_OPEN_DRAIN(PORT, PIN, MODE, SPEED, DEFAULT_STATE) \
    _SET_MODE_OUTPUT_PUSH_PULL(PORT, PIN, MODE, SPEED, DEFAULT_STATE) \
    GPIO##PORT->OTYPER = ( GPIO##PORT->OTYPER | (1UL << PIN ) );

#define _SET_MODE_AF_PUSH_PULL(PORT, PIN, MODE, SPEED, DEFAULT_STATE) \
    GPIO##PORT->MODER = ( GPIO##PORT->MODER & (~(3UL << (PIN * 2))) ) | (2UL << (PIN * 2)); \
    GPIO##PORT->OTYPER = ( GPIO##PORT->OTYPER & (~(1UL << PIN )) ); \
    GPIO##PORT->OSPEEDR = ( GPIO##PORT->OSPEEDR & (~(3UL << (PIN * 2))) ) | (SPEED << (PIN * 2));

#define _SET_MODE_AF_OPEN_DRAIN(PORT, PIN, MODE, SPEED, DEFAULT_STATE) \
    _SET_MODE_AF_PUSH_PULL(PORT, PIN, MODE, SPEED, DEFAULT_STATE) \
    GPIO##PORT->OTYPER = ( GPIO##PORT->OTYPER | (1UL << PIN ) );

#define _SET_MODE_INPUT_FLOATING(PORT, PIN, MODE, SPEED, DEFAULT_STATE) \
    GPIO##PORT->MODER = ( GPIO##PORT->MODER & (~(3UL << (PIN * 2))) ); \
    GPIO##PORT->PUPDR = ( GPIO##PORT->PUPDR & (~(3UL << (PIN * 2))) );

#define _SET_MODE_INPUT_PULL_UP(PORT, PIN, MODE, SPEED, DEFAULT_STATE) \
    _SET_MODE_INPUT_FLOATING(PORT, PIN, MODE, SPEED, DEFAULT_STATE) \
    GPIO##PORT->PUPDR = ( GPIO##PORT->PUPDR | (1UL << (PIN * 2)) );

#define _SET_MODE_INPUT_PULL_DOWN(PORT, PIN, MODE, SPEED, DEFAULT_STATE) \
    _SET_MODE_INPUT_FLOATING(PORT, PIN, MODE, SPEED, DEFAULT_STATE) \
    GPIO##PORT->PUPDR = ( GPIO##PORT->PUPDR | (2UL << (PIN * 2)) );

#define _SET_MODE_INPUT_ANALOG(PORT, PIN, MODE, SPEED, DEFAULT_STATE) \
    GPIO##PORT->MODER = ( GPIO##PORT->MODER | (3UL << (PIN * 2)) ); \
    GPIO##PORT->PUPDR = ( GPIO##PORT->PUPDR & (~(3UL << (PIN * 2))) );

//--------------------------------------------------------------------------------------------------
// Реализация макросов

#define _PIN_CONFIGURATION(PORT, PIN, MODE, SPEED, DEFAULT_STATE, AF) \
    do { _SET_##MODE(PORT, PIN, MODE, SPEED, DEFAULT_STATE) \
        _GPIO_PIN##PIN##_CONFIGURATION(PORT, AF)              } while (0)

#define _PIN_ON(PORT, PIN, MODE, SPEED, DEFAULT_STATE, AF) \
    do { GPIO##PORT->BSRR = (1UL << PIN); } while (0)

#define _PIN_OFF(PORT, PIN, MODE, SPEED, DEFAULT_STATE, AF) \
    do { GPIO##PORT->BSRR = (1UL << (PIN + 16)); } while (0)

#define _PIN_TOGGLE(PORT, PIN, MODE, SPEED, DEFAULT_STATE, AF) \
    do { GPIO##PORT->ODR ^= (1UL << PIN); } while (0)

//#define _PIN_ISSET_IN(PORT, PIN, MODE, SPEED, DEFAULT_STATE, AF)
//    ( GPIO##PORT->ODR & (1UL << PIN) )

#define _PIN_ISSET_OUT(PORT, PIN, MODE, SPEED, DEFAULT_STATE, AF) \
    ( GPIO##PORT->IDR & (1UL << PIN) )

//--------------------------------------------------------------------------------------------------
// Переопределение макросов

#define PIN_CONFIGURATION(PIN_DESCRIPTION)  _PIN_CONFIGURATION(PIN_DESCRIPTION)
#define PIN_ON(PIN_DESCRIPTION)             _PIN_ON(PIN_DESCRIPTION)
#define PIN_OFF(PIN_DESCRIPTION)            _PIN_OFF(PIN_DESCRIPTION)
#define PIN_TOGGLE(PIN_DESCRIPTION)         _PIN_TOGGLE(PIN_DESCRIPTION)
//#define PIN_ISSET_IN(PIN_DESCRIPTION)       _PIN_ISSET_IN(PIN_DESCRIPTION)
#define PIN_ISSET(PIN_DESCRIPTION)      _PIN_ISSET_OUT(PIN_DESCRIPTION)

#endif // _GPIO_MACROS_F4_H_

