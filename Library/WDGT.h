#ifndef __WDGT_H__
#define __WDGT_H__

#include "gd32f4xx.h"

void FWDGT_init(uint16_t reload_value, uint8_t prescaler_div);

void FWDGT_feed();

#endif