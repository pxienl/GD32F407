#include "i2c_oled.h"
#include "stdlib.h"
#include "oledfont.h"

#ifdef SCREEN_I2C_USE_SOFTWARE
	I2C_soft_struct SCREEN_I2C;
#endif

// uint8_t I2C_OLED_GRAM[SCREEN_I2C_WIDTH_SIZE][SCREEN_I2C_HEIGHT_SIZE];
uint8_t I2C_OLED_GRAM[SCREEN_I2C_HEIGHT_SIZE][SCREEN_I2C_WIDTH_SIZE];

//反显函数
void I2C_OLED_ColorTurn(uint8_t i)
{
	if(i==0)
		{
			I2C_OLED_WR_Byte(0xA6,I2C_OLED_CMD);//正常显示
		}
	if(i==1)
		{
			I2C_OLED_WR_Byte(0xA7,I2C_OLED_CMD);//反色显示
		}
}

//屏幕旋转180度
void I2C_OLED_DisplayTurn(uint8_t i)
{
	if(i==0)
		{
			I2C_OLED_WR_Byte(0xC8,I2C_OLED_CMD);//正常显示
			I2C_OLED_WR_Byte(0xA1,I2C_OLED_CMD);
		}
	if(i==1)
		{
			I2C_OLED_WR_Byte(0xC0,I2C_OLED_CMD);//反转显示
			I2C_OLED_WR_Byte(0xA0,I2C_OLED_CMD);
		}
}

//发送一个字节
//mode:数据/命令标志 0,表示命令;1,表示数据;
void I2C_OLED_WR_Byte(uint8_t dat,uint8_t mode)
{
	SCREEN_I2C_WRITE(0x78,mode?0x40:0x00,&dat,1);
}

//开启OLED显示 
void I2C_OLED_DisPlay_On(void)
{
	I2C_OLED_WR_Byte(0x8D,I2C_OLED_CMD);//电荷泵使能
	I2C_OLED_WR_Byte(0x14,I2C_OLED_CMD);//开启电荷泵
	I2C_OLED_WR_Byte(0xAF,I2C_OLED_CMD);//点亮屏幕
}

//关闭OLED显示 
void I2C_OLED_DisPlay_Off(void)
{
	I2C_OLED_WR_Byte(0x8D,I2C_OLED_CMD);//电荷泵使能
	I2C_OLED_WR_Byte(0x10,I2C_OLED_CMD);//关闭电荷泵
	I2C_OLED_WR_Byte(0xAE,I2C_OLED_CMD);//关闭屏幕
}

//更新显存到OLED	
void I2C_OLED_Refresh(void)
{
	uint8_t y,x;
	for(y=0;y<SCREEN_I2C_HEIGHT_SIZE;y++)
	{
		I2C_OLED_WR_Byte(0xb0+y,I2C_OLED_CMD); //设置行起始地址
		I2C_OLED_WR_Byte(0x00,I2C_OLED_CMD);   //设置低列起始地址
		I2C_OLED_WR_Byte(0x10,I2C_OLED_CMD);   //设置高列起始地址
		SCREEN_I2C_WRITE(0x78,0x40,I2C_OLED_GRAM[y],SCREEN_I2C_WIDTH_SIZE);
  }
}
//清屏函数
void I2C_OLED_Clear(void)
{
	uint32_t len = SCREEN_I2C_WIDTH_SIZE * SCREEN_I2C_HEIGHT_SIZE;
	uint8_t *p = (uint8_t *)I2C_OLED_GRAM;
	while (--len)
	{
		*p = 0;
		p++;
	}
	
	I2C_OLED_Refresh();//更新显示
}

//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空	
void I2C_OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t)
{
	uint8_t i,m,n;
	i=y/8;
	m=y%8;
	n=1<<m;
	if(t) I2C_OLED_GRAM[i][x]|=n;
	else I2C_OLED_GRAM[i][x]&=~n;
}

//画线
//x1,y1:起点坐标
//x2,y2:结束坐标
void I2C_OLED_DrawLine(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t mode)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1;
	uRow=x1;//画线起点坐标
	uCol=y1;
	if(delta_x>0)incx=1; //设置单步方向 
	else if (delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//水平线 
	else {incy=-1;delta_y=-delta_x;}
	if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		I2C_OLED_DrawPoint(uRow,uCol,mode);//画点
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}
//x,y:圆心坐标
//r:圆的半径
void I2C_OLED_DrawCircle(uint8_t x,uint8_t y,uint8_t r)
{
	int a, b,num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r)      
    {
        I2C_OLED_DrawPoint(x + a, y - b,1);
        I2C_OLED_DrawPoint(x - a, y - b,1);
        I2C_OLED_DrawPoint(x - a, y + b,1);
        I2C_OLED_DrawPoint(x + a, y + b,1);
 
        I2C_OLED_DrawPoint(x + b, y + a,1);
        I2C_OLED_DrawPoint(x + b, y - a,1);
        I2C_OLED_DrawPoint(x - b, y - a,1);
        I2C_OLED_DrawPoint(x - b, y + a,1);
        
        a++;
        num = (a * a + b * b) - r*r;//计算画的点离圆心的距离
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}



//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//size1:选择字体 6x8/6x12/8x16/12x24
//mode:0,反色显示;1,正常显示
void I2C_OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size1,uint8_t mode)
{
	uint8_t i,m,temp,size2,chr1;
	uint8_t x0=x,y0=y;
	if(size1==8)size2=6;
	else size2=(size1/8+((size1%8)?1:0))*(size1/2);  //得到字体一个字符对应点阵集所占的字节数
	chr1=chr-' ';  //计算偏移后的值
	for(i=0;i<size2;i++)
	{
		if(size1==8)
			  {temp=asc2_0806[chr1][i];} //调用0806字体
		else if(size1==12)
        {temp=asc2_1206[chr1][i];} //调用1206字体
		else if(size1==16)
        {temp=asc2_1608[chr1][i];} //调用1608字体
		else if(size1==24)
        {temp=asc2_2412[chr1][i];} //调用2412字体
		else return;
		for(m=0;m<8;m++)
		{
			if(temp&0x01)I2C_OLED_DrawPoint(x,y,mode);
			else I2C_OLED_DrawPoint(x,y,!mode);
			temp>>=1;
			y++;
		}
		x++;
		if((size1!=8)&&((x-x0)==size1/2))
		{x=x0;y0=y0+8;}
		y=y0;
  }
}


//显示字符串
//x,y:起点坐标  
//size1:字体大小 
//*chr:字符串起始地址 
//mode:0,反色显示;1,正常显示
void I2C_OLED_ShowString(uint8_t x,uint8_t y,char *chr,uint8_t size1,uint8_t mode)
{
	while((*chr>=' ')&&(*chr<='~'))//判断是不是非法字符!
	{
		I2C_OLED_ShowChar(x,y,*chr,size1,mode);
		if(size1==8)x+=6;
		else x+=size1/2;
		chr++;
  }
}

//m^n
uint32_t I2C_OLED_Pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;
	while(n--)
	{
	  result*=m;
	}
	return result;
}

//显示数字
//x,y :起点坐标
//num :要显示的数字
//len :数字的位数
//size:字体大小
//mode:0,反色显示;1,正常显示
void I2C_OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size1,uint8_t mode)
{
	uint8_t t,temp,m=0;
	if(size1==8)m=2;
	for(t=0;t<len;t++)
	{
		temp=(num/I2C_OLED_Pow(10,len-t-1))%10;
			if(temp==0)
			{
				I2C_OLED_ShowChar(x+(size1/2+m)*t,y,'0',size1,mode);
      }
			else 
			{
			  I2C_OLED_ShowChar(x+(size1/2+m)*t,y,temp+'0',size1,mode);
			}
  }
}

//x,y：起点坐标
//sizex,sizey,图片长宽
//BMP[]：要写入的图片数组
//mode:0,反色显示;1,正常显示
void I2C_OLED_ShowPicture(uint8_t x,uint8_t y,uint8_t sizex,uint8_t sizey,uint8_t BMP[],uint8_t mode)
{
	uint16_t j=0;
	uint8_t i,n,temp,m;
	uint8_t x0=x,y0=y;
	sizey=sizey/8+((sizey%8)?1:0);
	for(n=0;n<sizey;n++)
	{
		 for(i=0;i<sizex;i++)
		 {
				temp=BMP[j];
				j++;
				for(m=0;m<8;m++)
				{
					if(temp&0x01)I2C_OLED_DrawPoint(x,y,mode);
					else I2C_OLED_DrawPoint(x,y,!mode);
					temp>>=1;
					y++;
				}
				x++;
				if((x-x0)==sizex)
				{
					x=x0;
					y0=y0+8;
				}
				y=y0;
     }
	 }
}
//OLED的初始化
void I2C_OLED_Init(void)
{
	#ifdef SCREEN_I2C_USE_SOFTWARE
		SCREEN_I2C = I2C_soft_init(SCREEN_I2C_SOFTWARE_SCL_GPIO,SCREEN_I2C_SOFTWARE_SCL_PIN,
															 SCREEN_I2C_SOFTWARE_SDA_GPIO,SCREEN_I2C_SOFTWARE_SDA_PIN,
															 SCREEN_I2C_SOFTWARE_SPEED);
	#else
		I2C_hardware_init(SCREEN_I2C_HARDWARE_PERIPH,SCREEN_I2C_HARDWARE_SPEED);
	#endif

	I2C_OLED_WR_Byte(0xAE,I2C_OLED_CMD);//--turn off oled panel
	I2C_OLED_WR_Byte(0x00,I2C_OLED_CMD);//---set low column address
	I2C_OLED_WR_Byte(0x10,I2C_OLED_CMD);//---set high column address
	I2C_OLED_WR_Byte(0x40,I2C_OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	I2C_OLED_WR_Byte(0x81,I2C_OLED_CMD);//--set contrast control register
	I2C_OLED_WR_Byte(0xCF,I2C_OLED_CMD);// Set SEG Output Current Brightness
	I2C_OLED_WR_Byte(0xA1,I2C_OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	I2C_OLED_WR_Byte(0xC8,I2C_OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	I2C_OLED_WR_Byte(0xA6,I2C_OLED_CMD);//--set normal display
	I2C_OLED_WR_Byte(0xA8,I2C_OLED_CMD);//--set multiplex ratio(1 to 64)
	I2C_OLED_WR_Byte(0x3f,I2C_OLED_CMD);//--1/64 duty
	I2C_OLED_WR_Byte(0xD3,I2C_OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	I2C_OLED_WR_Byte(0x00,I2C_OLED_CMD);//-not offset
	I2C_OLED_WR_Byte(0xd5,I2C_OLED_CMD);//--set display clock divide ratio/oscillator frequency
	I2C_OLED_WR_Byte(0x80,I2C_OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	I2C_OLED_WR_Byte(0xD9,I2C_OLED_CMD);//--set pre-charge period
	I2C_OLED_WR_Byte(0xF1,I2C_OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	I2C_OLED_WR_Byte(0xDA,I2C_OLED_CMD);//--set com pins hardware configuration
	I2C_OLED_WR_Byte(0x12,I2C_OLED_CMD);
	I2C_OLED_WR_Byte(0xDB,I2C_OLED_CMD);//--set vcomh
	I2C_OLED_WR_Byte(0x40,I2C_OLED_CMD);//Set VCOM Deselect Level
	I2C_OLED_WR_Byte(0x20,I2C_OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	I2C_OLED_WR_Byte(0x02,I2C_OLED_CMD);//
	I2C_OLED_WR_Byte(0x8D,I2C_OLED_CMD);//--set Charge Pump enable/disable
	I2C_OLED_WR_Byte(0x14,I2C_OLED_CMD);//--set(0x10) disable
	I2C_OLED_WR_Byte(0xA4,I2C_OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	I2C_OLED_WR_Byte(0xA6,I2C_OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
	I2C_OLED_Clear();
	I2C_OLED_WR_Byte(0xAF,I2C_OLED_CMD);
}

