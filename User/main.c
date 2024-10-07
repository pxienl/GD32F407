#include "main.h"
#include "systick.h"
#include "DMA.h"
#include "USART.h"
#include "GPIO.h"
#include "u8g2/u8g2.h"
#include "I2C.h"

// #define I2C_SOFTWARE

void RX0_recv(uint8_t *rxbuffer,uint32_t len){
    usart_dma_send(USART0,rxbuffer,len);
}


uint8_t u8g2_gpio_and_delay_gd32_software(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
	switch(msg){
		//Initialize SPI peripheral
		case U8X8_MSG_GPIO_AND_DELAY_INIT:
			/* HAL initialization contains all what we need so we can skip this part. */

		break;

		//Function which implements a delay, arg_int contains the amount of ms
		case U8X8_MSG_DELAY_MILLI:
		    delay_1ms(arg_int);

		break;
		//Function which delays 10us
		case U8X8_MSG_DELAY_10MICRO:
		    delay_1us(10);

		break;
		//Function which delays 100ns
		case U8X8_MSG_DELAY_100NANO:
		    __NOP();

		break;

    #ifdef I2C_SOFTWARE
        case U8X8_MSG_GPIO_I2C_CLOCK:
			if (arg_int) gpio_bit_set(GPIOB,GPIO_PIN_6);
			else gpio_bit_reset(GPIOB,GPIO_PIN_6);
        break;

        case U8X8_MSG_GPIO_I2C_DATA:
            if (arg_int) gpio_bit_set(GPIOB,GPIO_PIN_7);
			else gpio_bit_reset(GPIOB,GPIO_PIN_7);
		break;
    #endif

		default:
			return 0; //A message was received which is not implemented, return 0 to indicate an error
	}

	return 1; // command processed successfully.
}

uint8_t u8g2_byte_cb_hardware(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	switch(msg){
        case U8X8_MSG_BYTE_SEND:
          for (uint32_t i = 0; i < arg_int; i++)
          {
            I2C_hardware_send(I2C0,*((uint8_t *)arg_ptr + i));
          }
          
          break;
        case U8X8_MSG_BYTE_INIT:
          break;
        case U8X8_MSG_BYTE_SET_DC:
          break;
        case U8X8_MSG_BYTE_START_TRANSFER:
          if ( u8x8->display_info->i2c_bus_clock_100kHz >= 4 )
          {
    	      i2c_clock_config(I2C0,400000,I2C_DTCY_2);
          }

          I2C_hardware_start(I2C0,1);
          I2C_hardware_address(I2C0,u8x8_GetI2CAddress(u8x8),I2C_TRANSMITTER);
          break;
        case U8X8_MSG_BYTE_END_TRANSFER:
          I2C_hardware_stop(I2C0);
          break;

		default:
			return 0; //A message was received which is not implemented, return 0 to indicate an error
	}

	return 1; // command processed successfully.
}

int main(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    systick_config();
    usart_init(USART0,GPIOA,GPIO_AF_7,GPIO_PIN_9 | GPIO_PIN_10);
    usart_dma_tx_init(USART0,DMA1,DMA_CH7);
    usart_dma_rx_init(USART0,DMA1,DMA_CH5);

    printf("init");

    u8g2_t u8g2;

    #ifdef I2C_SOFTWARE
      GPIO_output_init(GPIOB,GPIO_PUPD_PULLUP,GPIO_OTYPE_OD,GPIO_PIN_6|GPIO_PIN_7);
      u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2,U8G2_R0,u8x8_byte_sw_i2c,u8g2_gpio_and_delay_gd32_software);
    #else
      GPIO_af_init(GPIOB,GPIO_AF_4,GPIO_PUPD_PULLUP,GPIO_PIN_6|GPIO_PIN_7);
      I2C_hardware_init(I2C0,100000);


      /*!
          \brief    initialize u8g2 with full frame buffer
          \param[in]  rotation: Rotation procedure
            \arg        U8G2_R0: No rotation, landscape
            \arg        U8G2_R1: 90 degree clockwise rotation
            \arg        U8G2_R2: 180 degree clockwise rotation
            \arg        U8G2_R3: 270 degree clockwise rotation
            \arg        U8G2_MIRROR: No rotation, landscape, display content is mirrored (v2.6.x)
          \param[in]  byte_cb: Either one of the existing procedures or a custom procedure for the target controller.
            \arg        u8x8_byte_4wire_sw_spi: Standard 8-bit SPI communication with "four pins" (SCK, MOSI, DC, CS)
            \arg        u8x8_byte_3wire_sw_spi: 9-bit communication with "three pins" (SCK, MOSI, CS)
            \arg        u8x8_byte_8bit_6800mode: Parallel interface, 6800 format
            \arg        u8x8_byte_8bit_8080mode: Parallel interface, 8080 format
            \arg        u8x8_byte_sw_i2c: Two wire, I2C communication
            \arg        u8x8_byte_ks0108: Special interface for KS0108 controller
          \param[in]  gpio_and_delay_cb:  Lowlevel delay and GPIO procedure

          \retval     none
      */
      u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2,U8G2_R0,u8g2_byte_cb_hardware,u8g2_gpio_and_delay_gd32_software);
    #endif

    u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
    u8g2_SetPowerSave(&u8g2, 0); // wake up display
    // 画两条线试试
    u8g2_DrawLine(&u8g2, 0,0, 127, 63);
    u8g2_DrawLine(&u8g2, 127,0 , 0,63);
    u8g2_SendBuffer(&u8g2); // 将数据发送到屏幕

	while(1) 
	{
		
	}
}
