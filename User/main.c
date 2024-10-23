#include "main.h"
#include "GPIO.h"
#include "USART.h"
#include "systick.h"
#include "SPI.h"
#include <string.h>
#include "ff.h"


FATFS fs;
uint8_t buff_tx[512]="SD卡扇区读写测试,fat文件系统读写测试\r\n";
uint8_t buff_rx[512];
void test_fatfs_write(const TCHAR *file_name,uint8_t *buff,uint32_t len);
void test_fatfs_read(const TCHAR *file_name,uint8_t *buff,uint32_t len);

void test_fatfs(){
  uint8_t stat=f_mount(&fs,"0:",1);//SD卡挂载
  printf("stat=%d\r\n",stat);
  if(stat==0) printf("磁盘挂载成功\r\n");
  test_fatfs_write("0:/test.txt",buff_tx,strlen((char *)buff_tx));
  test_fatfs_read("0:/test.txt",buff_rx,strlen((char *)buff_tx));
  printf("rx:%s\r\n",buff_rx);
  //while(1){}
}
void test_fatfs_write(const TCHAR *file_name,uint8_t *buff,uint32_t len)
{
	/*1.打开文件*/
	FIL fp;
	FRESULT res;
	UINT bw;
	res=f_open(&fp,file_name,FA_WRITE|FA_CREATE_ALWAYS);//写+创建
	if(res!=FR_OK)
	{
		printf("文件打开或创建失败:ERR%d\r\n",res);
		return ;
	}
	printf("文件创建或打开成功\r\n");
	res=f_write(&fp,buff,len,&bw);
	if(res==FR_OK)
	{
		printf("数据写入成功 %d Byte\r\n",bw);
	}
	else printf("写入失败ERR：%d\r\n",res);
	f_close(&fp);//关闭文件
}

void test_fatfs_read(const TCHAR *file_name,uint8_t *buff,uint32_t len)
{
		/*1.打开文件*/
	FIL fp;
	FRESULT res;
	UINT br;
	res=f_open(&fp,file_name,FA_READ);//读
	if(res!=FR_OK)
	{
		printf("文件打开失败:ERR%d\r\n",res);
		return ;
	}
	printf("文件打开成功\r\n");
	res=f_read(&fp,buff,len,&br);
	if(res!=FR_OK)
	{
		printf("读取数据失败ERR:%d \r\n",res);
	}
	else
	{
		printf("读取数据成功 %d byte\r\n",br);
	}
	f_close(&fp);//关闭文件
}

void RX0_recv(uint8_t *rxbuffer, uint32_t len) {
}

int main(void) {
  nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
  systick_config();
  usart_init(USART0, GPIOA, GPIO_AF_7, GPIO_PIN_9 | GPIO_PIN_10);
  usart_dma_tx_init(USART0,DMA1,DMA_CH7);
  usart_dma_rx_init(USART0,DMA1,DMA_CH5);

  printf("init\n");

  test_fatfs();
  

  while (1) {
    delay_1ms(1000);
  }
}
