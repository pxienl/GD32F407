#include "cst816t.h"

#include <stdio.h>
#include "I2C.h"
#include "GPIO.h"
#include "EXTI.h"
#include "systick.h"

static uint16_t X_Axis;
static uint16_t Y_Axis=0;
static uint8_t fingerNum = 0;

void CST816T_GPIOInit(void)
{
	GPIO_output_init(GPIOD,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_PIN_7);
	EXTI_init(GPIOB,GPIO_PUPD_PULLUP,EXTI_4,3,3,EXTI_TRIG_FALLING);

	//初始化
	_CST816T_RESET_HIGH_();
}


void CST816T_SendByte(uint8_t Addr,uint8_t* Data)
{
	I2C_hardware_write(I2C0,0x2A,Addr,Data,1);
}

void CST816T_ReceiveByte(uint8_t Addr,uint8_t* Data)
{
	I2C_hardware_read(I2C0,0x2A,Addr,Data,1);
}

void CST816_GetAction(void)
{
	uint8_t data[6];uint16_t X=0,Y=0;
	
	CST816T_ReceiveByte(0x01,data);
	CST816T_ReceiveByte(0x03,data+1);
	CST816T_ReceiveByte(0x04,data+2);
	CST816T_ReceiveByte(0x05,data+3);
	CST816T_ReceiveByte(0x06,data+4);
	// 读手指的个数
	CST816T_ReceiveByte(0x02,data+5);
	
	//State=data[0];
	X=(uint16_t)((data[1]&0x0F)<<8)|data[2];//(temp[0]&0X0F)<<4|
	Y=(uint16_t)((data[3]&0x0F)<<8)|data[4];//(temp[2]&0X0F)<<4|
	
	if(X<TOUCH_W&&Y<TOUCH_H)
	{
		X_Axis=X;
		Y_Axis=Y;
	}
	
	// 手指的数量
	fingerNum = data[5];
	printf("X:%d,Y:%d finger:%d \t\n",X_Axis,Y_Axis,fingerNum);
	if(fingerNum > 5){
		uint8_t ChipID=0;
		uint8_t FwVersion=0;
		CST816T_ReceiveByte(0xa7,&ChipID);
		CST816T_ReceiveByte(0xa9,&FwVersion);
		printf("ChipID:%X\r\n",ChipID);
		printf("FwVersion:%d\r\n",FwVersion);
	}		
}

void CST816T_Reset(void)
{
	_CST816T_RESET_LOW_();
	delay_1ms(10);
	_CST816T_RESET_HIGH_();
	delay_1ms(50);
}

void CST816T_Init(void)
{
	uint8_t ChipID=0;
	uint8_t FwVersion=0;
	
	GPIO_af_init(GPIOB,GPIO_AF_4,GPIO_PUPD_NONE,GPIO_PIN_6 | GPIO_PIN_7);
	I2C_hardware_init(I2C0,100000);

	CST816T_GPIOInit();
	CST816T_Reset( );//芯片上电初始化
	
	// 延时
	delay_1ms(100);
	
	CST816T_ReceiveByte(0xa7,&ChipID);
	CST816T_ReceiveByte(0xa9,&FwVersion);
	printf("ChipID:%X\r\n",ChipID);
	printf("FwVersion:%d\r\n",FwVersion);
	CST816T_Reset();
}

void CST816T_ClrBaseDatas(void)
{
	//State=CST816T_STATE_RELEASE;
	X_Axis=0;
	Y_Axis=0;
	fingerNum=0;
}



void EXTI4_IRQHandler(void)
{
	 
	 if(exti_interrupt_flag_get(CST816T_EXTI_Line) == SET) {
			exti_interrupt_flag_clear(CST816T_EXTI_Line);
			CST816_GetAction();
			//printf("touch down\r\n");
	 }else{
		  CST816_GetAction();
			//CST816T_ClrBaseDatas();
		  //printf("touch release\r\n");
	 }
	 
	 exti_interrupt_flag_clear(CST816T_EXTI_Line); 
}

uint8_t CST816T_is_pressed(void){
	
	//CST816_GetAction();
	
	if(fingerNum > 0 && X_Axis>0 && Y_Axis>0){
		return 1;
	}else{
		return 0;
	}
}


void CST816T_get_xy(uint16_t* x,uint16_t* y){
	*x = X_Axis;
	*y = Y_Axis;
}