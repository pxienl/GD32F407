#ifndef __ADC_H__
#define __ADC_H__

#include "gd32f4xx.h"

void ADC_single_init(uint32_t adc_periph, uint8_t adc_channel);
uint16_t ADC_get(uint32_t adc_periph, uint8_t adc_channel);

void ADC_multiple_dma_init(uint32_t adc_periph, uint8_t adc_channels[],uint8_t len, uint16_t data[],uint32_t dma_periph,dma_channel_enum dma_channelx,dma_subperipheral_enum sub_periph);

#endif