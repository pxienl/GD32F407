#include "main.h"
#include "systick.h"
#include "DMA.h"
#include "USART.h"
#include "GPIO.h"
#include "I2C.h"
#include "i2c_oled.h"

void RX0_recv(uint8_t *rxbuffer,uint32_t len){
    usart_dma_send(USART0,rxbuffer,len);
}

int main(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    systick_config();
    usart_init(USART0,GPIOA,GPIO_AF_7,GPIO_PIN_9 | GPIO_PIN_10);
    usart_dma_tx_init(USART0,DMA1,DMA_CH7);
    usart_dma_rx_init(USART0,DMA1,DMA_CH5);

    printf("init");
    
    GPIO_af_init(GPIOB,GPIO_AF_4,GPIO_PUPD_PULLUP,GPIO_PIN_6|GPIO_PIN_7);

    uint8_t t=' ';
	delay_1ms(500);
 	OLED_Init();				//初始化OLED
	while(1) 
	{
		
	}
}
