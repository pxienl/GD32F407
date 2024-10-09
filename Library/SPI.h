#ifndef __SPI_H__
#define __SPI_H__

#include "gd32f4xx.h"

typedef struct {
  uint32_t SCLK_GPIO;
  uint32_t SCLK_PIN;
  uint32_t MOSI_GPIO;
  uint32_t MOSI_PIN;
  uint32_t MISO_GPIO;
  uint32_t MISO_PIN;
  uint32_t CS_GPIO;
  uint32_t CS_PIN;

  uint8_t CPOL;
  uint8_t CPHA;
  uint8_t BitFirst;

  uint32_t Freq;
} SPI_sw_struct;

void SPI_sw_struct_init(SPI_sw_struct* spi);

void SPI_sw_start(SPI_sw_struct* spi);
void SPI_sw_stop(SPI_sw_struct* spi);
uint8_t SPI_sw_transform(SPI_sw_struct* spi,uint8_t data);
uint8_t SPI_sw_read(SPI_sw_struct* spi);

void SPI_hw_select(uint32_t spi_periph);
void SPI_hw_init(uint32_t spi_periph);
uint8_t SPI_hw_transform(uint32_t spi_periph, uint8_t data);

#endif