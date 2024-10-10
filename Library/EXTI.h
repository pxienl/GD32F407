#ifndef __EXTI_H__
#define __EXTI_H__

#include "gd32f4xx.h"

#define EXTI_0_RUN
// #define EXTI_1_RUN
// #define EXTI_2_RUN
// #define EXTI_3_RUN
// #define EXTI_4_RUN
// #define EXTI_5_RUN
// #define EXTI_6_RUN
// #define EXTI_7_RUN
// #define EXTI_8_RUN
// #define EXTI_9_RUN
// #define EXTI_10_RUN
// #define EXTI_11_RUN
// #define EXTI_12_RUN
// #define EXTI_13_RUN
// #define EXTI_14_RUN
// #define EXTI_15_RUN

void EXTI_init(uint32_t GPIOx, uint32_t pull_up_down, exti_line_enum EXTI_x, uint8_t PrePriority, uint8_t SubPriority, exti_trig_type_enum EXTI_TRIG);
void EXTI_soft_init(exti_line_enum EXTI_x, uint8_t PrePriority, uint8_t SubPriority, exti_trig_type_enum EXTI_TRIG);

#ifdef EXTI_0_RUN
extern void EXTI0_IRQHandler_RUN();
#endif

#ifdef EXTI_1_RUN
extern void EXTI1_IRQHandler_RUN();
#endif

#ifdef EXTI_2_RUN
extern void EXTI2_IRQHandler_RUN();
#endif

#ifdef EXTI_3_RUN
extern void EXTI3_IRQHandler_RUN();
#endif

#ifdef EXTI_4_RUN
extern void EXTI4_IRQHandler_RUN();
#endif
#ifdef EXTI_5_RUN
extern void EXTI5_IRQHandler_RUN();
#endif
#ifdef EXTI_6_RUN
extern void EXTI6_IRQHandler_RUN();
#endif
#ifdef EXTI_7_RUN
extern void EXTI7_IRQHandler_RUN();
#endif
#ifdef EXTI_8_RUN
extern void EXTI8_IRQHandler_RUN();
#endif
#ifdef EXTI_9_RUN
extern void EXTI9_IRQHandler_RUN();
#endif
#ifdef EXTI_10_RUN
extern void EXTI10_IRQHandler_RUN();
#endif
#ifdef EXTI_11_RUN
extern void EXTI11_IRQHandler_RUN();
#endif
#ifdef EXTI_12_RUN
extern void EXTI12_IRQHandler_RUN();
#endif
#ifdef EXTI_13_RUN
extern void EXTI13_IRQHandler_RUN();
#endif
#ifdef EXTI_14_RUN
extern void EXTI14_IRQHandler_RUN();
#endif
#ifdef EXTI_15_RUN
extern void EXTI15_IRQHandler_RUN();
#endif

#endif