#include "main.h"
#include "systick.h"
#include "DMA.h"
#include "USART.h"
#include "ADC.h"
#include "GPIO.h"

void RX0_recv(uint8_t *rxbuffer,uint32_t len){
  printf("RX0 recv");
}

int main(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    systick_config();
    usart_init(USART0,GPIOA,GPIO_AF_7,GPIO_PIN_9 | GPIO_PIN_10);
    usart_dma_tx_init(USART0,DMA1,DMA_CH7);
    // usart_dma_rx_init(USART0,DMA1,DMA_CH5);

    // uint16_t buffer[2] = {0};
    // uint8_t channels[] = {ADC_CHANNEL_14,ADC_CHANNEL_16};

    GPIO_analog_init(GPIOA,GPIO_PUPD_NONE,GPIO_PIN_1);
    ADC_single_init(ADC0,ADC_CHANNEL_1);
    
    // ADC_multiple_dma_init(ADC0, channels,2, buffer,DMA1,DMA_CH4,DMA_SUBPERI0);


    while(1) {
      printf("%d\n", ADC_get(ADC0,ADC_INSERTED_CHANNEL_0));
      delay_1ms(10);
    }
}
