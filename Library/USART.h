#ifndef __USART_H__
#define __USART_H__

#include "gd32f4xx.h"

#define USART_PRINT 0
#define USART_PRINT_DMA 0

#define USART_DMA

#define USART_DMA_RX 0

#define USART_RX0
// #define USART_RX1
// #define USART_RX2
// #define UART_RX3
// #define UART_RX4
// #define USART_RX5
// #define UART_RX6
// #define UART_RX7

#ifdef USART_DMA
  #define RX_DMA_BUFFER_SIZE RX0BUFFER_SIZE
  #define RX_DMA_BUFFER rx0buffer
#endif

#ifdef USART_RX0
  #define RX0BUFFER_SIZE 64
  extern uint8_t rx0buffer[RX0BUFFER_SIZE+1];
  extern void RX0_recv(uint8_t *rxbuffer,uint32_t len);
#endif
#ifdef USART_RX1
  #define RX1BUFFER_SIZE 64
  extern uint8_t rx1buffer[RX1BUFFER_SIZE+1];
  extern void RX1_recv(uint8_t *rxbuffer,uint32_t len);
#endif
#ifdef USART_RX2
  #define RX2BUFFER_SIZE 64
  extern uint8_t rx2buffer[RX2BUFFER_SIZE+1];
  extern void RX2_recv(uint8_t *rxbuffer,uint32_t len);
#endif
#ifdef UART_RX3
  #define RX3BUFFER_SIZE 64
  extern uint8_t rx3buffer[RX3BUFFER_SIZE+1];
  extern void RX3_recv(uint8_t *rxbuffer,uint32_t len);
#endif
#ifdef UART_RX4
  #define RX4BUFFER_SIZE 64
  extern uint8_t rx4buffer[RX4BUFFER_SIZE+1];
  extern void RX4_recv(uint8_t *rxbuffer,uint32_t len);
#endif
#ifdef USART_RX5
  #define RX5BUFFER_SIZE 64
  extern uint8_t rx5buffer[RX5BUFFER_SIZE+1];
  extern void RX5_recv(uint8_t *rxbuffer,uint32_t len);
#endif
#ifdef UART_RX6
  #define RX6BUFFER_SIZE 64
  extern uint8_t rx6buffer[RX6BUFFER_SIZE+1];
  extern void RX6_recv(uint8_t *rxbuffer,uint32_t len);
#endif
#ifdef UART_RX7
  #define RX7BUFFER_SIZE 64
  extern uint8_t rx7buffer[RX7BUFFER_SIZE+1];
  extern void RX7_recv(uint8_t *rxbuffer,uint32_t len);
#endif

void usart_init(uint32_t usart_periph,uint32_t gpio_periph, uint32_t alt_func_num, uint32_t pin);
void send_byte(uint32_t usart_periph,uint8_t byte);
void send_string(uint32_t usart_periph,char *str);
void send_array(uint32_t usart_periph,uint8_t *data,uint32_t length);

#ifdef USART_DMA
  void usart_dma_tx_init(uint32_t usart_periph,uint32_t dma_periph,dma_channel_enum channelx);
  void usart_dma_send(uint32_t usart_periph,uint8_t * data,uint8_t len);
  #if defined(USART_DMA_RX) && (defined(USART_RX0) || defined(USART_RX1) || defined(USART_RX2) || defined(UART_RX3) || defined(UART_RX4) || defined(USART_RX5) || defined(UART_RX6) || defined(UART_RX7))
    void usart_dma_rx_init(uint32_t usart_periph,uint32_t dma_periph,dma_channel_enum channelx);
  #endif
#endif

#endif