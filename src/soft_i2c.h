#ifndef _SOFT_I2C_H_
#define _SOFT_I2C_H_

#include <stdint.h>
#include <stdbool.h>

void i2c_init();
bool check_i2c(uint8_t hwAddress);
bool i2c_writeByte(uint8_t hwAddress, uint8_t regAddress, uint8_t data);
bool i2c_writeWord(uint8_t hwAddress, uint8_t regAddress, uint16_t data);
bool i2c_writeDWord(uint8_t hwAddress, uint8_t regAddress, uint32_t data);
bool i2c_readByte(uint8_t hwAddress, uint8_t regAddress, uint8_t *data);
bool i2c_readWord(uint8_t hwAddress, uint8_t regAddress, uint16_t *data);
bool i2c_readDWord(uint8_t hwAddress, uint8_t regAddress, uint32_t *data);
bool i2c_write(uint8_t hwAddress, uint8_t regAddress, uint8_t *data, uint32_t size);
bool i2c_read(uint8_t hwAddress, uint8_t regAddress, uint8_t *data, uint32_t size);

#endif // _SOFT_I2C_H_
