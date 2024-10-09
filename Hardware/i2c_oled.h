#ifndef __OLED_H
#define __OLED_H 

#include "gd32f4xx.h"
#include "I2C.h"
#include "stdlib.h"	

#define SCREEN_I2C_WIDTH 128
#define SCREEN_I2C_HEIGHT 64
#define SCREEN_I2C_WIDTH_SIZE SCREEN_I2C_WIDTH
#define SCREEN_I2C_HEIGHT_SIZE (SCREEN_I2C_HEIGHT / 8)

// #define SCREEN_I2C_USE_SOFTWARE

#ifdef SCREEN_I2C_USE_SOFTWARE
  #define SCREEN_I2C_SOFTWARE_SCL_GPIO GPIOB
  #define SCREEN_I2C_SOFTWARE_SCL_PIN GPIO_PIN_6
  #define SCREEN_I2C_SOFTWARE_SDA_GPIO GPIOB
  #define SCREEN_I2C_SOFTWARE_SDA_PIN GPIO_PIN_7
  #define SCREEN_I2C_SOFTWARE_SPEED 50
  extern I2C_soft_struct SCREEN_I2C;
  #define SCREEN_I2C_READ(addr, reg, data, len) I2C_soft_read(&SCREEN_I2C, addr, reg, data, len)
  #define SCREEN_I2C_WRITE(addr, reg, data, len) I2C_soft_write(&SCREEN_I2C, addr, reg, data, len)
#else
  #define SCREEN_I2C_HARDWARE_PERIPH I2C0
  #define SCREEN_I2C_HARDWARE_SPEED 100000U
  #define SCREEN_I2C_READ(addr, reg, data, len) I2C_hardware_read(SCREEN_I2C_HARDWARE_PERIPH, addr, reg, data, len)
  #define SCREEN_I2C_WRITE(addr, reg, data, len) I2C_hardware_write(SCREEN_I2C_HARDWARE_PERIPH, addr, reg, data, len)
#endif

//-----------------OLED端口定义---------------- 

#define I2C_OLED_CMD  0	//写命令
#define I2C_OLED_DATA 1	//写数据

void I2C_OLED_ColorTurn(uint8_t i);
void I2C_OLED_DisplayTurn(uint8_t i);
void I2C_OLED_WR_Byte(uint8_t dat,uint8_t mode);
void I2C_OLED_DisPlay_On(void);
void I2C_OLED_DisPlay_Off(void);
void I2C_OLED_Refresh(void);
void I2C_OLED_Clear(void);
void I2C_OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void I2C_OLED_DrawLine(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t mode);
void I2C_OLED_DrawCircle(uint8_t x,uint8_t y,uint8_t r);
void I2C_OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size1,uint8_t mode);
void I2C_OLED_ShowString(uint8_t x,uint8_t y,char *chr,uint8_t size1,uint8_t mode);
void I2C_OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size1,uint8_t mode);
void I2C_OLED_ShowPicture(uint8_t x,uint8_t y,uint8_t sizex,uint8_t sizey,uint8_t BMP[],uint8_t mode);
void I2C_OLED_Init(void);

#endif

