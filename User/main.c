#include "main.h"
#include "systick.h"
#include "DMA.h"
#include "USART.h"
#include "WDGT.h"

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

    FWDGT_init(2000,FWDGT_PSC_DIV64); // wait 4s to reboot

    while(1) {
        delay_1ms(3500);
        fwdgt_counter_reload();
        printf("feed");
    }
}
