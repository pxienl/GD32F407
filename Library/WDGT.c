#include "WDGT.h"

/*!
    \brief    initialize free watchdog
              confiure FWDGT counter clock: 32KHz(IRC32K) / prescaler_div = XX KHz 
    \param[in]  reload_value: specify reload value(0x0000 - 0x0FFF(4095))
    \param[in]  prescaler_div: FWDGT prescaler value
                only one parameter can be selected which is shown as below:
      \arg        FWDGT_PSC_DIV4: FWDGT prescaler set to 4
      \arg        FWDGT_PSC_DIV8: FWDGT prescaler set to 8
      \arg        FWDGT_PSC_DIV16: FWDGT prescaler set to 16
      \arg        FWDGT_PSC_DIV32: FWDGT prescaler set to 32
      \arg        FWDGT_PSC_DIV64: FWDGT prescaler set to 64
      \arg        FWDGT_PSC_DIV128: FWDGT prescaler set to 128
      \arg        FWDGT_PSC_DIV256: FWDGT prescaler set to 256
    \retval     none
*/
void FWDGT_init(uint16_t reload_value, uint8_t prescaler_div){
    rcu_osci_on(RCU_IRC32K);
    while (SUCCESS != rcu_osci_stab_wait(RCU_IRC32K));
    
    fwdgt_config(reload_value, prescaler_div);
    fwdgt_enable();
}

void FWDGT_feed(){
    fwdgt_counter_reload();
}

/*!
    \brief    initialize window watchdog  
    \param[in]  counter: 0x00 - 0x7F   
    \param[in]  window: 0x00 - 0x7F
                  if less than 0x3F, a reset will be triggered when start the window watchdog timer counter 
    \param[in]  prescaler: wwdgt prescaler value
                only one parameter can be selected which is shown as below:
      \arg        WWDGT_CFG_PSC_DIV1: the time base of window watchdog counter = (PCLK1/4096)/1
      \arg        WWDGT_CFG_PSC_DIV2: the time base of window watchdog counter = (PCLK1/4096)/2
      \arg        WWDGT_CFG_PSC_DIV4: the time base of window watchdog counter = (PCLK1/4096)/4
      \arg        WWDGT_CFG_PSC_DIV8: the time base of window watchdog counter = (PCLK1/4096)/8
    \param[out] none
    \retval     none
*/
void WWDGT_init(uint16_t counter, uint16_t window, uint32_t prescaler){
    rcu_periph_clock_enable(RCU_WWDGT);
    wwdgt_config(counter, window, prescaler);
    wwdgt_enable();
}

// frequency to period
float hz2ms(float freq){
    // 1s = 1000 ms
    return 1000 / freq; // = (1s / hz) * 1000ms
}

uint8_t pow2(uint8_t x){
    if(!x) return 1;
    uint8_t z = 2;
    while(--x) z*=2;
    return z;
}

float WWDGT_window_start_ms(){
    uint8_t counter = WWDGT_CTL & BITS(0,6);
    uint8_t window = WWDGT_CFG & BITS(0,6);
    uint8_t psc = (WWDGT_CFG & BITS(7,8)) >> 6;

    psc = pow2(psc);

    return hz2ms(WWDGT_CLOCK/4096/psc) * (counter - window);
}

float WWDGT_window_end_ms(){
    
    uint8_t counter = WWDGT_CTL & BITS(0,6);
    uint8_t psc = (WWDGT_CFG & BITS(7,8)) >> 6;

    psc = pow2(psc);

    return hz2ms(WWDGT_CLOCK/4096/psc) * (counter - 0x3F);
}