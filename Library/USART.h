#ifndef __USART_H__
#define __USART_H__

#include "gd32f4xx.h"

#define USART_PRINT 0
#define USART_RX0
// #define USART_RX1
// #define USART_RX2
// #define UART_RX3
// #define UART_RX4
// #define USART_RX5
// #define UART_RX6
// #define UART_RX7

#define RX0BUFFER_SIZE 64
#define RX1BUFFER_SIZE 64
#define RX2BUFFER_SIZE 64
#define RX3BUFFER_SIZE 64
#define RX4BUFFER_SIZE 64
#define RX5BUFFER_SIZE 64
#define RX6BUFFER_SIZE 64
#define RX7BUFFER_SIZE 64


#ifdef USART_RX0
extern void RX0_recv(uint8_t *rxbuffer,uint32_t len);
#endif
#ifdef USART_RX1
extern void RX1_recv(uint8_t *rxbuffer,uint32_t len);
#endif
#ifdef USART_RX2
extern void RX2_recv(uint8_t *rxbuffer,uint32_t len);
#endif
#ifdef UART_RX3
extern void RX3_recv(uint8_t *rxbuffer,uint32_t len);
#endif
#ifdef UART_RX4
extern void RX4_recv(uint8_t *rxbuffer,uint32_t len);
#endif
#ifdef USART_RX5
extern void RX5_recv(uint8_t *rxbuffer,uint32_t len);
#endif
#ifdef UART_RX6
extern void RX6_recv(uint8_t *rxbuffer,uint32_t len);
#endif
#ifdef UART_RX7
extern void RX7_recv(uint8_t *rxbuffer,uint32_t len);
#endif

void usart_init(uint32_t usart_periph,uint32_t gpio_periph, uint32_t alt_func_num, uint32_t pin);
void send_byte(uint32_t usart_periph,uint8_t byte);
void send_string(uint32_t usart_periph,char *str);
void send_array(uint32_t usart_periph,uint8_t *data,uint32_t length);

#endif