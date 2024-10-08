#include "GPIO.h"

static void GPIO_select(uint32_t gpio_periph){
    if(gpio_periph == GPIOA) rcu_periph_clock_enable(RCU_GPIOA);
    else if(gpio_periph == GPIOB) rcu_periph_clock_enable(RCU_GPIOB);
    else if(gpio_periph == GPIOC) rcu_periph_clock_enable(RCU_GPIOC);
    else if(gpio_periph == GPIOD) rcu_periph_clock_enable(RCU_GPIOD);
    else if(gpio_periph == GPIOE) rcu_periph_clock_enable(RCU_GPIOE);
    else if(gpio_periph == GPIOF) rcu_periph_clock_enable(RCU_GPIOF);
    else if(gpio_periph == GPIOG) rcu_periph_clock_enable(RCU_GPIOG);
    else if(gpio_periph == GPIOH) rcu_periph_clock_enable(RCU_GPIOH);
    else if(gpio_periph == GPIOI) rcu_periph_clock_enable(RCU_GPIOI);
}

void GPIO_init(uint32_t gpio_periph, uint32_t mode, uint32_t pull_up_down, uint8_t otype, uint32_t pin){
    
    GPIO_select(gpio_periph);

    gpio_mode_set(gpio_periph, mode, pull_up_down, pin);
    gpio_output_options_set(gpio_periph, otype, GPIO_OSPEED_50MHZ, pin);
}

void GPIO_input_init(uint32_t gpio_periph, uint32_t pull_up_down, uint32_t pin){
    
    GPIO_select(gpio_periph);

    gpio_mode_set(gpio_periph, GPIO_MODE_INPUT, pull_up_down, pin);
}

void GPIO_output_init(uint32_t gpio_periph, uint32_t pull_up_down, uint8_t otype, uint32_t pin){
    
    GPIO_select(gpio_periph);

    gpio_mode_set(gpio_periph, GPIO_MODE_OUTPUT, pull_up_down, pin);
    gpio_output_options_set(gpio_periph, otype, GPIO_OSPEED_50MHZ, pin);
}

void GPIO_af_init(uint32_t gpio_periph, uint32_t alt_func_num, uint32_t pull_up_down, uint32_t pin){
    
    GPIO_select(gpio_periph);

    gpio_af_set(gpio_periph, alt_func_num, pin);
    gpio_mode_set(gpio_periph, GPIO_MODE_AF, pull_up_down, pin);
    gpio_output_options_set(gpio_periph, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, pin);
}

void GPIO_analog_init(uint32_t gpio_periph, uint32_t pull_up_down, uint32_t pin){
    GPIO_select(gpio_periph);
    gpio_mode_set(gpio_periph,GPIO_MODE_ANALOG,pull_up_down,pin);
}