#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART.h"

void RX0_recv(uint8_t *rxbuffer,uint32_t len){
    printf("receive: %s",rxbuffer);
}

int main(void)
{
    systick_config();
    usart_init(USART0,GPIOA,GPIO_AF_7,GPIO_PIN_9 | GPIO_PIN_10);

    while(1) {
        printf("test\r\n");
        send_string(USART0,"test\r\n");
        delay_1ms(1000);
    }
}
