#include "main.h"
#include "GPIO.h"
#include "USART.h"
#include "systick.h"
#include "SPI.h"
#include "st7789/st7789.h"
#include "cst816/cst816t.h"

void RX0_recv(uint8_t *rxbuffer, uint32_t len) {
}

int main(void) {
  nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
  systick_config();
  usart_init(USART0, GPIOA, GPIO_AF_7, GPIO_PIN_9 | GPIO_PIN_10);
  usart_dma_tx_init(USART0,DMA1,DMA_CH7);
  usart_dma_rx_init(USART0,DMA1,DMA_CH5);

  printf("init\n");

  ST7789_Init();
  ST7789_Test();

  CST816T_Init();

  while (1) {
    delay_1ms(1000);
  }
}
