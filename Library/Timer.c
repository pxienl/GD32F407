#include "Timer.h"

static void Timer_Select(uint32_t TIMERx){
    if(TIMERx == TIMER1)  rcu_periph_clock_enable(RCU_TIMER1);
    else if(TIMERx == TIMER2)  rcu_periph_clock_enable(RCU_TIMER2);
    else if(TIMERx == TIMER3)  rcu_periph_clock_enable(RCU_TIMER3);
    else if(TIMERx == TIMER4)  rcu_periph_clock_enable(RCU_TIMER4);
    else if(TIMERx == TIMER5)  rcu_periph_clock_enable(RCU_TIMER5);
    else if(TIMERx == TIMER6)  rcu_periph_clock_enable(RCU_TIMER6);
    else if(TIMERx == TIMER11) rcu_periph_clock_enable(RCU_TIMER11);
    else if(TIMERx == TIMER12) rcu_periph_clock_enable(RCU_TIMER12);
    else if(TIMERx == TIMER13) rcu_periph_clock_enable(RCU_TIMER13);
    else if(TIMERx == TIMER0)  rcu_periph_clock_enable(RCU_TIMER0);
    else if(TIMERx == TIMER7)  rcu_periph_clock_enable(RCU_TIMER7);
    else if(TIMERx == TIMER8)  rcu_periph_clock_enable(RCU_TIMER8);
    else if(TIMERx == TIMER9)  rcu_periph_clock_enable(RCU_TIMER9);
    else if(TIMERx == TIMER10) rcu_periph_clock_enable(RCU_TIMER10);
}

/*!
    \brief    initialize timer
    \param[in]  TIMERx: TIMERx(x=0..13)
    \param[in]  prescaler: timer prescaler value
    \param[in]  period: timer period value, it need bigger than 2564(only TIMERx(x=0,7,8,9,10)) or 1282
    \param[in]  nvic_irq: the NVIC interrupt request
      \arg        TIMER0_BRK_TIMER8_IRQn: TIMER0 break and TIMER8 interrupts                       
      \arg        TIMER0_UP_TIMER9_IRQn: TIMER0 update and TIMER9 interrupts                      
      \arg        TIMER0_TRG_CMT_TIMER10_IRQn: TIMER0 trigger and commutation  and TIMER10 interrupts   
      \arg        TIMER0_Channel_IRQn: TIMER0 channel capture compare interrupt                 
      \arg        TIMER1_IRQn: TIMER1 interrupt                                         
      \arg        TIMER2_IRQn: TIMER2 interrupt                                         
      \arg        TIMER3_IRQn: TIMER3 interrupts                                        
      \arg        TIMER4_IRQn: TIMER4 interrupt                                         
      \arg        TIMER5_DAC_IRQn: TIMER5 and DAC0 DAC1 underrun error interrupts           
      \arg        TIMER6_IRQn: TIMER6 interrupt                                         
      \arg        TIMER7_BRK_TIMER11_IRQn: TIMER7 break and TIMER11 interrupts                      
      \arg        TIMER7_UP_TIMER12_IRQn: TIMER7 update and TIMER12 interrupts                     
      \arg        TIMER7_TRG_CMT_TIMER13_IRQn: TIMER7 trigger and commutation and TIMER13 interrupts    
      \arg        TIMER7_Channel_IRQn: TIMER7 channel capture compare interrupt
    \param[in]  nvic_irq_pre_priority: the pre-emption priority needed to set
    \param[in]  nvic_irq_sub_priority: the subpriority needed to set
    \param[in]  interrupt: timer interrupt enable source only one parameter can be selected which is shown as below:
      \arg        TIMER_INT_UP: update interrupt enable, TIMERx(x=0..13)
      \arg        TIMER_INT_CH0: channel 0 interrupt enable, TIMERx(x=0..4,7..13)
      \arg        TIMER_INT_CH1: channel 1 interrupt enable, TIMERx(x=0..4,7,8,11)
      \arg        TIMER_INT_CH2: channel 2 interrupt enable, TIMERx(x=0..4,7)
      \arg        TIMER_INT_CH3: channel 3 interrupt enable , TIMERx(x=0..4,7)
      \arg        TIMER_INT_CMT: commutation interrupt enable, TIMERx(x=0,7)
      \arg        TIMER_INT_TRG: trigger interrupt enable, TIMERx(x=0..4,7,8,11)
      \arg        TIMER_INT_BRK: break interrupt enable, TIMERx(x=0,7)
    \param[out] none
    \retval     none
*/
void Timer_init(uint32_t TIMERx, uint16_t prescaler,uint32_t period,
                    uint8_t nvic_irq,uint8_t nvic_irq_pre_priority,
                    uint8_t nvic_irq_sub_priority,uint32_t interrupt) {
  Timer_Select(TIMERx);
  
  timer_deinit(TIMERx);

  timer_parameter_struct tps;
  timer_struct_para_init(&tps);

  // rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);
  // Frequency multiplication factor
  uint8_t factor = 2;
  if( TIMERx == TIMER0 || TIMERx == TIMER7 || TIMERx == TIMER8 || TIMERx == TIMER9 || TIMERx == TIMER10) factor = 1;

  tps.prescaler = prescaler - 1;
  tps.period = SystemCoreClock / period / factor - 1;

  timer_init(TIMERx, &tps);
  nvic_irq_enable(nvic_irq, nvic_irq_pre_priority, nvic_irq_sub_priority);
  timer_interrupt_enable(TIMERx, interrupt);
  timer_enable(TIMERx);
}

/*!
    \brief    get timer period
    \param[in]  TIMERx: TIMERx(x=0..13)
    \retval     Timer period
*/
uint32_t Timer_get_period(uint32_t TIMERx){
  return TIMER_CAR(TIMERx);
}

/*!
    \brief    initialize general timer to output pwm Square Wave
    \param[in]  TIMERx: TIMERx(x=0..13)
    \param[in]  prescaler: timer prescaler value
    \param[in]  period: timer period value, it need bigger than 2564(only TIMERx(x=0,7,8,9,10)) or 1282
    \param[in]  TIMER_CH_x:
                only one parameter can be selected which is shown as below:
      \arg        TIMER_CH_0: TIMER channel 0(TIMERx(x=0..4,7..13))
      \arg        TIMER_CH_1: TIMER channel 1(TIMERx(x=0..4,7,8,11))
      \arg        TIMER_CH_2: TIMER channel 2(TIMERx(x=0..4,7))
      \arg        TIMER_CH_3: TIMER channel 3(TIMERx(x=0..4,7))
    \param[in]  TIMER_OC_MODE: channel output compare mode
                only one parameter can be selected which is shown as below:
      \arg        TIMER_OC_MODE_TIMING: timing mode
      \arg        TIMER_OC_MODE_ACTIVE: active mode
      \arg        TIMER_OC_MODE_INACTIVE: inactive mode
      \arg        TIMER_OC_MODE_TOGGLE: toggle mode
      \arg        TIMER_OC_MODE_LOW: force low mode
      \arg        TIMER_OC_MODE_HIGH: force high mode
      \arg        TIMER_OC_MODE_PWM0: PWM0 mode
      \arg        TIMER_OC_MODE_PWM1: PWM1 mode
    \param[in]  pulse: channel output pulse value
    \param[out] none
    \retval     none
*/
void PWM_init(uint32_t TIMERx,uint16_t prescaler, uint32_t period,
                  uint16_t TIMER_CH_x, uint16_t TIMER_OC_MODE,uint16_t pulse){
  Timer_Select(TIMERx);

  timer_deinit(TIMERx);
  timer_parameter_struct tps;
  timer_struct_para_init(&tps);

  uint8_t factor = 2;
  if( TIMERx == TIMER0 || TIMERx == TIMER7 || TIMERx == TIMER8 || TIMERx == TIMER9 || TIMERx == TIMER10) factor = 1;

  tps.prescaler = prescaler - 1;
  tps.period = SystemCoreClock / period / factor - 1;

  timer_init(TIMERx, &tps);

  timer_oc_parameter_struct ocpara;

  ocpara.outputstate = (uint16_t)TIMER_CCX_ENABLE;
  ocpara.ocpolarity = TIMER_OC_POLARITY_HIGH;
  ocpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;

  // channel complementary output
  ocpara.outputnstate = TIMER_CCXN_DISABLE;
  ocpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
  ocpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

  timer_channel_output_config(TIMERx,TIMER_CH_x,&ocpara);
  timer_channel_output_mode_config(TIMERx,TIMER_CH_x,TIMER_OC_MODE);
  if(pulse > tps.period) pulse = tps.period;
  timer_channel_output_pulse_value_config(TIMERx,TIMER_CH_x,pulse);

  timer_enable(TIMERx);

}

/*!
    \brief      update pwm Square Wave Output pulse configuration
    \param[in]  TIMERx: please refer to the following parameters
    \param[in]  TIMER_CH_x:
                only one parameter can be selected which is shown as below:
      \arg        TIMER_CH_0: TIMER channel0(TIMERx(x=0..4,7..13))
      \arg        TIMER_CH_1: TIMER channel1(TIMERx(x=0..4,7,8,11))
      \arg        TIMER_CH_2: TIMER channel2(TIMERx(x=0..4,7))
      \arg        TIMER_CH_3: TIMER channel3(TIMERx(x=0..4,7))
    \param[in]  pulse: pluse value between 0 and Timer_get_period(TIMERx)
    \param[out] none
    \retval     none
*/
void PWM_update(uint32_t TIMERx, uint16_t TIMER_CH_x, uint16_t pulse){
  uint16_t period = (uint16_t)Timer_get_period(TIMERx);
  if(pulse > period) pulse = period;
  timer_channel_output_pulse_value_config(TIMERx, TIMER_CH_x, pulse);
}

/*!
    \brief      open TIMER channel complementary output
    \param[in]  TIMERx: please refer to the following parameters
    \param[in]  TIMER_CH_x:
                only one parameter can be selected which is shown as below:
      \arg        TIMER_CH_0: TIMER channel0(TIMERx(x=0..4,7..13))
      \arg        TIMER_CH_1: TIMER channel1(TIMERx(x=0..4,7,8,11))
      \arg        TIMER_CH_2: TIMER channel2(TIMERx(x=0..4,7))
      \arg        TIMER_CH_3: TIMER channel3(TIMERx(x=0..4,7))
    \param[out] none
    \retval     none
*/
void PWM_state_start(uint32_t TIMERx, uint16_t TIMER_CH_x){
  timer_channel_output_state_config(TIMERx,TIMER_CH_x, TIMER_CCX_ENABLE);
}

/*!
    \brief      close TIMER channel output
    \param[in]  TIMERx: please refer to the following parameters
    \param[in]  TIMER_CH_x:
                only one parameter can be selected which is shown as below:
      \arg        TIMER_CH_0: TIMER channel0(TIMERx(x=0..4,7..13))
      \arg        TIMER_CH_1: TIMER channel1(TIMERx(x=0..4,7,8,11))
      \arg        TIMER_CH_2: TIMER channel2(TIMERx(x=0..4,7))
      \arg        TIMER_CH_3: TIMER channel3(TIMERx(x=0..4,7))
    \param[out] none
    \retval     none
*/
void PWM_state_stop(uint32_t TIMERx, uint16_t TIMER_CH_x){
  timer_channel_output_state_config(TIMERx,TIMER_CH_x, TIMER_CCX_DISABLE);
}

/*!
    \brief      open TIMER channel complementary output
    \param[in]  TIMERx: please refer to the following parameters
    \param[in]  TIMER_CH_x:
                only one parameter can be selected which is shown as below:
      \arg        TIMER_CH_0: TIMER channel0(TIMERx(x=0..4,7..13))
      \arg        TIMER_CH_1: TIMER channel1(TIMERx(x=0..4,7,8,11))
      \arg        TIMER_CH_2: TIMER channel2(TIMERx(x=0..4,7))
      \arg        TIMER_CH_3: TIMER channel3(TIMERx(x=0..4,7))
    \param[out] none
    \retval     none
*/
void PWM_state_complementary_start(uint32_t TIMERx, uint16_t TIMER_CH_x){
  timer_channel_complementary_output_state_config(TIMERx,TIMER_CH_x, TIMER_CCXN_ENABLE);
}

/*!
    \brief      close TIMER channel complementary output
    \param[in]  TIMERx: please refer to the following parameters
    \param[in]  TIMER_CH_x:
                only one parameter can be selected which is shown as below:
      \arg        TIMER_CH_0: TIMER channel0(TIMERx(x=0..4,7..13))
      \arg        TIMER_CH_1: TIMER channel1(TIMERx(x=0..4,7,8,11))
      \arg        TIMER_CH_2: TIMER channel2(TIMERx(x=0..4,7))
      \arg        TIMER_CH_3: TIMER channel3(TIMERx(x=0..4,7))
    \param[out] none
    \retval     none
*/
void PWM_state_complementary_stop(uint32_t TIMERx, uint16_t TIMER_CH_x){
  timer_channel_complementary_output_state_config(TIMERx,TIMER_CH_x, TIMER_CCXN_DISABLE);
}

/*!
    \brief    initialize advanced timer to output pwm Square Wave
    \param[in]  TIMERx: TIMERx(x=0..13)
    \param[in]  prescaler: timer prescaler value
    \param[in]  period: timer period value, it need bigger than 2564(only TIMERx(x=0,7,8,9,10)) or 1282
    \param[in]  TIMER_CH_x:
                only one parameter can be selected which is shown as below:
      \arg        TIMER_CH_0: TIMER channel 0(TIMERx(x=0..4,7..13))
      \arg        TIMER_CH_1: TIMER channel 1(TIMERx(x=0..4,7,8,11))
      \arg        TIMER_CH_2: TIMER channel 2(TIMERx(x=0..4,7))
      \arg        TIMER_CH_3: TIMER channel 3(TIMERx(x=0..4,7))
    \param[in]  TIMER_OC_MODE: channel output compare mode
                only one parameter can be selected which is shown as below:
      \arg        TIMER_OC_MODE_TIMING: timing mode
      \arg        TIMER_OC_MODE_ACTIVE: active mode
      \arg        TIMER_OC_MODE_INACTIVE: inactive mode
      \arg        TIMER_OC_MODE_TOGGLE: toggle mode
      \arg        TIMER_OC_MODE_LOW: force low mode
      \arg        TIMER_OC_MODE_HIGH: force high mode
      \arg        TIMER_OC_MODE_PWM0: PWM0 mode
      \arg        TIMER_OC_MODE_PWM1: PWM1 mode
    \param[in]  pulse: channel output pulse value
    \param[out] none
    \retval     none
*/
void PWM_advanced_init(uint32_t TIMERx,uint16_t prescaler, uint32_t period,
                  uint16_t TIMER_CH_x, uint16_t TIMER_OC_MODE,uint16_t pulse){
  Timer_Select(TIMERx);

  timer_deinit(TIMERx);
  timer_parameter_struct tps;
  timer_struct_para_init(&tps);

  uint8_t factor = 2;
  if( TIMERx == TIMER0 || TIMERx == TIMER7 || TIMERx == TIMER8 || TIMERx == TIMER9 || TIMERx == TIMER10) factor = 1;

  tps.prescaler = prescaler - 1;
  tps.period = SystemCoreClock / period / factor - 1;

  timer_init(TIMERx, &tps);

  timer_oc_parameter_struct ocpara;

  ocpara.outputstate = (uint16_t)TIMER_CCX_ENABLE;
  ocpara.ocpolarity = TIMER_OC_POLARITY_HIGH;
  ocpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;

  // channel complementary output
  ocpara.outputnstate = TIMER_CCXN_ENABLE;
  ocpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
  ocpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

  timer_channel_output_config(TIMERx,TIMER_CH_x,&ocpara);

  timer_break_parameter_struct breakpara;

  breakpara.runoffstate     = TIMER_ROS_STATE_DISABLE;
  breakpara.ideloffstate    = TIMER_IOS_STATE_DISABLE;
  breakpara.deadtime        = 0U;
  breakpara.breakpolarity   = TIMER_BREAK_POLARITY_HIGH;
  breakpara.outputautostate = TIMER_OUTAUTO_ENABLE;
  breakpara.protectmode     = TIMER_CCHP_PROT_OFF;
  breakpara.breakstate      = TIMER_BREAK_ENABLE;

  timer_break_config(TIMERx, &breakpara);
  timer_break_enable(TIMERx);

  timer_channel_output_mode_config(TIMERx,TIMER_CH_x,TIMER_OC_MODE);
  if(pulse > tps.period) pulse = tps.period;
  timer_channel_output_pulse_value_config(TIMERx,TIMER_CH_x,pulse);

  timer_enable(TIMERx);
}