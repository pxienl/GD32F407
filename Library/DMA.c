#include "DMA.h"

void DMA_select(uint32_t dma_periph){
  if(dma_periph == DMA0) rcu_periph_clock_enable(RCU_DMA0);
  else if(dma_periph == DMA1) rcu_periph_clock_enable(RCU_DMA1);
}

/*!
    \brief    enable DMA interrupt
    \param[in]  dma_periph: DMAx(x=0,1)
      \arg        DMAx(x=0,1)
    \param[in]  channelx: specify which DMA channel
      \arg        DMA_CHx(x=0..7)
    \param[in]  source: specify which interrupt to enbale
                only one parameters can be selected which are shown as below:
      \arg        DMA_INT_SDE: single data mode exception interrupt enable
      \arg        DMA_INT_TAE: tranfer access error interrupt enable
      \arg        DMA_INT_HTF: half transfer finish interrupt enable
      \arg        DMA_INT_FTF: full transfer finish interrupt enable
      \arg        DMA_INT_FEE: FIFO exception interrupt enable
    \param[in]  nvic_irq_pre_priority: the pre-emption priority needed to set
    \param[in]  nvic_irq_sub_priority: the subpriority needed to set
    \retval     none
*/
static void DMA_interrupt_enable(uint32_t dma_periph, dma_channel_enum channelx, uint32_t source, 
                                      uint8_t nvic_irq_pre_priority, uint8_t nvic_irq_sub_priority){
      dma_interrupt_enable(dma_periph,channelx,source);
      if(dma_periph == DMA0){
        if(channelx == DMA_CH0)      nvic_irq_enable(DMA0_Channel0_IRQn,nvic_irq_pre_priority,nvic_irq_sub_priority);
        else if(channelx == DMA_CH1) nvic_irq_enable(DMA0_Channel1_IRQn,nvic_irq_pre_priority,nvic_irq_sub_priority);
        else if(channelx == DMA_CH2) nvic_irq_enable(DMA0_Channel2_IRQn,nvic_irq_pre_priority,nvic_irq_sub_priority);
        else if(channelx == DMA_CH3) nvic_irq_enable(DMA0_Channel3_IRQn,nvic_irq_pre_priority,nvic_irq_sub_priority);
        else if(channelx == DMA_CH4) nvic_irq_enable(DMA0_Channel4_IRQn,nvic_irq_pre_priority,nvic_irq_sub_priority);
        else if(channelx == DMA_CH5) nvic_irq_enable(DMA0_Channel5_IRQn,nvic_irq_pre_priority,nvic_irq_sub_priority);
        else if(channelx == DMA_CH6) nvic_irq_enable(DMA0_Channel6_IRQn,nvic_irq_pre_priority,nvic_irq_sub_priority);
        else if(channelx == DMA_CH7) nvic_irq_enable(DMA0_Channel7_IRQn,nvic_irq_pre_priority,nvic_irq_sub_priority);
      }
      else if(dma_periph == DMA1){
        if(channelx == DMA_CH0)      nvic_irq_enable(DMA1_Channel0_IRQn,nvic_irq_pre_priority,nvic_irq_sub_priority);
        else if(channelx == DMA_CH1) nvic_irq_enable(DMA1_Channel1_IRQn,nvic_irq_pre_priority,nvic_irq_sub_priority);
        else if(channelx == DMA_CH2) nvic_irq_enable(DMA1_Channel2_IRQn,nvic_irq_pre_priority,nvic_irq_sub_priority);
        else if(channelx == DMA_CH3) nvic_irq_enable(DMA1_Channel3_IRQn,nvic_irq_pre_priority,nvic_irq_sub_priority);
        else if(channelx == DMA_CH4) nvic_irq_enable(DMA1_Channel4_IRQn,nvic_irq_pre_priority,nvic_irq_sub_priority);
        else if(channelx == DMA_CH5) nvic_irq_enable(DMA1_Channel5_IRQn,nvic_irq_pre_priority,nvic_irq_sub_priority);
        else if(channelx == DMA_CH6) nvic_irq_enable(DMA1_Channel6_IRQn,nvic_irq_pre_priority,nvic_irq_sub_priority);
        else if(channelx == DMA_CH7) nvic_irq_enable(DMA1_Channel7_IRQn,nvic_irq_pre_priority,nvic_irq_sub_priority);
      }
}

/*!
    \brief    dma single data mode configuration
    \param[in]  dma_periph: DMAx(x=0,1)
    \param[in]  channelx: DMA channel
    \param[in]  source: data source
    \param[in]  destination: data destination
    \param[in]  width: data width
      \arg        DMA_MEMORY_WIDTH_8BIT
      \arg        DMA_MEMORY_WIDTH_16BIT
      \arg        DMA_MEMORY_WIDTH_32BIT
    \param[in]  number: transfrom data number
    \retval     none
*/
static void dma_single_data_mode_config(uint32_t dma_periph, dma_channel_enum channelx,
                uint32_t source, uint32_t destination, 
                uint32_t width, uint32_t number){
    dma_single_data_parameter_struct dma_single_data_param;
    // dma_single_data_para_struct_init(&dma_single_data_param);

    /* Configure DMA channel */
    dma_single_data_param.direction = DMA_MEMORY_TO_MEMORY;

    /* source address */
    dma_single_data_param.periph_addr = source;
    /* source address auto increase */
    dma_single_data_param.periph_inc = DMA_PERIPH_INCREASE_ENABLE;
    /* source data width */
    dma_single_data_param.periph_memory_width = width;

    /* transfrom data number */
    dma_single_data_param.number = number;

    /* destination address */
    dma_single_data_param.memory0_addr = destination;
    /* destination address auto increase */
    dma_single_data_param.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    
    /* circular mode */
    dma_single_data_param.circular_mode = DMA_CIRCULAR_MODE_DISABLE;

    /* priority */
    dma_single_data_param.priority = DMA_PRIORITY_LOW;

    dma_single_data_mode_init(dma_periph,channelx,&dma_single_data_param);
}

void DMA_m2m_8bit(dma_channel_enum channelx,
                uint32_t source, uint32_t destination, uint32_t number,bool enableInterrupt) {
    DMA_select(DMA1);
    dma_deinit(DMA1,channelx);
    dma_single_data_mode_config(DMA1,channelx,source, destination,DMA_PERIPH_WIDTH_8BIT, number);
    if(enableInterrupt) DMA_interrupt_enable(DMA1,channelx,DMA_INT_FTF,2,2);
    dma_channel_enable(DMA1,channelx);
}

void DMA_m2m_16bit(dma_channel_enum channelx,
                uint32_t source, uint32_t destination, uint32_t number,bool enableInterrupt) {
    DMA_select(DMA1);
    dma_deinit(DMA1,channelx);
    dma_single_data_mode_config(DMA1,channelx,source, destination,DMA_PERIPH_WIDTH_16BIT, number);
    if(enableInterrupt) DMA_interrupt_enable(DMA1,channelx,DMA_INT_FTF,2,2);
    dma_channel_enable(DMA1,channelx);
}

void DMA_m2m_32bit(dma_channel_enum channelx,
                uint32_t source, uint32_t destination, uint32_t number,bool enableInterrupt) {
    DMA_select(DMA1);
    dma_deinit(DMA1,channelx);
    dma_single_data_mode_config(DMA1,channelx,source, destination,DMA_PERIPH_WIDTH_32BIT, number);
    if(enableInterrupt) DMA_interrupt_enable(DMA1,channelx,DMA_INT_FTF,2,2);
    dma_channel_enable(DMA1,channelx);
}
