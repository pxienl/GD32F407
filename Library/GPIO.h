#ifndef __GPIO_H__
#define __GPIO_H__

#include "gd32f4xx.h"

void GPIO_init(uint32_t gpio_periph, uint32_t mode, uint32_t pull_up_down, uint8_t otype, uint32_t pin);
void GPIO_input_init(uint32_t gpio_periph, uint32_t pull_up_down, uint32_t pin);
void GPIO_output_init(uint32_t gpio_periph, uint32_t pull_up_down, uint8_t otype, uint32_t pin);
void GPIO_af_init(uint32_t gpio_periph, uint32_t alt_func_num, uint32_t pull_up_down, uint32_t pin);
void GPIO_analog_init(uint32_t gpio_periph, uint32_t pull_up_down, uint32_t pin);

#endif