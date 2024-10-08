#include "ADC.h"
#include "GPIO.h"
#include "stdio.h"
#include "DMA.h"

void ADC_select(uint32_t adc_periph){
  if(adc_periph == ADC0) rcu_periph_clock_enable(RCU_ADC0);
  else if(adc_periph == ADC1) rcu_periph_clock_enable(RCU_ADC1);
  else if(adc_periph == ADC2) rcu_periph_clock_enable(RCU_ADC2);
}

/*!
    \brief        configure ADC routine channel
    \param[in]    adc_periph: ADCx,x=0,1,2
    \param[in]    adc_channel: the selected ADC channel
                  only one parameter can be selected which is shown as below:
      \arg          ADC_CHANNEL_x(x=0..18): ADC channelx
    \param[in]    sample_time: the sample time value
                  only one parameter can be selected which is shown as below:
      \arg          ADC_SAMPLETIME_3: 3 cycles
      \arg          ADC_SAMPLETIME_15: 15 cycles
      \arg          ADC_SAMPLETIME_28: 28 cycles
      \arg          ADC_SAMPLETIME_56: 56 cycles
      \arg          ADC_SAMPLETIME_84: 84 cycles
      \arg          ADC_SAMPLETIME_112: 112 cycles
      \arg          ADC_SAMPLETIME_144: 144 cycles
      \arg          ADC_SAMPLETIME_480: 480 cycles
    \param[out]   none
    \retval       none
*/
void ADC_single_init(uint32_t adc_periph, uint8_t adc_channel){
  ADC_select(adc_periph);

  adc_deinit();

  // 84M / 4 = 21MHz
  // adc max frequency is 40MHz. look at the user manual 14.4.2. ADC clock
  adc_clock_config(ADC_ADCCK_PCLK2_DIV4);

  adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);

  adc_resolution_config(adc_periph,ADC_RESOLUTION_12B);

  adc_data_alignment_config(adc_periph,ADC_DATAALIGN_RIGHT);

  if(adc_periph == ADC0){
    if(adc_channel == ADC_CHANNEL_16 || adc_channel == ADC_CHANNEL_17)
      adc_channel_16_to_18(ADC_TEMP_VREF_CHANNEL_SWITCH, ENABLE);
    else if(adc_channel == ADC_CHANNEL_18)
      adc_channel_16_to_18(ADC_VBAT_CHANNEL_SWITCH, ENABLE);
  }

  // adc_routine_channel_config(adc_periph,0,adc_channel,ADC_SAMPLETIME_15);

  // adc_channel_length_config(adc_periph,ADC_ROUTINE_CHANNEL,1);


  adc_inserted_channel_config(adc_periph,0,adc_channel,ADC_SAMPLETIME_15);

  adc_special_function_config(adc_periph,ADC_SCAN_MODE,DISABLE);
  adc_special_function_config(adc_periph,ADC_CONTINUOUS_MODE,DISABLE);

  adc_enable(adc_periph);

  // ADC calibration
  adc_calibration_enable(adc_periph);

}

uint16_t ADC_get(uint32_t adc_periph, uint8_t adc_channel) {
  // adc_routine_channel_config(adc_periph,0,adc_channel,ADC_SAMPLETIME_15);
  adc_channel_length_config(adc_periph,ADC_INSERTED_CHANNEL,1);

  adc_software_trigger_enable(adc_periph, ADC_INSERTED_CHANNEL);
  // adc_software_trigger_enable(adc_periph, ADC_ROUTINE_CHANNEL);

  while(RESET == adc_flag_get(adc_periph, ADC_FLAG_EOC));

  adc_flag_clear(adc_periph, ADC_FLAG_EOC);

  // vol = ( adc_value / 2^{resolution_bit} ) * {Reference Voltage} 
  // Reference Voltage = 3.3V
  // float vol = value * 3.3 / 4096;


  return adc_inserted_data_read(adc_periph,adc_channel);

  // return adc_routine_data_read(adc_periph);

}

void ADC_multiple_dma_init(uint32_t adc_periph, uint8_t adc_channels[],uint8_t len, uint16_t data[],uint32_t dma_periph,dma_channel_enum dma_channelx,dma_subperipheral_enum sub_periph){
  ADC_select(adc_periph);

  adc_deinit();

  // 84M / 4 = 21MHz
  // adc max frequency is 40MHz. look at the user manual 14.4.2. ADC clock
  adc_clock_config(ADC_ADCCK_PCLK2_DIV4);

  adc_resolution_config(adc_periph,ADC_RESOLUTION_12B);

  adc_data_alignment_config(adc_periph,ADC_DATAALIGN_RIGHT);

  for (uint8_t i = 0; i < len; i++)
  {
    /* code */
    if(adc_periph == ADC0){
      if(adc_channels[i] == ADC_CHANNEL_16 || adc_channels[i] == ADC_CHANNEL_17)
        adc_channel_16_to_18(ADC_TEMP_VREF_CHANNEL_SWITCH, ENABLE);
      else if(adc_channels[i] == ADC_CHANNEL_18)
        adc_channel_16_to_18(ADC_VBAT_CHANNEL_SWITCH, ENABLE);
    }
    adc_routine_channel_config(adc_periph,i,adc_channels[i],ADC_SAMPLETIME_15);
  }
  
  adc_channel_length_config(adc_periph,ADC_ROUTINE_CHANNEL,len);

  adc_special_function_config(adc_periph,ADC_SCAN_MODE,ENABLE);
  adc_special_function_config(adc_periph,ADC_CONTINUOUS_MODE,ENABLE);

  adc_dma_request_after_last_enable(adc_periph);
  adc_dma_mode_enable(adc_periph);

  adc_enable(adc_periph);

  // ADC calibration
  adc_calibration_enable(adc_periph);

  adc_software_trigger_enable(adc_periph,ADC_ROUTINE_CHANNEL);

  

// DMA

  DMA_select(dma_periph);
  dma_deinit(dma_periph,dma_channelx);
  dma_single_data_parameter_struct dma_single_data_param;
  /* Configure DMA channel */
  dma_single_data_param.direction = DMA_PERIPH_TO_MEMORY;
  /* source address */
  dma_single_data_param.periph_addr = (uint32_t)&ADC_RDATA(adc_periph);
  /* source address auto increase */
  dma_single_data_param.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
  /* source data width */
  dma_single_data_param.periph_memory_width = DMA_PERIPH_WIDTH_16BIT;

  /* transfrom data number */
  dma_single_data_param.number = len;

  /* destination address */
  dma_single_data_param.memory0_addr = (uint32_t)data;
  /* destination address auto increase */
  dma_single_data_param.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
  
  /* circular mode */
  dma_single_data_param.circular_mode = DMA_CIRCULAR_MODE_ENABLE;

  /* priority */
  dma_single_data_param.priority = DMA_PRIORITY_LOW;

  dma_single_data_mode_init(dma_periph,dma_channelx,&dma_single_data_param);

  dma_channel_subperipheral_select(dma_periph, dma_channelx, sub_periph);

  dma_flag_clear(dma_periph,dma_channelx,DMA_FLAG_FTF);

  dma_channel_enable(dma_periph,dma_channelx);
}
