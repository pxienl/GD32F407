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
