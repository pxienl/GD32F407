#include "spi_oled.h"
#include "stdlib.h"
#include "systick.h"
#include "GPIO.h"
#include "stdio.h"

SPI_sw_struct spi;

//反显函数
void SPI_OLED_ColorTurn(uint8_t i)
{
		SPI_OLED_WR_Byte(0xA6 | i,SPI_OLED_CMD);
}

//屏幕旋转180度
void SPI_OLED_DisplayTurn(uint8_t i)
{
	if(i)
	{
		SPI_OLED_WR_Byte(0xC0,SPI_OLED_CMD);//反转显示
		SPI_OLED_WR_Byte(0xA0,SPI_OLED_CMD);
	}else{
		SPI_OLED_WR_Byte(0xC8,SPI_OLED_CMD);//正常显示
		SPI_OLED_WR_Byte(0xA1,SPI_OLED_CMD);
	}
}

//开启OLED显示 
void SPI_OLED_DisPlay_On(void)
{
	SPI_OLED_WR_Byte(0x8D,SPI_OLED_CMD);//电荷泵使能
	SPI_OLED_WR_Byte(0x14,SPI_OLED_CMD);//开启电荷泵
	SPI_OLED_WR_Byte(0xAF,SPI_OLED_CMD);//点亮屏幕
}

//关闭OLED显示 
void SPI_OLED_DisPlay_Off(void)
{
	SPI_OLED_WR_Byte(0x8D,SPI_OLED_CMD);//电荷泵使能
	SPI_OLED_WR_Byte(0x10,SPI_OLED_CMD);//关闭电荷泵
	SPI_OLED_WR_Byte(0xAE,SPI_OLED_CMD);//关闭屏幕
}


//向SSD1306写入一个字节。
//mode:数据/命令标志 0,表示命令;1,表示数据;
void SPI_OLED_WR_Byte(uint8_t dat,uint8_t cmd)
{	
	gpio_bit_write(GPIOA,GPIO_PIN_2,cmd);

	
	// SPI_sw_start(&spi);
	// SPI_sw_transform(&spi,dat); 
	// SPI_sw_stop(&spi);

	gpio_bit_reset(GPIOA,GPIO_PIN_3);
	// SPI_hw_transform(SPI0,dat);
	SPI_OLED_SEND(&dat);
	gpio_bit_set(GPIOA,GPIO_PIN_3);

	gpio_bit_set(GPIOA,GPIO_PIN_2);
}

//清屏函数
void SPI_OLED_Clear(void)
{
	uint8_t i,n;
	for(i=0;i<8;i++)
	{
	 	 SPI_OLED_WR_Byte(0xb0+i,SPI_OLED_CMD);//设置页地址
	   SPI_OLED_WR_Byte(0x10,SPI_OLED_CMD);  //设置列地址的高4位
	   SPI_OLED_WR_Byte(0x00,SPI_OLED_CMD);  //设置列地址的低4位
	   for(n=0;n<128;n++)
		 {
			 SPI_OLED_WR_Byte(0x00,SPI_OLED_DATA);//清除所有数据
		 }
  }
}

//设置起始地址
void SPI_OLED_address(uint8_t x,uint8_t y)
{
	SPI_OLED_WR_Byte(0xb0+y,SPI_OLED_CMD);              //设置页地址
	SPI_OLED_WR_Byte(((x&0xf0)>>4)|0x10,SPI_OLED_CMD);  //设置列地址的高4位
	SPI_OLED_WR_Byte((x&0x0f),SPI_OLED_CMD);            //设置列地址的低4位
}

//显示128x64点阵图像
void SPI_OLED_Display_128x64(uint8_t *dp)
{
	uint8_t i,j;
	for(i=0;i<8;i++)
	{
		SPI_OLED_address(0,i);
		for(j=0;j<128;j++)
		{
			SPI_OLED_WR_Byte(*dp,SPI_OLED_DATA); //写数据到OLED,每写完一个8位的数据后列地址自动加1
			dp++;
    }
  }
}

//显示16x16点阵图像、汉字、生僻字或16x16点阵的其他图标
void SPI_OLED_Display_16x16(uint8_t x,uint8_t y,uint8_t *dp)
{
	uint8_t i,j;
	for(j=0;j<2;j++)
	{
		SPI_OLED_address(x,y);
		for(i=0;i<16;i++)
		{
			SPI_OLED_WR_Byte(*dp,SPI_OLED_DATA);  //写数据到OLED,每写完一个8位的数据后列地址自动加1
			dp++;
    }
		y++;
  }
}

//显示8x16点阵图像、ASCII, 或8x16点阵的自造字符、其他图标
void SPI_OLED_Display_8x16(uint8_t x,uint8_t y,uint8_t *dp)
{
	uint8_t i,j;
	for(j=0;j<2;j++)
	{
		SPI_OLED_address(x,y);
		for(i=0;i<8;i++)
		{
			SPI_OLED_WR_Byte(*dp,SPI_OLED_DATA);   //写数据到LCD,每写完一个8位的数据后列地址自动加1
			dp++;
    }
		y++;
  }
}

//显示5*7点阵图像、ASCII, 或5x7点阵的自造字符、其他图标
void SPI_OLED_Display_5x7(uint8_t x,uint8_t y,uint8_t *dp)
{
	uint8_t i;
  SPI_OLED_address(x,y);
	for(i=0;i<6;i++)
	{
		SPI_OLED_WR_Byte(*dp,SPI_OLED_DATA);
		dp++;
  }
}

//送指令到晶联讯字库IC
void Send_Command_to_ROM(uint8_t dat)
{
	// SPI_sw_transform(&spi,dat);
	SPI_OLED_SEND(&dat);
	// SPI_hw_transform(SPI0,dat);
}

//从晶联讯字库IC中取汉字或字符数据（1个字节）
uint8_t Get_data_from_ROM(void)
{
	// return SPI_sw_transform(&spi,0x00);
	return SPI_OLED_RECV();
	// return SPI_hw_transform(SPI0,0x00);
}


//从相关地址（addrHigh：地址高字节,addrMid：地址中字节,addrLow：地址低字节）中连续读出DataLen个字节的数据到 pbuff的地址
//连续读取
void SPI_OLED_get_data_from_ROM(uint8_t addrHigh,uint8_t addrMid,uint8_t addrLow,uint8_t *pbuff,uint8_t DataLen)
{
	uint8_t i;
	SPI_OLED_ROM_CS_Clr();
	Send_Command_to_ROM(0x03);
	Send_Command_to_ROM(addrHigh);
	Send_Command_to_ROM(addrMid);
	Send_Command_to_ROM(addrLow);
	for(i=0;i<DataLen;i++)
	{
		*(pbuff+i)=Get_data_from_ROM();
  }
	SPI_OLED_ROM_CS_Set();
}


uint32_t fontaddr=0;
void SPI_OLED_Display_GB2312_string(uint8_t x,uint8_t y,char *text)
{
	uint8_t i=0;
	uint8_t addrHigh,addrMid,addrLow;
	uint8_t fontbuf[32];
	while(text[i]>0x00)
	{
		if((text[i]>=0xb0)&&(text[i]<=0xf7)&&(text[i+1]>=0xa1))
		{
			//国标简体（GB2312）汉字在晶联讯字库IC中的地址由以下公式来计算：
			//Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+ 846)*32+ BaseAdd;BaseAdd=0
			//由于担心8位单片机有乘法溢出问题，所以分三部取地址
			fontaddr=(text[i]-0xb0)*94;
			fontaddr+=(text[i+1]-0xa1)+846;
			fontaddr=fontaddr*32;
			
			addrHigh=(fontaddr&0xff0000)>>16;   //地址的高8位,共24位
			addrMid=(fontaddr&0xff00)>>8;       //地址的中8位,共24位
			addrLow=(fontaddr&0xff);            //地址的低8位,共24位
			
			SPI_OLED_get_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,32);
			//取32个字节的数据，存到"fontbuf[32]"
			SPI_OLED_Display_16x16(x,y,fontbuf);
			//显示汉字到LCD上，y为页地址，x为列地址，fontbuf[]为数据
			x+=16;
			i+=2;
    }
		else if((text[i]>=0xa1)&&(text[i]<=0xa3)&&(text[i+1]>=0xa1))
		{
			
			fontaddr=(text[i]-0xa1)*94;
			fontaddr+=(text[i+1]-0xa1);
			fontaddr=fontaddr*32;
			
			addrHigh=(fontaddr&0xff0000)>>16;
			addrMid=(fontaddr&0xff00)>>8;
			addrLow=(fontaddr&0xff);
			
			SPI_OLED_get_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,32);
			SPI_OLED_Display_16x16(x,y,fontbuf);
			x+=16;
			i+=2;
    }
		else if((text[i]>=0x20)&&(text[i]<=0x7e))
		{
			unsigned char fontbuf[16];
			fontaddr=(text[i]-0x20);
			fontaddr=(unsigned long)(fontaddr*16);
			fontaddr=(unsigned long)(fontaddr+0x3cf80);
			
			addrHigh=(fontaddr&0xff0000)>>16;
			addrMid=(fontaddr&0xff00)>>8;
			addrLow=fontaddr&0xff;
			
			SPI_OLED_get_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,16);
			SPI_OLED_Display_8x16(x,y,fontbuf);
			x+=8;
			i+=1;
    }
		else 
			i++;
  }
}

void SPI_OLED_Display_string_5x7(uint8_t x,uint8_t y,char *text)
{
	uint8_t i=0;
	uint8_t addrHigh,addrMid,addrLow;
	while(text[i]>0x00)
	{
		if((text[i]>=0x20)&&(text[i]<=0x7e))
		{

			uint8_t fontbuf[8];
			fontaddr=(text[i]-0x20);
			fontaddr=(unsigned long)(fontaddr*8);
			fontaddr=(unsigned long)(fontaddr+0x3bfc0);
			
			addrHigh=(fontaddr&0xff0000)>>16;
			addrMid=(fontaddr&0xff00)>>8;
			addrLow=fontaddr&0xff;
			
			SPI_OLED_get_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,8);
			SPI_OLED_Display_5x7(x,y,fontbuf);

			for (uint8_t i = 0; i < 8; i++)
			{
				/* code */
				printf("%x ",fontbuf[i]);
			}
			printf("\n");
			x+=6;
			i+=1;
    }
		else 
			i++;
  }
}

//显示2个数字
//x,y :起点坐标
//num1：要显示的小数
//len :数字的位数
void SPI_OLED_ShowNum(uint8_t x,uint8_t y,float num1,uint8_t len)
{
	uint8_t i;
	uint32_t t,num;
	x=x+len*8+8;//要显示的小数最低位的横坐标
	num=num1*100;//将小数左移两位并转化为整数
	SPI_OLED_Display_GB2312_string(x-24,y,".");//显示小数点
	for(i=0;i<len;i++)
	{
		t=num%10;//取个位数的数值
		num=num/10;//将整数右移一位
		x-=8;
		if(i==2){x-=8;}//当显示出来两个小数之后，空出小数点的位置
		switch(t)
		{
			case 0 :SPI_OLED_Display_GB2312_string(x,y,"0");break;
			case 1 :SPI_OLED_Display_GB2312_string(x,y,"1");break;
			case 2 :SPI_OLED_Display_GB2312_string(x,y,"2");break;
			case 3 :SPI_OLED_Display_GB2312_string(x,y,"3");break;
			case 4 :SPI_OLED_Display_GB2312_string(x,y,"4");break;
			case 5 :SPI_OLED_Display_GB2312_string(x,y,"5");break;
			case 6 :SPI_OLED_Display_GB2312_string(x,y,"6");break;
			case 7 :SPI_OLED_Display_GB2312_string(x,y,"7");break;
			case 8 :SPI_OLED_Display_GB2312_string(x,y,"8");break;
			case 9 :SPI_OLED_Display_GB2312_string(x,y,"9");break;
		}
	}
}

//OLED的初始化
void SPI_OLED_Init(void)
{
	
	// spi.SCLK_GPIO = GPIOA;
	// spi.SCLK_PIN 	= GPIO_PIN_5;
	// spi.MOSI_GPIO = GPIOA;
	// spi.MOSI_PIN 	= GPIO_PIN_7;
	// spi.MISO_GPIO = GPIOA;
	// spi.MISO_PIN 	= GPIO_PIN_6;
	// spi.CS_GPIO 	= GPIOA;
	// spi.CS_PIN 		= GPIO_PIN_3;

	// spi.CPOL 			= 0;
	// spi.CPHA 			= 0;
	// spi.BitFirst 	= 1;
	// spi.Freq 			= 20;
	
	// SPI_sw_struct_init(&spi);

	GPIO_output_init(GPIOA,GPIO_PUPD_PULLUP,GPIO_OTYPE_OD,GPIO_PIN_3);
	GPIO_af_init(GPIOA,GPIO_AF_5,GPIO_PUPD_NONE,GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

	SPI_hw_init(SPI0);

	GPIO_output_init(GPIOA,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_PIN_2); // DC
	GPIO_output_init(GPIOC,GPIO_PUPD_PULLUP,GPIO_OTYPE_OD,GPIO_PIN_5); // CS2
	gpio_bit_set(GPIOC,GPIO_PIN_5);
	gpio_bit_set(GPIOA,GPIO_PIN_2);

	// gpio_bit_set(GPIOA,GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
	
	// delay_1ms(200);
	
	SPI_OLED_WR_Byte(0xAE,SPI_OLED_CMD);//--turn off oled panel
	SPI_OLED_WR_Byte(0x00,SPI_OLED_CMD);//---set low column address
	SPI_OLED_WR_Byte(0x10,SPI_OLED_CMD);//---set high column address
	SPI_OLED_WR_Byte(0x40,SPI_OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	SPI_OLED_WR_Byte(0x81,SPI_OLED_CMD);//--set contrast control register
	SPI_OLED_WR_Byte(0xCF,SPI_OLED_CMD); // Set SEG Output Current Brightness
	SPI_OLED_WR_Byte(0xA1,SPI_OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	SPI_OLED_WR_Byte(0xC8,SPI_OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	SPI_OLED_WR_Byte(0xA6,SPI_OLED_CMD);//--set normal display
	SPI_OLED_WR_Byte(0xA8,SPI_OLED_CMD);//--set multiplex ratio(1 to 64)
	SPI_OLED_WR_Byte(0x3f,SPI_OLED_CMD);//--1/64 duty
	SPI_OLED_WR_Byte(0xD3,SPI_OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	SPI_OLED_WR_Byte(0x00,SPI_OLED_CMD);//-not offset
	SPI_OLED_WR_Byte(0xd5,SPI_OLED_CMD);//--set display clock divide ratio/oscillator frequency
	SPI_OLED_WR_Byte(0x80,SPI_OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	SPI_OLED_WR_Byte(0xD9,SPI_OLED_CMD);//--set pre-charge period
	SPI_OLED_WR_Byte(0xF1,SPI_OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	SPI_OLED_WR_Byte(0xDA,SPI_OLED_CMD);//--set com pins hardware configuration
	SPI_OLED_WR_Byte(0x12,SPI_OLED_CMD);
	SPI_OLED_WR_Byte(0xDB,SPI_OLED_CMD);//--set vcomh
	SPI_OLED_WR_Byte(0x40,SPI_OLED_CMD);//Set VCOM Deselect Level
	SPI_OLED_WR_Byte(0x20,SPI_OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	SPI_OLED_WR_Byte(0x02,SPI_OLED_CMD);//
	SPI_OLED_WR_Byte(0x8D,SPI_OLED_CMD);//--set Charge Pump enable/disable
	SPI_OLED_WR_Byte(0x14,SPI_OLED_CMD);//--set(0x10) disable
	SPI_OLED_WR_Byte(0xA4,SPI_OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	SPI_OLED_WR_Byte(0xA6,SPI_OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
	SPI_OLED_Clear();
	SPI_OLED_WR_Byte(0xAF,SPI_OLED_CMD); /*display ON*/ 
}


