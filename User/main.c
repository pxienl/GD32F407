#include "main.h"
#include "systick.h"
#include "DMA.h"
#include "USART.h"
#include "ADC.h"
#include "GPIO.h"
#include "SPI.h"
#include "u8g2/u8g2.h"

uint8_t u8x8_byte_arduino_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
  uint8_t *data;
  uint8_t internal_spi_mode; 
  switch(msg) {
    case U8X8_MSG_BYTE_SEND:
			data = (uint8_t *)arg_ptr;
			while(arg_int > 0){
				SPI_hw_transform(SPI0,(uint8_t)*data);
				data++;
        arg_int--;
			}
      break;
    case U8X8_MSG_BYTE_INIT:
			GPIO_output_init(GPIOA,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_7);
			GPIO_af_init(GPIOA,GPIO_AF_5,GPIO_PUPD_NONE,GPIO_PIN_5|GPIO_PIN_7);
			gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5|GPIO_PIN_7);
      SPI_hw_init(SPI0);

      gpio_bit_set(GPIOA,GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_7);
			
			// gpio_bit_write(GPIOA,GPIO_PIN_2,u8x8->display_info->chip_disable_level);

      // delay_1ms(1000);
      break;
    case U8X8_MSG_BYTE_SET_DC:
      gpio_bit_write(GPIOA,GPIO_PIN_2,!!arg_int);
      break;
    case U8X8_MSG_BYTE_START_TRANSFER:
      gpio_bit_write(GPIOA,GPIO_PIN_3, 0);
      break;
    case U8X8_MSG_BYTE_END_TRANSFER:      
      gpio_bit_write(GPIOA,GPIO_PIN_3, 1);
      break;
    default:
      return 0;
  }  
  return 1;
}



uint8_t u8x8_gpio_and_delay_template(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  switch(msg)
  {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:	// called once during init phase of u8g2/u8x8
			GPIO_output_init(GPIOA,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_PIN_5|GPIO_PIN_7|GPIO_PIN_3|GPIO_PIN_2);
			delay_1ms(200);
      break;							// can be used to setup pins
    case U8X8_MSG_DELAY_10MICRO:		// delay arg_int * 10 micro seconds
			delay_1us(10);
      break;
    case U8X8_MSG_DELAY_MILLI:			// delay arg_int * 1 milli second
		  delay_1ms(1);
      break;
    case U8X8_MSG_GPIO_SPI_CLOCK:
		// 	gpio_bit_write(GPIOA,GPIO_PIN_5,arg_int);
      break;
    case U8X8_MSG_GPIO_SPI_DATA:
		// 	gpio_bit_write(GPIOA,GPIO_PIN_7,arg_int);
      break;
    case U8X8_MSG_GPIO_CS:				// CS (chip select) pin: Output level in arg_int
			gpio_bit_write(GPIOA,GPIO_PIN_3,arg_int);
      break;
		case U8X8_MSG_GPIO_DC:
		  gpio_bit_write(GPIOA,GPIO_PIN_2,arg_int);
    default:
      // u8x8_SetGPIOResult(u8x8, 1);			// default return value
      break;
  }
  return 1;
}

int main(void)
{
		u8g2_t u8g2;

    float o,vntc;
    uint16_t i,adc;
    int c;
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    systick_config();
    usart_init(USART0,GPIOA,GPIO_AF_7,GPIO_PIN_9 | GPIO_PIN_10);
    usart_dma_tx_init(USART0,DMA1,DMA_CH7);
    // usart_dma_rx_init(USART0,DMA1,DMA_CH5);

	

		u8g2_Setup_ssd1306_128x64_noname_f(&u8g2,U8G2_R0,u8x8_byte_arduino_hw_spi,u8x8_gpio_and_delay_template);
		u8g2_InitDisplay(&u8g2);
		u8g2_SetPowerSave(&u8g2, 0);
		// delay_1ms(1000);
    u8g2_DrawCircle(&u8g2,80, 20, 10, U8G2_DRAW_ALL);
		// u8g2_DrawLine(&u8g2, 0,0, 127, 63);
		
		// u8g2_DrawLine(&u8g2, 127,0 , 0,63);
		u8g2_SendBuffer(&u8g2);


		while(1) 
		{

		}
}
