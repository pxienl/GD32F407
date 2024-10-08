#include "USART.h"
#include "stdio.h"
#include "GPIO.h"
#include "stdbool.h"
#include "DMA.h"

bool busy = false;
uint8_t buffer = 0;

static void usart_config(uint32_t usart_periph,uint32_t baudval,uint8_t nvic_irq_pre_priority, uint8_t nvic_irq_sub_priority){
         if( usart_periph == USART1) rcu_periph_clock_enable(RCU_USART1);
    else if( usart_periph == USART2) rcu_periph_clock_enable(RCU_USART2);
    else if( usart_periph == UART3 ) rcu_periph_clock_enable(RCU_UART3);
    else if( usart_periph == UART4 ) rcu_periph_clock_enable(RCU_UART4);
    else if( usart_periph == UART6 ) rcu_periph_clock_enable(RCU_UART6);
    else if( usart_periph == UART7 ) rcu_periph_clock_enable(RCU_UART7);
    else if( usart_periph == USART0) rcu_periph_clock_enable(RCU_USART0);
    else if( usart_periph == USART5) rcu_periph_clock_enable(RCU_USART5);
    
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

/*
\brief      USART init 初始化
    \param[in]  usart_periph: USARTx(x=0,1,2,5)/UARTx(x=3,4,6,7)   usart的序列号

    \param[in]  gpio_periph: GPIO port                            GPIO 端口
                only one parameter can be selected which is shown as below:
      \arg        GPIOx(x = A,B,C,D,E,F,G,H,I)
    \param[in]  alt_func_num: GPIO pin af function 引脚对应的复用的功能，以及对应的AF位很重要
      \arg        GPIO_AF_0: SYSTEM
      \arg        GPIO_AF_1: TIMER0, TIMER1
      \arg        GPIO_AF_2: TIMER2, TIMER3, TIMER4
      \arg        GPIO_AF_3: TIMER7, TIMER8, TIMER9, TIMER10
      \arg        GPIO_AF_4: I2C0, I2C1, I2C2
      \arg        GPIO_AF_5: SPI0, SPI1, SPI2, SPI3, SPI4, SPI5
      \arg        GPIO_AF_6: SPI2, SPI3, SPI4
      \arg        GPIO_AF_7: USART0, USART1, USART2, SPI1, SPI2
      \arg        GPIO_AF_8: UART3, UART4, USART5, UART6, UART7
      \arg        GPIO_AF_9: CAN0, CAN1, TLI, TIMER11, TIMER12, TIMER13, I2C1, I2C2, CTC
      \arg        GPIO_AF_10: USB_FS, USB_HS
      \arg        GPIO_AF_11: ENET
      \arg        GPIO_AF_12: EXMC, SDIO, USB_HS
      \arg        GPIO_AF_13: DCI
      \arg        GPIO_AF_14: TLI
      \arg        GPIO_AF_15: EVENTOUT
    \param[in]  pin: GPIO pin            GPIO的引脚
                one or more parameters can be selected which are shown as below:
      \arg        GPIO_PIN_x(x=0..15), GPIO_PIN_ALL
    \retval     none
*/
void usart_init(uint32_t usart_periph,uint32_t gpio_periph, uint32_t alt_func_num, uint32_t pin){
    GPIO_af_init(gpio_periph,alt_func_num,GPIO_PUPD_NONE,pin);
    usart_config(usart_periph, 115200, 0, 1);
}

/*
\brief      USART 发送一个字节的函数
    \param[in]  usart_periph: USARTx(x=0,1,2,5)/UARTx(x=3,4,6,7)   usart的序列号
		\param[in]  byte: 需要传输的一个字节
    \retval     none
*/
void send_byte(uint32_t usart_periph,uint8_t byte){
    if(busy){
        usart_data_transmit(usart_periph, buffer);
        while(RESET == usart_flag_get(usart_periph,USART_FLAG_TBE));
    }
    buffer = byte;
    busy = true;
    usart_data_transmit(usart_periph, byte);
    while(RESET == usart_flag_get(usart_periph,USART_FLAG_TBE));
    busy = false;
    
}

/*
\brief      USART 发送一个字节的函数
    \param[in]  usart_periph: USARTx(x=0,1,2,5)/UARTx(x=3,4,6,7)   usart的序列号
		\param[in]  *str: 需要传输的一整个字符串 (指针) 根据字符串的特性，必须在后面加上一个‘\0’ 检索到就关闭发送
		\retval     none
*/
void send_string(uint32_t usart_periph,char *str){
    while (str&&*str) send_byte(usart_periph,*str++);
}

/*
\brief      USART 发送一个字节的函数
    \param[in]  usart_periph: USARTx(x=0,1,2,5)/UARTx(x=3,4,6,7)   usart的序列号
		\param[in]  *data: 	需要传输的字节数组
		\param[in]  length: 字节数组的长度
    \retval     none
*/
void send_array(uint32_t usart_periph,uint8_t *data,uint32_t length){
    while(length--) send_byte(usart_periph,*data++);
}

#ifdef USART_DMA
uint32_t USART0_dma_periph = NULL;
uint32_t USART0_tx_dma_channel = NULL;
uint32_t USART0_rx_dma_channel = NULL;

uint32_t USART1_dma_periph = NULL;
uint32_t USART1_tx_dma_channel = NULL;
uint32_t USART1_rx_dma_channel = NULL;

uint32_t USART2_dma_periph = NULL;
uint32_t USART2_tx_dma_channel = NULL;
uint32_t USART2_rx_dma_channel = NULL;

uint32_t UART3_dma_periph = NULL;
uint32_t UART3_tx_dma_channel = NULL;
uint32_t UART3_rx_dma_channel = NULL;

uint32_t UART4_dma_periph = NULL;
uint32_t UART4_tx_dma_channel = NULL;
uint32_t UART4_rx_dma_channel = NULL;

uint32_t USART5_dma_periph = NULL;
uint32_t USART5_tx_dma_channel = NULL;
uint32_t USART5_rx_dma_channel = NULL;

uint32_t UART6_dma_periph = NULL;
uint32_t UART6_tx_dma_channel = NULL;
uint32_t UART6_rx_dma_channel = NULL;

uint32_t UART7_dma_periph = NULL;
uint32_t UART7_tx_dma_channel = NULL;
uint32_t UART7_rx_dma_channel = NULL;

static void usart_dma_tx_set(uint32_t usart_periph,uint32_t dma_periph,dma_channel_enum channelx){
         if( usart_periph == USART0) USART0_dma_periph = dma_periph, USART0_tx_dma_channel = channelx;
    else if( usart_periph == USART1) USART1_dma_periph = dma_periph, USART1_tx_dma_channel = channelx;
    else if( usart_periph == USART2) USART2_dma_periph = dma_periph, USART2_tx_dma_channel = channelx;
    else if( usart_periph == UART3 ) UART3_dma_periph  = dma_periph, UART3_tx_dma_channel  = channelx;
    else if( usart_periph == UART4 ) UART4_dma_periph  = dma_periph, UART4_tx_dma_channel  = channelx;
    else if( usart_periph == USART5) USART5_dma_periph = dma_periph, USART5_tx_dma_channel = channelx;
    else if( usart_periph == UART6 ) UART6_dma_periph  = dma_periph, UART6_tx_dma_channel  = channelx;
    else if( usart_periph == UART7 ) UART7_dma_periph  = dma_periph, UART7_tx_dma_channel  = channelx;
}
static void usart_dma_rx_set(uint32_t usart_periph,uint32_t dma_periph,dma_channel_enum channelx){
         if( usart_periph == USART0) USART0_dma_periph = dma_periph, USART0_rx_dma_channel = channelx;
    else if( usart_periph == USART1) USART1_dma_periph = dma_periph, USART1_rx_dma_channel = channelx;
    else if( usart_periph == USART2) USART2_dma_periph = dma_periph, USART2_rx_dma_channel = channelx;
    else if( usart_periph == UART3 ) UART3_dma_periph  = dma_periph, UART3_rx_dma_channel  = channelx;
    else if( usart_periph == UART4 ) UART4_dma_periph  = dma_periph, UART4_rx_dma_channel  = channelx;
    else if( usart_periph == USART5) USART5_dma_periph = dma_periph, USART5_rx_dma_channel = channelx;
    else if( usart_periph == UART6 ) UART6_dma_periph  = dma_periph, UART6_rx_dma_channel  = channelx;
    else if( usart_periph == UART7 ) UART7_dma_periph  = dma_periph, UART7_rx_dma_channel  = channelx;
}

static void usart_dma_tx_select(uint32_t usart_periph,uint32_t* dma_periph,dma_channel_enum* channelx){
         if( usart_periph == USART0) *dma_periph = USART0_dma_periph, *channelx = USART0_tx_dma_channel;
    else if( usart_periph == USART1) *dma_periph = USART1_dma_periph, *channelx = USART1_tx_dma_channel;
    else if( usart_periph == USART2) *dma_periph = USART2_dma_periph, *channelx = USART2_tx_dma_channel;
    else if( usart_periph == UART3 ) *dma_periph = UART3_dma_periph , *channelx = UART3_tx_dma_channel;
    else if( usart_periph == UART4 ) *dma_periph = UART4_dma_periph , *channelx = UART4_tx_dma_channel;
    else if( usart_periph == USART5) *dma_periph = USART5_dma_periph, *channelx = USART5_tx_dma_channel;
    else if( usart_periph == UART6 ) *dma_periph = UART6_dma_periph , *channelx = UART6_tx_dma_channel;
    else if( usart_periph == UART7 ) *dma_periph = UART7_dma_periph , *channelx = UART7_tx_dma_channel;
}
static void usart_dma_rx_select(uint32_t usart_periph,uint32_t* dma_periph,dma_channel_enum* channelx){
         if( usart_periph == USART0) *dma_periph = USART0_dma_periph, *channelx = USART0_rx_dma_channel;
    else if( usart_periph == USART1) *dma_periph = USART1_dma_periph, *channelx = USART1_rx_dma_channel;
    else if( usart_periph == USART2) *dma_periph = USART2_dma_periph, *channelx = USART2_rx_dma_channel;
    else if( usart_periph == UART3 ) *dma_periph = UART3_dma_periph , *channelx = UART3_rx_dma_channel;
    else if( usart_periph == UART4 ) *dma_periph = UART4_dma_periph , *channelx = UART4_rx_dma_channel;
    else if( usart_periph == USART5) *dma_periph = USART5_dma_periph, *channelx = USART5_rx_dma_channel;
    else if( usart_periph == UART6 ) *dma_periph = UART6_dma_periph , *channelx = UART6_rx_dma_channel;
    else if( usart_periph == UART7 ) *dma_periph = UART7_dma_periph , *channelx = UART7_rx_dma_channel;
}

/**
    UART4_RX  DMA0 DMA_CH0 PERIEN4
    USART2_RX DMA0 DMA_CH1 PERIEN4
    UART3_RX  DMA0 DMA_CH2 PERIEN4
    USART2_TX DMA0 DMA_CH3 PERIEN4
    UART3_TX  DMA0 DMA_CH4 PERIEN4
    USART1_RX DMA0 DMA_CH5 PERIEN4
    USART1_TX DMA0 DMA_CH6 PERIEN4
    UART4_TX  DMA0 DMA_CH7 PERIEN4
    UART7_TX  DMA0 DMA_CH0 PERIEN5
    UART6_TX  DMA0 DMA_CH1 PERIEN5
    UART6_RX  DMA0 DMA_CH3 PERIEN5
    UART7_RX  DMA0 DMA_CH6 PERIEN5
    USART2_TX DMA0 DMA_CH4 PERIEN7
    USART0_RX DMA1 DMA_CH2 PERIEN4
    USART0_RX DMA1 DMA_CH5 PERIEN4
    USART0_TX DMA1 DMA_CH7 PERIEN4
    USART5_RX DMA1 DMA_CH1 PERIEN5
    USART5_RX DMA1 DMA_CH2 PERIEN5
    USART5_TX DMA1 DMA_CH6 PERIEN5
    USART5_TX DMA1 DMA_CH7 PERIEN5 
*/
static void uart_dma_perien_select(uint32_t usart_periph,dma_channel_enum channelx){
         if (usart_periph == USART0  &&  channelx == DMA_CH2) dma_channel_subperipheral_select(DMA1,channelx,DMA_SUBPERI4); // USART0_RX
    else if (usart_periph == USART0  &&  channelx == DMA_CH5) dma_channel_subperipheral_select(DMA1,channelx,DMA_SUBPERI4); // USART0_RX
    else if (usart_periph == USART0  &&  channelx == DMA_CH7) dma_channel_subperipheral_select(DMA1,channelx,DMA_SUBPERI4); // USART0_TX
    else if (usart_periph == USART1  &&  channelx == DMA_CH5) dma_channel_subperipheral_select(DMA0,channelx,DMA_SUBPERI4); // USART1_RX
    else if (usart_periph == USART1  &&  channelx == DMA_CH6) dma_channel_subperipheral_select(DMA0,channelx,DMA_SUBPERI4); // USART1_TX
    else if (usart_periph == USART2  &&  channelx == DMA_CH1) dma_channel_subperipheral_select(DMA0,channelx,DMA_SUBPERI4); // USART2_RX
    else if (usart_periph == USART2  &&  channelx == DMA_CH3) dma_channel_subperipheral_select(DMA0,channelx,DMA_SUBPERI4); // USART2_TX
    else if (usart_periph == USART2  &&  channelx == DMA_CH4) dma_channel_subperipheral_select(DMA0,channelx,DMA_SUBPERI7); // USART2_TX
    else if (usart_periph == UART3   &&  channelx == DMA_CH2) dma_channel_subperipheral_select(DMA0,channelx,DMA_SUBPERI4); // UART3_RX
    else if (usart_periph == UART3   &&  channelx == DMA_CH4) dma_channel_subperipheral_select(DMA0,channelx,DMA_SUBPERI4); // UART3_TX
    else if (usart_periph == UART4   &&  channelx == DMA_CH0) dma_channel_subperipheral_select(DMA0,channelx,DMA_SUBPERI4); // UART4_RX
    else if (usart_periph == UART4   &&  channelx == DMA_CH7) dma_channel_subperipheral_select(DMA0,channelx,DMA_SUBPERI4); // UART4_TX
    else if (usart_periph == USART5  &&  channelx == DMA_CH1) dma_channel_subperipheral_select(DMA1,channelx,DMA_SUBPERI5); // USART5_RX
    else if (usart_periph == USART5  &&  channelx == DMA_CH2) dma_channel_subperipheral_select(DMA1,channelx,DMA_SUBPERI5); // USART5_RX
    else if (usart_periph == USART5  &&  channelx == DMA_CH6) dma_channel_subperipheral_select(DMA1,channelx,DMA_SUBPERI5); // USART5_TX
    else if (usart_periph == USART5  &&  channelx == DMA_CH7) dma_channel_subperipheral_select(DMA1,channelx,DMA_SUBPERI5); // USART5_TX
    else if (usart_periph == UART6   &&  channelx == DMA_CH3) dma_channel_subperipheral_select(DMA0,channelx,DMA_SUBPERI5); // UART6_RX
    else if (usart_periph == UART6   &&  channelx == DMA_CH1) dma_channel_subperipheral_select(DMA0,channelx,DMA_SUBPERI5); // UART6_TX
    else if (usart_periph == UART7   &&  channelx == DMA_CH6) dma_channel_subperipheral_select(DMA0,channelx,DMA_SUBPERI5); // UART7_RX
    else if (usart_periph == UART7   &&  channelx == DMA_CH0) dma_channel_subperipheral_select(DMA0,channelx,DMA_SUBPERI5); // UART7_TX
}
/**
 * @brief 此函数通过开启DMA ， 让DMA实现USART发送的功能
 * 
 * @param usart_periph   usart_periph: USARTx(x=0,1,2,5)/UARTx(x=3,4,6,7)   usart的序列号
 * @param dma_periph  dma_periph  :  DMAx(x=0,1)
 * @param channelx  channelx  :  DMA_CHx(x=0..7)
 */
void usart_dma_tx_init(uint32_t usart_periph,uint32_t dma_periph,dma_channel_enum channelx){
    DMA_select(dma_periph);
    dma_deinit(dma_periph,channelx);
    dma_single_data_parameter_struct dma_param;
    dma_single_data_para_struct_init(&dma_param);
    dma_param.direction =  DMA_MEMORY_TO_PERIPH;
    dma_param.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_param.periph_addr = (uint32_t)&USART_DATA(usart_periph);
    dma_param.periph_memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_single_data_mode_init(dma_periph,channelx,&dma_param);
    uart_dma_perien_select(usart_periph,channelx);
    usart_dma_tx_set(usart_periph,dma_periph,channelx);
}

#ifdef USART_DMA_RX

/**
 * @brief 此函数通过开启DMA ， 让DMA实现USART接收的功能
 * 
 * @param usart_periph  usart_periph  :  USARTx(x=0,1,2,5)/UARTx(x=3,4,6,7)   usart的序列号
 * @param dma_periph  dma_periph  :  DMAx(x=0,1)
 * @param channelx  channelx  :  DMA_CHx(x=0..7)
 */
void usart_dma_rx_init(uint32_t usart_periph,uint32_t dma_periph,dma_channel_enum channelx){
    DMA_select(dma_periph);
    dma_deinit(dma_periph,channelx);
    dma_single_data_parameter_struct dma_param;
    dma_single_data_para_struct_init(&dma_param);
    dma_param.direction =  DMA_PERIPH_TO_MEMORY;
    dma_param.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_param.memory0_addr = (uint32_t)RX_DMA_BUFFER;
    dma_param.number = RX_DMA_BUFFER_SIZE;
    dma_param.periph_addr = (uint32_t)&USART_DATA(usart_periph);
    dma_param.periph_memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_param.priority = DMA_PRIORITY_MEDIUM;
    dma_single_data_mode_init(dma_periph,channelx,&dma_param);
    uart_dma_perien_select(usart_periph,channelx);
    usart_dma_rx_set(usart_periph,dma_periph,channelx);

    usart_dma_receive_config(usart_periph, USART_RECEIVE_DMA_ENABLE);

    dma_flag_clear(dma_periph,channelx,DMA_FLAG_FTF);
    dma_channel_enable(dma_periph,channelx);
}

#endif

/**
 * @brief 此函数通过DMA发送数据
 * 
 * @param usart_periph usart_periph  :  USARTx(x=0,1,2,5)/UARTx(x=3,4,6,7)   usart的序列号
 * @param data  指针，指向 目的地址 
 * @param len   数据长度 [1,255]
 */
void usart_dma_send(uint32_t usart_periph,uint8_t * data,uint8_t len){
    uint32_t dma_periph = NULL;
    dma_channel_enum channelx = NULL;
    usart_dma_tx_select(usart_periph,&dma_periph,&channelx);

    dma_memory_address_config(dma_periph,channelx,DMA_MEMORY_0,(uint32_t)data);
    dma_transfer_number_config(dma_periph,channelx,len);
    usart_dma_transmit_config(usart_periph, USART_TRANSMIT_DMA_ENABLE);
    dma_channel_enable(dma_periph,channelx);
    while(RESET == dma_flag_get(dma_periph, channelx, DMA_FLAG_FTF));
    dma_flag_clear(dma_periph, channelx, DMA_FLAG_FTF);
}
#endif

#ifdef USART_PRINT
int fputc(int ch,FILE *f){
    #if USART_PRINT == 0
        #if defined(USART_PRINT_DMA) && USART_PRINT_DMA == USART_PRINT
            usart_dma_send(USART0,(uint8_t*)&ch,1);
        #else
            send_byte(USART0,ch);
        #endif
    #endif
    #if USART_PRINT == 1
        #if defined(USART_PRINT_DMA) && USART_PRINT_DMA == USART_PRINT
            usart_dma_send(USART1,(uint8_t*)&ch,1);
        #else
            send_byte(USART1,ch);
        #endif
    #endif
    #if USART_PRINT == 2
        #if defined(USART_PRINT_DMA) && USART_PRINT_DMA == USART_PRINT
            usart_dma_send(USART2,(uint8_t*)&ch,1);
        #else
            send_byte(USART2,ch);
        #endif
    #endif
    #if USART_PRINT == 3
        #if defined(USART_PRINT_DMA) && USART_PRINT_DMA == USART_PRINT
            usart_dma_send(UART3,(uint8_t*)&ch,1);
        #else
            send_byte(UART3,ch);
        #endif
    #endif
    #if USART_PRINT == 4
        #if defined(USART_PRINT_DMA) && USART_PRINT_DMA == USART_PRINT
            usart_dma_send(UART4,(uint8_t*)&ch,1);
        #else
            send_byte(UART4,ch);
        #endif
    #endif
    #if USART_PRINT == 5
        #if defined(USART_PRINT_DMA) && USART_PRINT_DMA == USART_PRINT
            usart_dma_send(USART5,(uint8_t*)&ch,1);
        #else
            send_byte(USART5,ch);
        #endif
    #endif
    #if USART_PRINT == 6
        #if defined(USART_PRINT_DMA) && USART_PRINT_DMA == USART_PRINT
            usart_dma_send(UART6,(uint8_t*)&ch,1);
        #else
            send_byte(UART6,ch);
        #endif
    #endif
    #if USART_PRINT == 7
        #if defined(USART_PRINT_DMA) && USART_PRINT_DMA == USART_PRINT
            usart_dma_send(UART7,(uint8_t*)&ch,1);
        #else
            send_byte(UART7,ch);
        #endif
    #endif
    return ch;
}
#endif


#ifdef USART_RX0
uint8_t len0 = 0;
uint8_t rx0buffer[RX0BUFFER_SIZE+1];

void USART0_IRQHandler(void){
    if(SET == usart_interrupt_flag_get(USART0,USART_INT_FLAG_RBNE)){
        len0 %= RX0BUFFER_SIZE;
        rx0buffer[len0++] = usart_data_receive(USART0);
        usart_interrupt_flag_clear(USART0,USART_INT_FLAG_RBNE);
    }

    if(SET == usart_interrupt_flag_get(USART0,USART_INT_FLAG_IDLE)){
        usart_data_receive(USART0);
        #if defined(USART_DMA_RX) && USART_DMA_RX == 0
            dma_channel_disable(USART0_dma_periph,USART0_rx_dma_channel);
            len0 = RX_DMA_BUFFER_SIZE - dma_transfer_number_get(USART0_dma_periph,USART0_rx_dma_channel);
            if(len0){
                rx0buffer[len0] = '\0';
                RX0_recv(rx0buffer,len0);
            }
            len0 = 0;
            dma_flag_clear(USART0_dma_periph,USART0_rx_dma_channel,DMA_FLAG_FTF);
            dma_channel_enable(USART0_dma_periph,USART0_rx_dma_channel);
        #else
            rx0buffer[len0++] = '\0';
            RX0_recv(rx0buffer,len0);
            len0 = 0;
        #endif
    }
}
#endif
#ifdef USART_RX1
uint8_t len1 = 0;
uint8_t rx1buffer[RX1BUFFER_SIZE+1];
void USART1_IRQHandler(void){
    if(SET == usart_interrupt_flag_get(USART1,USART_INT_FLAG_RBNE)){
        len1 %= RX1BUFFER_SIZE;
        rx1buffer[len1++] = usart_data_receive(USART1);
        usart_interrupt_flag_clear(USART1,USART_INT_FLAG_RBNE);
    }

    if(SET == usart_interrupt_flag_get(USART1,USART_INT_FLAG_IDLE)){
        usart_data_receive(USART1);
        #if defined(USART_DMA_RX) && USART_DMA_RX == 1
            dma_channel_disable(USART1_dma_periph,USART1_rx_dma_channel);
            len1 = RX_DMA_BUFFER_SIZE - dma_transfer_number_get(USART1_dma_periph,USART1_rx_dma_channel);
            if(len1){
                rx1buffer[len1] = '\0';
                RX1_recv(rx1buffer,len1);
            }
            len1 = 0;
            dma_flag_clear(USART1_dma_periph,USART1_rx_dma_channel,DMA_FLAG_FTF);
            dma_channel_enable(USART1_dma_periph,USART1_rx_dma_channel);
        #else
            rx1buffer[len1++] = '\0';
            RX1_recv(rx1buffer,len1);
            len1 = 0;
        #endif
    }
}
#endif
#ifdef USART_RX2
uint8_t len2 = 0;
uint8_t rx2buffer[RX2BUFFER_SIZE+1];
void USART2_IRQHandler(void){
    if(SET == usart_interrupt_flag_get(USART2,USART_INT_FLAG_RBNE)){
        len2 %= RX2BUFFER_SIZE;
        rx2buffer[len2++] = usart_data_receive(USART2);
        usart_interrupt_flag_clear(USART2,USART_INT_FLAG_RBNE);
    }

    if(SET == usart_interrupt_flag_get(USART2,USART_INT_FLAG_IDLE)){
        usart_data_receive(USART2);
        #if defined(USART_DMA_RX) && USART_DMA_RX == 2
            dma_channel_disable(USART2_dma_periph,USART2_rx_dma_channel);
            len2 = RX_DMA_BUFFER_SIZE - dma_transfer_number_get(USART2_dma_periph,USART2_rx_dma_channel);
            if(len2){
                rx2buffer[len2] = '\0';
                RX2_recv(rx2buffer,len2);
            }
            len2 = 0;
            dma_flag_clear(USART2_dma_periph,USART2_rx_dma_channel,DMA_FLAG_FTF);
            dma_channel_enable(USART2_dma_periph,USART2_rx_dma_channel);
        #else
            rx2buffer[len2++] = '\0';
            RX2_recv(rx2buffer,len2);
            len2 = 0;
        #endif
    }
}
#endif
#ifdef UART_RX3
uint8_t len3 = 0;
uint8_t rx3buffer[RX3BUFFER_SIZE+1];
void UART3_IRQHandler(void){
    if(SET == usart_interrupt_flag_get(UART3,USART_INT_FLAG_RBNE)){
        len3 %= RX3BUFFER_SIZE;
        rx3buffer[len3++] = usart_data_receive(UART3);
        usart_interrupt_flag_clear(UART3,USART_INT_FLAG_RBNE);
    }

    if(SET == usart_interrupt_flag_get(UART3,USART_INT_FLAG_IDLE)){
        usart_data_receive(UART3);
        #if defined(USART_DMA_RX) && USART_DMA_RX == 3
            dma_channel_disable(UART3_dma_periph,UART3_rx_dma_channel);
            len3 = RX_DMA_BUFFER_SIZE - dma_transfer_number_get(UART3_dma_periph,UART3_rx_dma_channel);
            if(len3){
                rx3buffer[len3] = '\0';
                RX3_recv(rx3buffer,len3);
            }
            len3 = 0;
            dma_flag_clear(UART3_dma_periph,UART3_rx_dma_channel,DMA_FLAG_FTF);
            dma_channel_enable(UART3_dma_periph,UART3_rx_dma_channel);
        #else
            rx3buffer[len3++] = '\0';
            RX3_recv(rx3buffer,len3);
            len3 = 0;
        #endif
    }
}
#endif
#ifdef UART_RX4
uint8_t len4 = 0;
uint8_t rx4buffer[RX4BUFFER_SIZE+1];
void UART4_IRQHandler(void){
    if(SET == usart_interrupt_flag_get(UART4,USART_INT_FLAG_RBNE)){
        len4 %= RX4BUFFER_SIZE;
        rx4buffer[len4++] = usart_data_receive(UART4);
        usart_interrupt_flag_clear(UART4,USART_INT_FLAG_RBNE);
    }

    if(SET == usart_interrupt_flag_get(UART4,USART_INT_FLAG_IDLE)){
        usart_data_receive(UART4);
        #if defined(USART_DMA_RX) && USART_DMA_RX == 4
            dma_channel_disable(UART4_dma_periph,UART4_rx_dma_channel);
            len4 = RX_DMA_BUFFER_SIZE - dma_transfer_number_get(UART4_dma_periph,UART4_rx_dma_channel);
            if(len4){
                rx4buffer[len4] = '\0';
                RX4_recv(rx4buffer,len4);
            }
            len4 = 0;
            dma_flag_clear(UART4_dma_periph,UART4_rx_dma_channel,DMA_FLAG_FTF);
            dma_channel_enable(UART4_dma_periph,UART4_rx_dma_channel);
        #else
            rx4buffer[len4++] = '\0';
            RX4_recv(rx4buffer,len4);
            len4 = 0;
        #endif
    }
}
#endif
#ifdef USART_RX5
uint8_t len5 = 0;
uint8_t rx5buffer[RX5BUFFER_SIZE+1];
void USART5_IRQHandler(void){
    if(SET == usart_interrupt_flag_get(USART5,USART_INT_FLAG_RBNE)){
        len5 %= RX5BUFFER_SIZE;
        rx5buffer[len5++] = usart_data_receive(USART5);
        usart_interrupt_flag_clear(USART5,USART_INT_FLAG_RBNE);
    }

    if(SET == usart_interrupt_flag_get(USART5,USART_INT_FLAG_IDLE)){
        usart_data_receive(USART5);
        #if defined(USART_DMA_RX) && USART_DMA_RX == 5
            dma_channel_disable(USART5_dma_periph,USART5_rx_dma_channel);
            len5 = RX_DMA_BUFFER_SIZE - dma_transfer_number_get(USART5_dma_periph,USART5_rx_dma_channel);
            if(len5){
                rx5buffer[len5] = '\0';
                RX5_recv(rx5buffer,len5);
            }
            len5 = 0;
            dma_flag_clear(USART5_dma_periph,USART5_rx_dma_channel,DMA_FLAG_FTF);
            dma_channel_enable(USART5_dma_periph,USART5_rx_dma_channel);
        #else
            rx5buffer[len5++] = '\0';
            RX5_recv(rx5buffer,len5);
            len5 = 0;
        #endif
    }
}
#endif