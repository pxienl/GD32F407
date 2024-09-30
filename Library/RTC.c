#include "RTC.h"

void (*RTC_alarm_handler)() = NULL;

rtc_parameter_struct rps;

rtc_parameter_struct RTC_init(){
     /* enable PMU clock */
    rcu_periph_clock_enable(RCU_PMU);
    /* enable the access of the RTC registers */
    pmu_backup_write_enable();

    /* reset backup domain */
    rcu_bkp_reset_enable();
    rcu_bkp_reset_disable();

    #if defined (RTC_CLOCK_SOURCE_IRC32K)
        rcu_osci_on(RCU_IRC32K);
        rcu_osci_stab_wait(RCU_IRC32K);
        rcu_rtc_clock_config(RCU_RTCSRC_IRC32K);
        rps.factor_asyn = 0x7F;
        rps.factor_syn  = 0x00F9;
    #elif defined (RTC_CLOCK_SOURCE_HXTAL)
        rcu_osci_on(RCU_HXTAL);
        rcu_osci_stab_wait(RCU_HXTAL);
        rcu_rtc_clock_config(RCU_RTCSRC_HXTAL_DIV_RTCDIV);
        rcu_rtc_div_config(RCU_RTC_HXTAL_DIV25);
        rps.factor_asyn = 0x7F;
        rps.factor_syn  = 0x09C3;
    #endif

    rcu_periph_clock_enable(RCU_RTC);
    rtc_register_sync_wait();

    return rps;
}

void RTC_factor_syn_set(rtc_parameter_struct* rps,uint16_t second){
  #if defined (RTC_CLOCK_SOURCE_IRC32K)
    rps->factor_syn = IRC32K_VALUE / ((rps->factor_asyn+1) * second) - 1;
  #elif defined (RTC_CLOCK_SOURCE_HXTAL)
    rps->factor_syn = (HXTAL_VALUE/((RCU_CFG0 & RCU_CFG0_RTCDIV) >> 16)) / ((rps->factor_asyn+1) / second) - 1;
  #endif
}

void RTC_alarm_init(rtc_alarm_struct* alarm,void (* handler)()){
  RTC_alarm_handler = handler;
  rtc_alarm_config(RTC_ALARM0,alarm);
  nvic_irq_enable(RTC_Alarm_IRQn,2,2);
  rtc_interrupt_enable(RTC_INT_ALARM0);
  rtc_flag_clear(RTC_FLAG_ALRM0);
  rtc_alarm_enable(RTC_ALARM0);

  exti_interrupt_flag_clear(RTC_ALARM_EXTI);
  exti_init(RTC_ALARM_EXTI,EXTI_INTERRUPT,EXTI_TRIG_RISING);
  exti_interrupt_enable(RTC_ALARM_EXTI);
}

void RTC_Alarm_IRQHandler(){
    if(exti_interrupt_flag_get(RTC_ALARM_EXTI) == SET){
        exti_interrupt_flag_clear(RTC_ALARM_EXTI);
        rtc_flag_clear(RTC_FLAG_ALRM0);
        RTC_alarm_handler();
    }
}