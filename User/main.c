#include "main.h"
#include "GPIO.h"
#include "USART.h"
#include "systick.h"


void RX0_recv(uint8_t *rxbuffer, uint32_t len) {
  UNUSED(len);
  printf("%s",rxbuffer);
  fflush(stdout);
}

int main(void) {
  systick_config();
  usart_init(USART0, GPIOA, GPIO_AF_7, GPIO_PIN_9 | GPIO_PIN_10);
  usart_dma_tx_init(USART0,DMA1,DMA_CH7);
  usart_dma_rx_init(USART0,DMA1,DMA_CH5);

  GPIO_output_init(GPIOB,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_PIN_2);

  printf("init\n");

  while (1) {
    gpio_bit_toggle(GPIOB,GPIO_PIN_2);
    delay_1ms(1000);
  }
}
