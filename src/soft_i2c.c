#include "soft_i2c.h"
#include "gpio_F4_defines.h"
#include "uart.h"

#ifndef I2C_SDA_SOFT
#define I2C_SDA_SOFT        A,  0,  MODE_OUTPUT_PUSH_PULL, SPEED_50MHZ,    1, AF0
#endif

#ifndef I2C_SCL_SOFT
#define I2C_SCL_SOFT        A,  0,  MODE_OUTPUT_PUSH_PULL, SPEED_50MHZ,    1, AF0
#endif


#define NACK 0
#define ACK 1
#define WRITE 0
#define READ 1

/*TIMER for i2c*/
static volatile uint32_t SK_counter;
static const uint8_t max_repeat = 3;

void timer_init()
{       
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;         //подать тактирование на TIM3
    TIM3->PSC     = SystemCoreClock/1000000-1;  //настроить делитель для формирования us
}

void delay_us(uint16_t us)
{
    TIM3->ARR = us;                         //Устанавливаем значение переполнения таймера, а значит и значение при котором генерируется Событие обновления
    TIM3->EGR |= TIM_EGR_UG;                //Генерируем Событие обновления для записи данных в регистры PSC и ARR
    TIM3->CR1 |= TIM_CR1_CEN|TIM_CR1_OPM;	//Запускаем таймер записью бита CEN и устанавливаем режим Одного прохода установкой бита OPM
    while ((TIM3->CR1 & TIM_CR1_CEN) != 0);
}

//void TIM4_IRQHandler(void)
//{
//    // FIXME таймер 15 можно использовать без прерывания - настроить его на счёт
//    // от заданного значения к нулю без рестарта и просто дождаться, пока значение CNT не станет равным нулю.

//    if (SK_counter)
//    {
//        SK_counter--;
//    }
//    TIM4->CNT = 0;
//    TIM4->SR &= ~(TIM_SR_UIF);//clear flag interupt
//}

//------------------------------------------------------------------
// I2C Delay
static void i2c_Wait()
{
    delay_us(3);
}

//------------------------------------------------------------------
// I2C SDA SCL Control
//------------------------------------------------------------------
static void SetLowSDA()
{
//    PIN_CONFIGURATION(I2C_SDA_DOWN);
    PIN_OFF(I2C_SDA_SOFT);
    i2c_Wait();
}

static void SetHighSDA()
{
//    PIN_CONFIGURATION(I2C_SDA_UP);
    PIN_ON(I2C_SDA_SOFT);
    i2c_Wait();
}

static void SetLowSCL()
{
//    PIN_CONFIGURATION(I2C_SCL_DOWN);
    PIN_OFF(I2C_SCL_SOFT);
    i2c_Wait();
}

static void SetHighSCL()
{
//    PIN_CONFIGURATION(I2C_SCL_UP);
    PIN_ON(I2C_SCL_SOFT);
    i2c_Wait();
}

//------------------------------------------------------------------
// I2C Start Data Transfer
//------------------------------------------------------------------
static void i2c_start(void)
{
    SetHighSCL();
    SetHighSDA();

    SetHighSCL();
    SetLowSDA();

    SetLowSCL();
    SetHighSDA();
}

//------------------------------------------------------------------
// I2C Stop  Transfer
//------------------------------------------------------------------
static void i2c_stop(void)
{
    SetLowSCL();
    SetLowSDA();

    SetHighSCL();
    SetLowSDA();

    SetHighSCL();
    SetHighSDA();
}


void I2C2_ResetBus(void)
{
    char i;

    SetHighSCL();
    SetHighSDA();

    i2c_start();

    for (i = 0; i < 9; i++) /* up to 9 clocks until SDA goes high */
    {
        SetLowSCL();
        SetHighSCL();
    }

    i2c_start();
}

//------------------------------------------------------------------
// I2C Write  Transfer
//------------------------------------------------------------------
static int16_t _i2c_write(uint8_t a)
{
    int16_t i;
    int16_t return_ack;

    for (i = 0; i < 8; i++)
    {
        SetLowSCL();
        if (a & 0x80)
        {
            SetHighSDA();
        }
        else
        {
            SetLowSDA();
        }

        SetHighSCL();
        a <<= 1;
    }
    SetLowSCL();

    /* ack Read */
    SetHighSDA();
    SetHighSCL();

    if (PIN_ISSET(I2C_SDA_SOFT))
    {
        return_ack = NACK;
    }
    else
    {
        return_ack = ACK;
    }

    SetLowSCL();

    return (return_ack);
}

//------------------------------------------------------------------
// I2C Read  Transfer
//------------------------------------------------------------------
static uint8_t _i2c_read(int16_t ack)
{
    int16_t i;
    uint8_t caracter = 0x00;

    SetLowSCL();
    SetHighSDA();

    for (i = 0; i < 8; i++)
    {
        caracter = caracter << 1;
        SetHighSCL();

        if (PIN_ISSET(I2C_SDA_SOFT))
        {
            caracter = caracter + 1;
        }

        SetLowSCL();
    }

    if (ack)
    {
        SetLowSDA();
    }
    SetHighSCL();
    SetLowSCL();

    return (caracter);
}

bool i2c_write(uint8_t hwAddress, uint8_t regAddress, uint8_t *data, uint32_t size)
{
    bool state;

    for (uint8_t i=0; i< max_repeat; i++)
    {
        state = true;

        i2c_start();
        state &= _i2c_write(hwAddress | WRITE);
        state &= _i2c_write(regAddress);
        for (uint32_t i = 0; i < size; i++)
        {
            state &= _i2c_write(data[size - 1 - i]);
        }
        i2c_stop();

        if (state == true)
        {
            break;
        }
        else
        {
            // TODO сообщение об ошибке
            I2C2_ResetBus();
        }
    }

    return state;
}

bool i2c_read(uint8_t hwAddress, uint8_t regAddress, uint8_t *data, uint32_t size)
{
    bool state;

    for (uint8_t i=0; i< max_repeat; i++)
    {
        state = true;
        i2c_start();
        state &= _i2c_write(hwAddress | WRITE);
        state &= _i2c_write(regAddress);
        i2c_start();
        state &= _i2c_write(hwAddress | READ);
        for (uint32_t i = 0; i < (size - 1); i++)
        {
            data[size - i -1] = _i2c_read(ACK);
        }
        data[0] = _i2c_read(NACK);
        i2c_stop();

        if (state == true)
        {
            break;
        }
        else
        {
            // TODO сообщение об ошибке
            I2C2_ResetBus();
        }
    }

    return state;
}

bool i2c_readByte(uint8_t hwAddress, uint8_t regAddress, uint8_t *data)
{
    return i2c_read(hwAddress, regAddress, (uint8_t *)data, sizeof(uint8_t));
}

bool i2c_readWord(uint8_t hwAddress, uint8_t regAddress, uint16_t *data)
{
    return i2c_read(hwAddress, regAddress, (uint8_t *)data, sizeof(uint16_t));
}

bool i2c_readDWord(uint8_t hwAddress, uint8_t regAddress, uint32_t *data)
{
    return i2c_read(hwAddress, regAddress, (uint8_t *)data, sizeof(uint32_t));
}

bool i2c_writeByte(uint8_t hwAddress, uint8_t regAddress, uint8_t data)
{
    return i2c_write(hwAddress, regAddress, (uint8_t *)&data, sizeof(uint8_t));
}

bool i2c_writeWord(uint8_t hwAddress, uint8_t regAddress, uint16_t data)
{
    return i2c_write(hwAddress, regAddress, (uint8_t *)&data, sizeof(uint16_t));
}

bool i2c_writeDWord(uint8_t hwAddress, uint8_t regAddress, uint32_t data)
{
    return i2c_write(hwAddress, regAddress, (uint8_t *)&data, sizeof(uint32_t));
}

//------------------------------------------------------------------
// I2C Initialize Bus
//------------------------------------------------------------------
void i2c_init()
{
    PIN_CONFIGURATION(I2C_SCL_SOFT);
    PIN_CONFIGURATION(I2C_SDA_SOFT);
    timer_init();
    SetHighSCL();
    SetLowSDA();
    SetHighSDA();

    I2C2_ResetBus();
}

bool check_i2c(uint8_t hwAddress)
{
    i2c_start();
    bool is = _i2c_write(hwAddress | READ);
    i2c_stop();
    return is;
}
