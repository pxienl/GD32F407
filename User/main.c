#include "main.h"
#include "GPIO.h"
#include "USART.h"
#include "systick.h"
#include "SPI.h"
#include "spi_oled.h"

void RX0_recv(uint8_t *rxbuffer, uint32_t len) {
}

int main(void) {
  nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
  systick_config();
  usart_init(USART0, GPIOA, GPIO_AF_7, GPIO_PIN_9 | GPIO_PIN_10);
  usart_dma_tx_init(USART0,DMA1,DMA_CH7);
  usart_dma_rx_init(USART0,DMA1,DMA_CH5);

  printf("init\n");

  SPI_dma_transmit_init(SPI0,DMA1,DMA_CH3);
  SPI_dma_receive_init(SPI0,DMA1,DMA_CH2);
  SPI_OLED_Init();
  SPI_OLED_ColorTurn(1);
  SPI_OLED_Display_string_5x7(0,0,"SPI");
  

  while (1) {
    delay_1ms(1000);
  }
}
