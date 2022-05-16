#include "uart.h"

#if defined(USE_CONSOLE) && defined(USE_RING_BUFFER_USART6)
extern void useFromInteruptUSART6(uint8_t data);
#endif


//==================================================================================================

#ifdef USE_RING_BUFFER_USART6
    #define TX_BUFFER_SIZE 256 //размер буфера на передачу

    // Буфер на передачу
    static volatile uint8_t  tx_buffer[TX_BUFFER_SIZE]; // сам буфер
    static volatile uint16_t tx_wr_index = 0; // индекс хвоста буфера (куда писать данные)
    static volatile uint16_t tx_rd_index = 0; // индекс начала буфера (откуда читать данные)
    static volatile uint16_t tx_counter = 0; // количество данных в буфере
#endif

//--------------------------------------------------------------------------------------------------


void initUART_PLL_IN()
{
    RCC->APB2ENR |= RCC_APB2ENR_USART6EN;	// Разрешаем тактирование UART
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    NVIC_EnableIRQ(USART6_IRQn);
    NVIC_EnableIRQ(USART2_IRQn);
    //для вызова функции операционки через прерывания
    //должны быть >configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
    NVIC_SetPriority(USART6_IRQn, 6);
    NVIC_SetPriority(USART2_IRQn, 5);
}

void configureUart(uint32_t systemFreq, uint32_t baudrate, USART_TypeDef * uartC)
{
    // Обычная конструкция вида USART1->BRR = systemFreq/baudrate; даёт ошибку округления,
    // чтобы этого избежать, прибавим 0,5 для получения ближайшего целого.
    // в целочисленной арифметике этому соответcвует следующая конструкция:
        uartC->BRR = (2*systemFreq/baudrate + 1)/2;

        uartC->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;	// включаем UART, разрешаем работу приёмника и передатчика
    //interupt for UART

}

void writeUartData(uint8_t data, USART_TypeDef * uartC)
{
        while((uartC->SR & USART_SR_TXE) == RESET){};	// Дожидаемся освобождения буфера передатчика
        uartC->DR = data;
}

uint8_t readUartData(USART_TypeDef * uartC)
{
        while((uartC->SR & USART_SR_RXNE) == RESET) {}; // Дожидаемся, пока буфер приёмника чем-то заполнится
        return uartC->DR;
}


void watyTransmitDone(USART_TypeDef * uartC)
{
    while((uartC->SR & USART_SR_TC) == RESET) {};
}

void setRxInterupt(USART_TypeDef * uartC, bool en)
{
    if(en)
        uartC->CR1 |= USART_CR1_RXNEIE;//interupt RX en
    else
         uartC->CR1 &= ~USART_CR1_RXNEIE;//interupt RX dis
}

void setTxInterupt(USART_TypeDef * uartC, bool en)
{
    if(en)
        uartC->CR1 |= USART_CR1_TXEIE;//interupt TX en
    else
         uartC->CR1 &= ~USART_CR1_TXEIE;//interupt TX dis
}

void putUart6(uint8_t data)
{
#ifdef USE_RING_BUFFER_USART6
    while (tx_counter == TX_BUFFER_SIZE) {}; //если буфер переполнен, ждем
    if (tx_counter || (!(USART6->SR & USART_SR_TXE))) //если в буфере уже что-то есть или если в данный момент что-то уже передается
    {
        tx_buffer[tx_wr_index++] = data; //то кладем данные в буфер
        if (tx_wr_index == TX_BUFFER_SIZE)
        {
            tx_wr_index = 0;    //идем по кругу
        }
        USART6->CR1 &= ~USART_CR1_TXEIE;//запрещаем прерывание, чтобы оно не мешало менять переменную
        ++tx_counter;                   //увеличиваем счетчик количества данных в буфере
        USART6->CR1 |= USART_CR1_TXEIE; // разрешаем прерывание
    }
    else //если USART1 свободен
    {
        while (!(USART6->SR & USART_SR_TXE)) {}; // Проверка что буфер на передачу опустел
        USART6->DR = data;    // передаем данные без прерывания
    }

#else
    writeUartData(data, USART6);
#endif
}


#ifdef USE_RING_BUFFER_USART6
void USART6_IRQHandler(void)
{
    if (USART6->SR & USART_SR_RXNE) // Прерывание по приему?
    {

        if ((USART6->SR & (USART_SR_NE | USART_SR_FE | USART_SR_PE)) == 0) //проверяем нет ли ошибок
        {
#ifdef USE_CONSOLE
            uint8_t data = (uint8_t)(USART6->DR);
            useFromInteruptUSART6(data);
#endif
        }
        else
        {
            USART6->DR; // вычитываем данные и на всякий случай
            USART6->SR &= ~USART_SR_RXNE; // ещё и сбрасываем флаг прерывания
        }
    }

    if (USART6->SR & USART_SR_TXE) //прерывание по передачи
    {
        if (tx_counter) //если есть что передать
        {
            --tx_counter; // уменьшаем количество не переданных данных
            USART6->DR = tx_buffer[tx_rd_index++]; //передаем данные инкрементируя хвост буфера
            if (tx_rd_index == TX_BUFFER_SIZE)
            {
                tx_rd_index = 0;    //идем по кругу
            }
        }
        else //если нечего передать,
        {
            USART6->CR1 &= ~USART_CR1_TXEIE; //запрещаем прерывание по передачи
        }
    }

    if (USART6->SR & USART_SR_ORE) //прерывание по переполнению буфера
    {
        USART6->DR; // вычитываем данные и на всякий случай
        USART6->SR &= ~USART_SR_ORE; // ещё и сбрасываем флаг прерывания
    }
}
#endif
