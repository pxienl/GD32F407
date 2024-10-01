#ifndef __WDGT_H__
#define __WDGT_H__

#include "gd32f4xx.h"

#define WWDGT_CLOCK 42000000

void FWDGT_init(uint16_t reload_value, uint8_t prescaler_div);

void FWDGT_feed();

void WWDGT_init(uint16_t counter, uint16_t window, uint32_t prescaler);

float WWDGT_window_start_ms();
float WWDGT_window_end_ms();

#endif