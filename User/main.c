#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "Timer.h"
#include "GPIO.h"

int main(void)
{
    systick_config();
    GPIO_output_init(GPIOC,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_PIN_6);
    gpio_bit_reset(GPIOC,GPIO_PIN_6);
    GPIO_af_init(GPIOE,GPIO_AF_1,GPIO_PUPD_NONE,GPIO_PIN_8 | GPIO_PIN_9);
    PWM_advanced_init(TIMER0,1,10000,TIMER_CH_0,TIMER_OC_MODE_PWM0,0);

    
    while(1) {
        for (int i = 0; i <= 100; i++)
        {
            PWM_update(TIMER0,TIMER_CH_0,Timer_get_period(TIMER0) * i / 100);
            delay_1ms(10);
        }

        PWM_state_stop(TIMER0,TIMER_CH_0);
        
    }
}
