#include "SPI.h"
#include "systick.h"
#include "GPIO.h"

void SPI_sw_struct_init(SPI_sw_struct* spi){
  
  GPIO_output_init(spi->CS_GPIO,GPIO_PUPD_PULLUP,GPIO_OTYPE_OD,spi->CS_PIN);
  GPIO_output_init(spi->SCLK_GPIO,GPIO_PUPD_PULLUP,GPIO_OTYPE_OD,spi->SCLK_PIN);
  GPIO_output_init(spi->MOSI_GPIO,GPIO_PUPD_PULLUP,GPIO_OTYPE_OD,spi->MOSI_PIN);
  GPIO_input_init(spi->MISO_GPIO,GPIO_PUPD_PULLUP,spi->MISO_PIN);

  gpio_bit_set(spi->CS_GPIO,spi->CS_PIN);
  gpio_bit_set(spi->MOSI_GPIO,spi->MOSI_PIN);
  gpio_bit_write(spi->SCLK_GPIO,spi->SCLK_PIN,spi->CPOL);

  
}

void SPI_sw_start(SPI_sw_struct* spi){
  gpio_bit_reset(spi->CS_GPIO,spi->CS_PIN);
  gpio_bit_write(spi->SCLK_GPIO,spi->SCLK_PIN,spi->CPOL);
}

void SPI_sw_stop(SPI_sw_struct* spi){
  gpio_bit_write(spi->SCLK_GPIO,spi->SCLK_PIN,spi->CPOL);
  gpio_bit_set(spi->CS_GPIO,spi->CS_PIN);
}

uint8_t SPI_sw_transform(SPI_sw_struct* spi,uint8_t data){
  uint8_t recv = 0;
  for (uint8_t i = 0; i < 8; i++)
  {
    gpio_bit_write(spi->SCLK_GPIO,spi->SCLK_PIN,0);
    gpio_bit_write(spi->MOSI_GPIO,spi->MOSI_PIN, data&0x80);
    data <<= 1;
    delay_1us(spi->Freq);

    gpio_bit_write(spi->SCLK_GPIO,spi->SCLK_PIN,1);
    recv <<= 1;
    recv |= !!gpio_input_bit_get(spi->MISO_GPIO,spi->MISO_PIN);
    delay_1us(spi->Freq);
  }
  return recv;
}

uint8_t SPI_sw_read(SPI_sw_struct* spi){
  uint8_t i,read=0;
  for(i = 0; i < 8; i++)
  {
      gpio_bit_write(spi->SCLK_GPIO,spi->SCLK_PIN,1);
      delay_1us(spi->Freq);
      read<<=1;
      if(gpio_input_bit_get(spi->MISO_GPIO,spi->MISO_PIN))
      {
          read++;
      }
      gpio_bit_write(spi->SCLK_GPIO,spi->SCLK_PIN,0);
  }
  return read;
}

void SPI_hw_select(uint32_t spi_periph){  
       if(spi_periph == SPI0) rcu_periph_clock_enable(RCU_SPI0);
  else if(spi_periph == SPI1) rcu_periph_clock_enable(RCU_SPI1);
  else if(spi_periph == SPI2) rcu_periph_clock_enable(RCU_SPI2);
  else if(spi_periph == SPI3) rcu_periph_clock_enable(RCU_SPI3);
  else if(spi_periph == SPI4) rcu_periph_clock_enable(RCU_SPI4);
  else if(spi_periph == SPI5) rcu_periph_clock_enable(RCU_SPI5);
}

void SPI_hw_init(uint32_t spi_periph){
  spi_parameter_struct spi_struct;
  spi_struct.device_mode          = SPI_MASTER;
  spi_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
  spi_struct.frame_size           = SPI_FRAMESIZE_8BIT;
  spi_struct.nss                  = SPI_NSS_SOFT;
  spi_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
  spi_struct.prescale             = SPI_PSC_2;
  spi_struct.endian               = SPI_ENDIAN_MSB;

  SPI_hw_select(spi_periph);

  spi_init(spi_periph,&spi_struct);
  spi_enable(spi_periph);
}

uint8_t SPI_hw_transform(uint32_t spi_periph, uint8_t data){
  while(RESET == spi_i2s_flag_get(spi_periph, SPI_FLAG_TBE));
  spi_i2s_data_transmit(spi_periph, data);
  while(RESET == spi_i2s_flag_get(spi_periph, SPI_FLAG_RBNE));
  return spi_i2s_data_receive(spi_periph);
}
