#ifndef __TIMER_H__
#define __TIMER_H__

#include "gd32f4xx.h"

void Timer_init(uint32_t TIMERx, uint16_t prescaler,uint32_t period,
                    uint8_t nvic_irq,uint8_t nvic_irq_pre_priority,
                    uint8_t nvic_irq_sub_priority,uint32_t interrupt);

uint32_t Timer_get_period(uint32_t TIMERx);

void PWM_init(uint32_t TIMERx,uint16_t prescaler, uint32_t period,
                  uint16_t TIMER_CH_x, uint16_t TIMER_OC_MODE,uint16_t pulse);

void PWM_update(uint32_t TIMERx, uint16_t TIMER_CH_x, uint16_t pulse);
void PWM_state_start(uint32_t TIMERx, uint16_t TIMER_CH_x);
void PWM_state_stop(uint32_t TIMERx, uint16_t TIMER_CH_x);
#endif