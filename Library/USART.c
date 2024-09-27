#include "USART.h"
#include "stdio.h"
#include "GPIO.h"

static void usart_config(uint32_t usart_periph,uint32_t baudval,uint8_t nvic_irq_pre_priority, uint8_t nvic_irq_sub_priority){
    if(usart_periph == USART1)  rcu_periph_clock_enable(RCU_USART1);
    else if(usart_periph == USART2) rcu_periph_clock_enable(RCU_USART2);
    else if(usart_periph == UART3) rcu_periph_clock_enable(RCU_UART3);
    else if(usart_periph == UART4) rcu_periph_clock_enable(RCU_UART4);
    else if(usart_periph == UART6) rcu_periph_clock_enable(RCU_UART6);
    else if(usart_periph == UART7) rcu_periph_clock_enable(RCU_UART7);
    else if(usart_periph == USART0) rcu_periph_clock_enable(RCU_USART0);
    else if(usart_periph == USART5) rcu_periph_clock_enable(RCU_USART5);
    
    usart_deinit(usart_periph);
    usart_baudrate_set(usart_periph, baudval);
    usart_receive_config(usart_periph, USART_RECEIVE_ENABLE);
    usart_transmit_config(usart_periph, USART_TRANSMIT_ENABLE);
    usart_enable(usart_periph);

    usart_interrupt_enable(usart_periph,USART_INT_RBNE);
    usart_interrupt_enable(usart_periph,USART_INT_IDLE);

    if(usart_periph == USART1) nvic_irq_enable(USART1_IRQn,nvic_irq_pre_priority,nvic_irq_sub_priority);
    else if(usart_periph == USART2) nvic_irq_enable(USART2_IRQn,nvic_irq_pre_priority,nvic_irq_sub_priority);
    else if(usart_periph == UART3) nvic_irq_enable(UART3_IRQn,nvic_irq_pre_priority,nvic_irq_sub_priority);
    else if(usart_periph == UART4) nvic_irq_enable(UART4_IRQn,nvic_irq_pre_priority,nvic_irq_sub_priority);
#if defined (GD32F450) || defined (GD32F470)
    else if(usart_periph == UART6) nvic_irq_enable(UART6_IRQn,nvic_irq_pre_priority,nvic_irq_sub_priority);
    else if(usart_periph == UART7) nvic_irq_enable(UART7_IRQn,nvic_irq_pre_priority,nvic_irq_sub_priority);
#endif
    else if(usart_periph == USART0) nvic_irq_enable(USART0_IRQn,nvic_irq_pre_priority,nvic_irq_sub_priority);
    else if(usart_periph == USART5) nvic_irq_enable(USART5_IRQn,nvic_irq_pre_priority,nvic_irq_sub_priority);
}

void usart_init(uint32_t usart_periph,uint32_t gpio_periph, uint32_t alt_func_num, uint32_t pin){
    GPIO_af_init(gpio_periph,alt_func_num,GPIO_PUPD_NONE,pin);
    usart_config(usart_periph, 115200, 0, 0);
}

void send_byte(uint32_t usart_periph,uint8_t byte){
    usart_data_transmit(usart_periph, byte);
    while(RESET == usart_flag_get(usart_periph,USART_FLAG_TBE));
}

void send_string(uint32_t usart_periph,char *str){
    while (str&&*str) send_byte(usart_periph,*str++);
}

void send_array(uint32_t usart_periph,uint8_t *data,uint32_t length){
    while(length--) send_byte(usart_periph,*data++);
}

#ifdef USART_PRINT
int fputc(int ch,FILE *f){
    #if USART_PRINT == 0
    send_byte(USART0,ch);
    #endif
    #if USART_PRINT == 1
    send_byte(USART1,ch);
    #endif
    #if USART_PRINT == 2
    send_byte(USART2,ch);
    #endif
    #if USART_PRINT == 3
    send_byte(UART3,ch);
    #endif
    #if USART_PRINT == 4
    send_byte(UART4,ch);
    #endif
    #if USART_PRINT == 5
    send_byte(USART5,ch);
    #endif
    #if USART_PRINT == 6
    send_byte(UART6,ch);
    #endif
    #if USART_PRINT == 7
    send_byte(UART7,ch);
    #endif
    return ch;
}
#endif

uint8_t rx0buffer[RX0BUFFER_SIZE+1];
uint8_t rx1buffer[RX1BUFFER_SIZE+1];
uint8_t rx2buffer[RX2BUFFER_SIZE+1];
uint8_t rx3buffer[RX3BUFFER_SIZE+1];
uint8_t rx4buffer[RX4BUFFER_SIZE+1];
uint8_t rx5buffer[RX5BUFFER_SIZE+1];
uint8_t rx6buffer[RX6BUFFER_SIZE+1];
uint8_t rx7buffer[RX7BUFFER_SIZE+1];

uint8_t len0 = 0;
uint8_t len1 = 0;
uint8_t len2 = 0;
uint8_t len3 = 0;
uint8_t len4 = 0;
uint8_t len5 = 0;
uint8_t len6 = 0;
uint8_t len7 = 0;

#ifdef USART_RX0
void USART0_IRQHandler(void){
    if(SET == usart_interrupt_flag_get(USART0,USART_INT_FLAG_RBNE)){
        len0 %= RX0BUFFER_SIZE;
        rx0buffer[len0++] = usart_data_receive(USART0);
        usart_interrupt_flag_clear(USART0,USART_INT_FLAG_RBNE);
    }

    if(SET == usart_interrupt_flag_get(USART0,USART_INT_FLAG_IDLE)){
        usart_data_receive(USART0);
        rx0buffer[len0++] = '\0';
        len0 = 0;
        RX0_recv(rx0buffer,len0);
    }
}
#endif
#ifdef USART_RX1
void USART1_IRQHandler(void){
    if(SET == usart_interrupt_flag_get(USART1,USART_INT_FLAG_RBNE)){
        len1 %= RX1BUFFER_SIZE;
        rx1buffer[len1++] = usart_data_receive(USART1);
        usart_interrupt_flag_clear(USART1,USART_INT_FLAG_RBNE);
    }

    if(SET == usart_interrupt_flag_get(USART1,USART_INT_FLAG_IDLE)){
        usart_data_receive(USART1);
        rx1buffer[len1++] = '\0';
        len1 = 0;
        RX1_recv(rx1buffer,len1);
    }
}
#endif
#ifdef USART_RX2
void USART2_IRQHandler(void){
    if(SET == usart_interrupt_flag_get(USART2,USART_INT_FLAG_RBNE)){
        len2 %= RX2BUFFER_SIZE;
        rx2buffer[len2++] = usart_data_receive(USART2);
        usart_interrupt_flag_clear(USART2,USART_INT_FLAG_RBNE);
    }

    if(SET == usart_interrupt_flag_get(USART2,USART_INT_FLAG_IDLE)){
        usart_data_receive(USART2);
        rx2buffer[len2++] = '\0';
        len2 = 0;
        RX2_recv(rx2buffer,len2);
    }
}
#endif
#ifdef UART_RX3
void UART3_IRQHandler(void){
    if(SET == usart_interrupt_flag_get(UART3,USART_INT_FLAG_RBNE)){
        len3 %= RX3BUFFER_SIZE;
        rx3buffer[len3++] = usart_data_receive(UART3);
        usart_interrupt_flag_clear(UART3,USART_INT_FLAG_RBNE);
    }

    if(SET == usart_interrupt_flag_get(UART3,USART_INT_FLAG_IDLE)){
        usart_data_receive(UART3);
        rx3buffer[len3++] = '\0';
        len3 = 0;
        RX3_recv(rx3buffer,len3);
    }
}
#endif
#ifdef UART_RX4
void UART4_IRQHandler(void){
    if(SET == usart_interrupt_flag_get(UART4,USART_INT_FLAG_RBNE)){
        len4 %= RX4BUFFER_SIZE;
        rx4buffer[len4++] = usart_data_receive(UART4);
        usart_interrupt_flag_clear(UART4,USART_INT_FLAG_RBNE);
    }

    if(SET == usart_interrupt_flag_get(UART4,USART_INT_FLAG_IDLE)){
        usart_data_receive(UART4);
        rx4buffer[len4++] = '\0';
        len4 = 0;
        RX4_recv(rx4buffer,len4);
    }
}
#endif
#ifdef USART_RX5
void USART5_IRQHandler(void){
    if(SET == usart_interrupt_flag_get(USART5,USART_INT_FLAG_RBNE)){
        len5 %= RX5BUFFER_SIZE;
        rx5buffer[len5++] = usart_data_receive(USART5);
        usart_interrupt_flag_clear(USART5,USART_INT_FLAG_RBNE);
    }

    if(SET == usart_interrupt_flag_get(USART5,USART_INT_FLAG_IDLE)){
        usart_data_receive(USART5);
        rx5buffer[len5++] = '\0';
        len5 = 0;
        RX5_recv(rx5buffer,len5);
    }
}
#endif