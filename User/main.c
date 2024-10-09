#include "main.h"
#include "systick.h"
#include "DMA.h"
#include "USART.h"
#include "ADC.h"
#include "GPIO.h"
#include "spi_oled.h"

int main(void)
{
    float o,vntc;
    uint16_t i,adc;
    int c;
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    systick_config();
    usart_init(USART0,GPIOA,GPIO_AF_7,GPIO_PIN_9 | GPIO_PIN_10);
    usart_dma_tx_init(USART0,DMA1,DMA_CH7);
    // usart_dma_rx_init(USART0,DMA1,DMA_CH5);

  	uint8_t t=0;
		delay_1ms(168);
 		SPI_OLED_Init();				//初始化OLED
		t=' ';
		SPI_OLED_ColorTurn(0);//0正常显示，1 反色显示
  	SPI_OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
		
		uint8_t a[] = {0x80,0x80,0x80,0x80,0x01}; /*"_",0*/
		SPI_OLED_Display_5x7(0,7,a);/*显示一串5x7点阵的ASCII字*/
		SPI_OLED_Display_5x7(5,7,a);/*显示一串5x7点阵的ASCII字*/
		SPI_OLED_Display_5x7(10,7,a);/*显示一串5x7点阵的ASCII字*/
		SPI_OLED_Display_string_5x7(16,7,"ABCDEFG");/*显示一串5x7点阵的ASCII字*/

		SPI_OLED_Display_GB2312_string(0,0,"12864,如果需要输入中文需要将该文件编码保存为 GBK");	/*在第1页，第1列，显示一串16x16点阵汉字或8x16的ASCII字*/
		while(1) 
		{
			delay_1ms(1000);
			// SPI_OLED_Clear();
	}
}
