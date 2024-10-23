/*!
    \file    diskio.c
    \brief   Low level disk interface driver

    \version 2021-05-20, V1.0.0, firmware for GD32 USBFS&USBHS
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

/* Includes ------------------------------------------------------------------*/
#include "diskio.h"
#include "ff_gen_drv.h"
#include "sdcard.h"
#include <stdint.h>
#include <stdio.h>

#if defined ( __GNUC__ )
#ifndef __weak
#define __weak __attribute__((weak))
#endif
#endif

extern Disk_drvTypeDef  disk;
extern sd_card_info_struct sd_cardinfo; 
/*!
    \brief      Gets Disk Status
    \param[in]  pdrv: Physical drive number (0..)
    \param[out] none
    \retval     DSTATUS: Operation status
*/
DSTATUS disk_status (
    BYTE pdrv       /* Physical drive number to identify the drive */
)
{
    // DSTATUS stat;

    // stat = disk.drv[pdrv]->disk_status(disk.lun[pdrv]);

    return RES_OK;
}

/*!
    \brief      Initializes a Drive
    \param[in]  pdrv: Physical drive number (0..)
    \param[out] none
    \retval     DSTATUS: Operation status
*/
DSTATUS disk_initialize (
    BYTE pdrv       /* Physical drive nmuber to identify the drive */
)
{
   uint8_t res = sd_init();
   if(res == SD_OK) res=0;
   
  printf("disk_initialize:%d\r\n", res);
  
   if(res) return STA_NOINIT;
   else return 0;
    
    /*DSTATUS stat = RES_OK;

    if (disk.is_initialized[pdrv] == 0) {
        disk.is_initialized[pdrv] = 1;
        stat = disk.drv[pdrv]->disk_initialize(disk.lun[pdrv]);
    }

    return stat;*/
}

/*!
    \brief      Reads Sector(s)
    \param[in]  pdrv: Physical drive number (0..)
    \param[in]  buff: Data buffer to store read data
    \param[in]  sector: Sector address (LBA)
    \param[in]  count: Number of sectors to write (1..128)
    \param[out] none
    \retval     DRESULT: Operation result
*/
DRESULT disk_read (
    BYTE pdrv,      /* Physical drive nmuber to identify the drive */
    BYTE *buff,     /* Data buffer to store read data */
    DWORD sector,   /* Sector address in LBA */
    UINT count      /* Number of sectors to read */
)
{
    uint8_t res = 0;
    
    if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误
    res=sd_block_read((uint32_t*)buff,sector<<9,count);	 
            //printf("sd_block_read sector: %d  count: %d \r\n",sector,count);
    if(res == SD_OK) res=0;
		while(res )//读出错
		{
        printf("sd rd error:%d  sector: %d  count: %d \r\n",res,sector,count);
				sd_init();	//重新初始化SD卡
				res=sd_block_read((uint32_t*)buff,sector<<9,count);	
        if(res == SD_OK) res=0;
        else return RES_ERROR;	
		}
    
    //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if(res==0)return RES_OK;	 
    else return RES_ERROR;	
  
    /*DRESULT res;

    res = disk.drv[pdrv]->disk_read(disk.lun[pdrv], buff, sector, count);

    return res;*/
}

/*!
    \brief      Writes Sector(s)
    \param[in]  pdrv: Physical drive number (0..)
    \param[in]  buff: Data to be written
    \param[in]  sector: Sector address (LBA)
    \param[in]  count: Number of sectors to write (1..128)
    \param[out] none
    \retval     DRESULT: Operation result
*/
#if _USE_WRITE == 1
DRESULT disk_write (
    BYTE pdrv,          /* Physical drive nmuber to identify the drive */
    const BYTE *buff,   /* Data to be written */
    DWORD sector,       /* Sector address in LBA */
    UINT count          /* Number of sectors to write */
)
{   
    uint8_t res=0;  
    if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		 	 
    
    res=sd_block_write((uint32_t*)buff,sector<<9,count);
    if(res == SD_OK) res=0;
    while(res )//写出错
    {
      printf("sd wr error:%d\r\n",res);
      sd_init();	//重新初始化SD卡
      res=sd_block_write((uint32_t*)buff,sector<<9,count);	
      if(res == SD_OK) res=0;
      else return RES_ERROR;
    }
    
    //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if(res == 0x00)return RES_OK;	 
    else return RES_ERROR;
    /*
    DRESULT res;

    res = disk.drv[pdrv]->disk_write(disk.lun[pdrv], buff, sector, count);

    return res;
    */
}
#endif /* _USE_WRITE == 1 */

/*!
    \brief      I/O control operation
    \param[in]  pdrv: Physical drive number (0..)
    \param[in]  cmd: Control code
    \param[in]  buff: Buffer to send/receive control data
    \param[out] none
    \retval     DRESULT: Operation result
*/
#if _USE_IOCTL == 1
DRESULT disk_ioctl (
    BYTE pdrv,      /* Physical drive nmuber (0..) */
    BYTE cmd,       /* Control code */
    void *buff      /* Buffer to send/receive control data */
)
{   
    DRESULT res;
    
    switch(cmd)
    {
      case CTRL_SYNC:
      res = RES_OK; 
          break;	 
      case GET_SECTOR_SIZE:
      *(DWORD*)buff = 512; 
          res = RES_OK;
          break;	 
      case GET_BLOCK_SIZE:
      *(WORD*)buff = sd_cardinfo.card_blocksize;
          res = RES_OK;
          break;	 
      case GET_SECTOR_COUNT:
          *(DWORD*)buff = sd_cardinfo.card_blocksize;
          res = RES_OK;
          break;
      default:
          res = RES_PARERR;
          break;
    }
    return res;
    /*
    DRESULT res;

    res = disk.drv[pdrv]->disk_ioctl(disk.lun[pdrv], cmd, buff);

    return res;*/
}
#endif /* _USE_IOCTL == 1 */

/*!
    \brief      Gets Time from RTC
    \param[in]  none
    \param[out] none
    \retval     Time in DWORD
*/
__weak DWORD get_fattime (void)
{
    return 0;
}




