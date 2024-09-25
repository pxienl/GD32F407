#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART.h"
#include "EXTI.h"

void RX0_recv(uint8_t *rxbuffer,uint32_t len){
    printf("receive: %s",rxbuffer);
}

FlagStatus pre_state = RESET;
FlagStatus state;
uint32_t tick = 0,pre_tick;
#define abs(x) ((x)>0?(x):-(x))
void EXTI0_IRQHandler_RUN(){
    // tick = system_tick_us();
    // if(abs(pre_tick-tick) < 100) {
    //     printf("pre_tick - tick = %d\n",pre_tick-tick);
    // }
    // else
        state = gpio_input_bit_get(GPIOC,GPIO_PIN_0);
        // pre_tick = tick;
        if(state != pre_state){
            if(!state){
                printf("keydown %d\n", tick);
            }else{
                printf("keyup %d\n", tick);
                exti_software_interrupt_enable(EXTI_1);
            }
            pre_state = state;
        }
}

void EXTI1_IRQHandler_RUN(){
    printf("EXTI1 interrupt\n");
    exti_software_interrupt_disable(EXTI_1);
}

int main(void)
{
    systick_config();
    usart_init(USART0,GPIOA,GPIO_AF_7,GPIO_PIN_9 | GPIO_PIN_10);
    EXTI_Init(GPIOC,EXTI_0,1,1,EXTI_TRIG_BOTH);
    EXTI_soft_init(EXTI_1,1,1,EXTI_TRIG_BOTH);

    while(1) {
        delay_1ms(1000);
    }
}
