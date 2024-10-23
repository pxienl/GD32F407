#include "SPI.h"
#include "GPIO.h"
#include "systick.h"
#include "DMA.h"

void SPI_sw_struct_init(SPI_sw_struct *spi)
{
    GPIO_output_init(spi->CS_GPIO, GPIO_PUPD_NONE, GPIO_OTYPE_PP, spi->CS_PIN);
    GPIO_output_init(spi->SCLK_GPIO, GPIO_PUPD_NONE, GPIO_OTYPE_PP, spi->SCLK_PIN);
    GPIO_output_init(spi->MOSI_GPIO, GPIO_PUPD_NONE, GPIO_OTYPE_PP, spi->MOSI_PIN);
    GPIO_input_init(spi->MISO_GPIO, GPIO_PUPD_PULLUP, spi->MISO_PIN);

    gpio_bit_set(spi->CS_GPIO, spi->CS_PIN);
    gpio_bit_set(spi->MOSI_GPIO, spi->MOSI_PIN);
    gpio_bit_write(spi->SCLK_GPIO, spi->SCLK_PIN, spi->CPOL);
}

void SPI_sw_start(SPI_sw_struct *spi)
{
    gpio_bit_reset(spi->CS_GPIO, spi->CS_PIN);
    gpio_bit_write(spi->SCLK_GPIO, spi->SCLK_PIN, spi->CPOL);
}

void SPI_sw_stop(SPI_sw_struct *spi)
{
    gpio_bit_write(spi->SCLK_GPIO, spi->SCLK_PIN, spi->CPOL);
    gpio_bit_set(spi->CS_GPIO, spi->CS_PIN);
}

uint8_t SPI_sw_transform(SPI_sw_struct *spi, uint8_t data)
{
    uint8_t recv = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        gpio_bit_write(spi->SCLK_GPIO, spi->SCLK_PIN, spi->CPOL);
        gpio_bit_write(spi->MOSI_GPIO, spi->MOSI_PIN, (data >> (7 - i)) & 0x01);
        delay_1us(spi->Freq);

        gpio_bit_write(spi->SCLK_GPIO, spi->SCLK_PIN, !spi->CPOL);
        recv <<= 1;
        recv |= !!gpio_input_bit_get(spi->MISO_GPIO, spi->MISO_PIN);
        delay_1us(spi->Freq);
    }

    gpio_bit_write(spi->SCLK_GPIO, spi->SCLK_PIN, spi->CPOL);
    return recv;
}

void SPI_hw_select(uint32_t spi_periph)
{
    if (spi_periph == SPI0)
        rcu_periph_clock_enable(RCU_SPI0);
    else if (spi_periph == SPI1)
        rcu_periph_clock_enable(RCU_SPI1);
    else if (spi_periph == SPI2)
        rcu_periph_clock_enable(RCU_SPI2);
    else if (spi_periph == SPI3)
        rcu_periph_clock_enable(RCU_SPI3);
    else if (spi_periph == SPI4)
        rcu_periph_clock_enable(RCU_SPI4);
    else if (spi_periph == SPI5)
        rcu_periph_clock_enable(RCU_SPI5);
}

void SPI_hw_init(uint32_t spi_periph)
{
    spi_parameter_struct spi_struct;
    spi_struct.device_mode          = SPI_MASTER;
    spi_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_struct.nss                  = SPI_NSS_SOFT;
    spi_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_struct.prescale             = SPI_PSC_2;
    spi_struct.endian               = SPI_ENDIAN_MSB;

    SPI_hw_select(spi_periph);

    spi_init(spi_periph, &spi_struct);
    spi_enable(spi_periph);
}

uint8_t SPI_hw_transform(uint32_t spi_periph, uint8_t data)
{
    while (RESET == spi_i2s_flag_get(spi_periph, SPI_FLAG_TBE))
        ;
    spi_i2s_data_transmit(spi_periph, data);
    while (RESET == spi_i2s_flag_get(spi_periph, SPI_FLAG_RBNE))
        ;
    return spi_i2s_data_receive(spi_periph);
}

static void SPI_dma_perien_select(uint32_t spi_periph,dma_channel_enum channelx){
         if (spi_periph == SPI0) dma_channel_subperipheral_select(DMA1,channelx,DMA_SUBPERI3);
    else if (spi_periph == SPI1) dma_channel_subperipheral_select(DMA0,channelx,DMA_SUBPERI0);
    else if (spi_periph == SPI2) dma_channel_subperipheral_select(DMA0,channelx,DMA_SUBPERI0);
    else if (spi_periph == SPI3 && (channelx == DMA_CH0 || channelx == DMA_CH1)) dma_channel_subperipheral_select(DMA1,channelx,DMA_SUBPERI4);
    else if (spi_periph == SPI3 && (channelx == DMA_CH3 || channelx == DMA_CH4)) dma_channel_subperipheral_select(DMA1,channelx,DMA_SUBPERI5);
    else if (spi_periph == SPI4 && (channelx == DMA_CH3 || channelx == DMA_CH4)) dma_channel_subperipheral_select(DMA1,channelx,DMA_SUBPERI2);
    else if (spi_periph == SPI4 && (channelx == DMA_CH5 || channelx == DMA_CH6)) dma_channel_subperipheral_select(DMA1,channelx,DMA_SUBPERI7);
    else if (spi_periph == SPI5 ) dma_channel_subperipheral_select(DMA1,channelx,DMA_SUBPERI1);
}

void SPI_dma_transmit_init(uint32_t spi_periph, uint32_t dma_periph, dma_channel_enum channelx)
{
    
    DMA_select(dma_periph);
    dma_deinit(dma_periph, channelx);
    dma_single_data_parameter_struct dma_param;
    dma_single_data_para_struct_init(&dma_param);
    dma_param.periph_addr         = (uint32_t)&SPI_DATA(spi_periph);
    dma_param.direction           = DMA_MEMORY_TO_PERIPH;
    dma_param.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_param.priority            = DMA_PRIORITY_LOW;
    dma_param.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
    dma_param.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    dma_param.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;
    dma_single_data_mode_init(dma_periph, channelx, &dma_param);
    SPI_dma_perien_select(spi_periph, channelx);
}

void SPI_dma_receive_init(uint32_t spi_periph, uint32_t dma_periph, dma_channel_enum channelx)
{
    spi_dma_enable(spi_periph,SPI_DMA_RECEIVE);
    DMA_select(dma_periph);
    dma_single_data_parameter_struct dma_param;
    dma_single_data_para_struct_init(&dma_param);
    dma_deinit(dma_periph, channelx);
    dma_param.periph_addr = (uint32_t)&SPI_DATA(spi_periph);
    dma_param.direction           = DMA_PERIPH_TO_MEMORY;
    dma_param.priority            = DMA_PRIORITY_LOW;
    dma_param.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_param.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
    dma_param.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    dma_param.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;

    dma_single_data_mode_init(dma_periph, channelx, &dma_param);
    SPI_dma_perien_select(spi_periph, channelx);

}

void SPI_dma_send(uint32_t spi_periph,uint32_t dma_periph, dma_channel_enum channelx,uint8_t * data,uint32_t len){
    dma_memory_address_config(dma_periph,channelx,DMA_MEMORY_0,(uint32_t)data);
    dma_transfer_number_config(dma_periph,channelx,len);
    dma_channel_enable(dma_periph,channelx);
    spi_dma_enable(spi_periph,SPI_DMA_TRANSMIT);
    while(RESET == dma_flag_get(dma_periph, channelx, DMA_FLAG_FTF));
    dma_flag_clear(dma_periph, channelx, DMA_FLAG_FTF);
}
// uint8_t SPI_dma_recv(uint32_t spi_periph,uint32_t dma_periph, dma_channel_enum channelx){
//     uint8_t data;
//     dma_memory_address_config(dma_periph,channelx,DMA_MEMORY_0,(uint32_t)&data);
//     dma_transfer_number_config(dma_periph,channelx,1);
//     dma_channel_enable(dma_periph,channelx);
//     spi_dma_enable(spi_periph,SPI_DMA_RECEIVE);
//     while(RESET == dma_flag_get(dma_periph, channelx, DMA_FLAG_FTF));
//     dma_flag_clear(dma_periph, channelx, DMA_FLAG_FTF);
//     return data;
// }

uint8_t SPI_dma_transform(uint32_t spi_periph,uint32_t dma_periph, dma_channel_enum txchannelx,dma_channel_enum rxchannelx,uint8_t data){
    uint8_t recv = 0;
    dma_memory_address_config(dma_periph,rxchannelx,DMA_MEMORY_0,(uint32_t)&recv);
    dma_transfer_number_config(dma_periph,rxchannelx,1);
    dma_channel_enable(dma_periph,rxchannelx);
    spi_dma_enable(spi_periph,SPI_DMA_RECEIVE);
    SPI_dma_send(spi_periph,dma_periph,txchannelx,&data,1);
    while(RESET == dma_flag_get(dma_periph, rxchannelx, DMA_FLAG_FTF));
    dma_flag_clear(dma_periph, rxchannelx, DMA_FLAG_FTF);
    return recv;
}