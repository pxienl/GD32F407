#ifndef __I2C_H__
#define __I2C_H__

#include "gd32f4xx.h"

#define	I2C_HARDWARE_TIMEOUT	50000

void I2C_hardware_init();

void I2C_hardware_write(uint32_t addr, uint32_t reg,uint8_t* data, uint32_t len);
void I2C_hardware_read(uint32_t addr, uint32_t reg,uint8_t* data, uint32_t len);

#endif