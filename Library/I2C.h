#ifndef __I2C_H__
#define __I2C_H__

#include "gd32f4xx.h"

#define	I2C_HARDWARE_TIMEOUT	50000

void I2C_hardware_init(uint32_t i2c_periph,uint32_t speed);

uint8_t I2C_hardware_start(uint32_t i2c_periph,uint8_t checkBSY);
uint8_t I2C_hardware_stop(uint32_t i2c_periph);
uint8_t I2C_hardware_address(uint32_t i2c_periph, uint32_t addr, uint32_t trandirection);
uint8_t I2C_hardware_send(uint32_t i2c_periph, uint8_t data);

void I2C_hardware_write(uint32_t i2c_periph, uint32_t addr, uint32_t reg,uint8_t* data, uint32_t len);
void I2C_hardware_read(uint32_t i2c_periph, uint32_t addr, uint32_t reg,uint8_t* data, uint32_t len);

typedef struct{
    uint32_t SCL_GPIO;
    uint32_t SCL_PIN;
    uint32_t SDA_GPIO;
    uint32_t SDA_PIN;
    uint32_t delay_us;
}I2C_soft_struct;

I2C_soft_struct I2C_soft_init(uint32_t SCL_GPIO, uint32_t SCL_PIN,uint32_t SDA_GPIO, uint32_t SDA_PIN, uint32_t freq);
void I2C_soft_write(I2C_soft_struct* i2c,uint32_t addr, uint32_t reg,uint8_t* data, uint32_t len);
void I2C_soft_read(I2C_soft_struct* i2c,uint32_t addr, uint32_t reg,uint8_t* data, uint32_t len);

#endif