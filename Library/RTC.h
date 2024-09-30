#ifndef __RTC_H__
#define __RTC_H__

#include "gd32f4xx.h"

#define RTC_CLOCK_SOURCE_IRC32K
// #define RTC_CLOCK_SOURCE_HXTAL

#define RTC_ALARM_EXTI EXTI_17

#define RTC_WRITE_BCD(value) (((value/10) << 4) + (value%10))
#define RTC_READ_BCD(bcd) (((bcd >> 4) * 10) + (bcd & 0xF))

rtc_parameter_struct RTC_init();

void RTC_factor_syn_set(rtc_parameter_struct* rps,uint16_t second);

void RTC_alarm_init(rtc_alarm_struct* alarm,void (* handler)());

#endif