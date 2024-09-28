#ifndef __DMA_H__
#define __DMA_H__

#include "gd32f4xx.h"
#include "stdbool.h"

void DMA_select(uint32_t dma_periph);

void DMA_m2m_8bit(dma_channel_enum channelx,
                uint32_t source, uint32_t destination, uint32_t number,bool enableInterrupt);

void DMA_m2m_16bit(dma_channel_enum channelx,
                uint32_t source, uint32_t destination, uint32_t number,bool enableInterrupt);

void DMA_m2m_32bit(dma_channel_enum channelx,
                uint32_t source, uint32_t destination, uint32_t number,bool enableInterrupt);


#endif