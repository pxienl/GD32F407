#include "EXTI.h"
#include "stdio.h"
#include "GPIO.h"

static void EXTI_config(exti_line_enum EXTI_x, exti_trig_type_enum EXTI_TRIG,
                        uint8_t EXTI_SOURCE_GPIOx, uint8_t EXTI_SOURCE_PINx,
                        IRQn_Type EXTIx_IRQn, uint8_t PrePriority,
                        uint8_t SubPriority) {
  rcu_periph_clock_enable(RCU_SYSCFG);
  syscfg_exti_line_config(EXTI_SOURCE_GPIOx, EXTI_SOURCE_PINx);
  exti_init(EXTI_x, EXTI_INTERRUPT, EXTI_TRIG);
  nvic_irq_enable(EXTIx_IRQn, PrePriority, SubPriority);
  exti_interrupt_enable(EXTI_x);
  exti_interrupt_flag_clear(EXTI_x);
}

static void EXTI_select(uint32_t GPIOx, exti_line_enum EXTI_x,uint32_t* Pin, 
                        uint8_t* EXTI_SOURCE_GPIOx,uint8_t* EXTI_SOURCE_PINx,
                        IRQn_Type* EXTIx_IRQn) {

  if (GPIOx == GPIOA)      *EXTI_SOURCE_GPIOx = EXTI_SOURCE_GPIOA;
  else if (GPIOx == GPIOB) *EXTI_SOURCE_GPIOx = EXTI_SOURCE_GPIOB;
  else if (GPIOx == GPIOC) *EXTI_SOURCE_GPIOx = EXTI_SOURCE_GPIOC;
  else if (GPIOx == GPIOD) *EXTI_SOURCE_GPIOx = EXTI_SOURCE_GPIOD;
  else if (GPIOx == GPIOE) *EXTI_SOURCE_GPIOx = EXTI_SOURCE_GPIOE;
  else if (GPIOx == GPIOF) *EXTI_SOURCE_GPIOx = EXTI_SOURCE_GPIOF;
  else if (GPIOx == GPIOG) *EXTI_SOURCE_GPIOx = EXTI_SOURCE_GPIOG;
  else if (GPIOx == GPIOH) *EXTI_SOURCE_GPIOx = EXTI_SOURCE_GPIOH;
  else if (GPIOx == GPIOI) *EXTI_SOURCE_GPIOx = EXTI_SOURCE_GPIOI;

  if (EXTI_x == EXTI_0)       *EXTI_SOURCE_PINx = EXTI_SOURCE_PIN0,  *Pin = GPIO_PIN_0,  *EXTIx_IRQn = EXTI0_IRQn;
  else if (EXTI_x == EXTI_1)  *EXTI_SOURCE_PINx = EXTI_SOURCE_PIN1,  *Pin = GPIO_PIN_1,  *EXTIx_IRQn = EXTI1_IRQn;
  else if (EXTI_x == EXTI_2)  *EXTI_SOURCE_PINx = EXTI_SOURCE_PIN2,  *Pin = GPIO_PIN_2,  *EXTIx_IRQn = EXTI2_IRQn;
  else if (EXTI_x == EXTI_3)  *EXTI_SOURCE_PINx = EXTI_SOURCE_PIN3,  *Pin = GPIO_PIN_3,  *EXTIx_IRQn = EXTI3_IRQn;
  else if (EXTI_x == EXTI_4)  *EXTI_SOURCE_PINx = EXTI_SOURCE_PIN4,  *Pin = GPIO_PIN_4,  *EXTIx_IRQn = EXTI4_IRQn;
  else if (EXTI_x == EXTI_5)  *EXTI_SOURCE_PINx = EXTI_SOURCE_PIN5,  *Pin = GPIO_PIN_5,  *EXTIx_IRQn = EXTI5_9_IRQn;
  else if (EXTI_x == EXTI_6)  *EXTI_SOURCE_PINx = EXTI_SOURCE_PIN6,  *Pin = GPIO_PIN_6,  *EXTIx_IRQn = EXTI5_9_IRQn;
  else if (EXTI_x == EXTI_7)  *EXTI_SOURCE_PINx = EXTI_SOURCE_PIN7,  *Pin = GPIO_PIN_7,  *EXTIx_IRQn = EXTI5_9_IRQn;
  else if (EXTI_x == EXTI_8)  *EXTI_SOURCE_PINx = EXTI_SOURCE_PIN8,  *Pin = GPIO_PIN_8,  *EXTIx_IRQn = EXTI5_9_IRQn;
  else if (EXTI_x == EXTI_9)  *EXTI_SOURCE_PINx = EXTI_SOURCE_PIN9,  *Pin = GPIO_PIN_9,  *EXTIx_IRQn = EXTI5_9_IRQn;
  else if (EXTI_x == EXTI_10) *EXTI_SOURCE_PINx = EXTI_SOURCE_PIN10, *Pin = GPIO_PIN_10, *EXTIx_IRQn = EXTI10_15_IRQn;
  else if (EXTI_x == EXTI_11) *EXTI_SOURCE_PINx = EXTI_SOURCE_PIN11, *Pin = GPIO_PIN_11, *EXTIx_IRQn = EXTI10_15_IRQn;
  else if (EXTI_x == EXTI_12) *EXTI_SOURCE_PINx = EXTI_SOURCE_PIN12, *Pin = GPIO_PIN_12, *EXTIx_IRQn = EXTI10_15_IRQn;
  else if (EXTI_x == EXTI_13) *EXTI_SOURCE_PINx = EXTI_SOURCE_PIN13, *Pin = GPIO_PIN_13, *EXTIx_IRQn = EXTI10_15_IRQn;
  else if (EXTI_x == EXTI_14) *EXTI_SOURCE_PINx = EXTI_SOURCE_PIN14, *Pin = GPIO_PIN_14, *EXTIx_IRQn = EXTI10_15_IRQn;
  else if (EXTI_x == EXTI_15) *EXTI_SOURCE_PINx = EXTI_SOURCE_PIN15, *Pin = GPIO_PIN_15, *EXTIx_IRQn = EXTI10_15_IRQn;
}

void EXTI_init(uint32_t GPIOx, exti_line_enum EXTI_x,
               uint8_t PrePriority, uint8_t SubPriority,
               exti_trig_type_enum EXTI_TRIG) {
  uint32_t Pin;
  uint8_t EXTI_SOURCE_GPIOx, EXTI_SOURCE_PINx;
  IRQn_Type EXTIx_IRQn;

  EXTI_select(GPIOx, EXTI_x,&Pin,&EXTI_SOURCE_GPIOx,&EXTI_SOURCE_PINx,&EXTIx_IRQn);
  GPIO_input_init(GPIOx,GPIO_PUPD_NONE,Pin);
  EXTI_config(EXTI_x,EXTI_TRIG,EXTI_SOURCE_GPIOx,EXTI_SOURCE_PINx,EXTIx_IRQn,PrePriority,SubPriority);
}
void EXTI_soft_init(exti_line_enum EXTI_x,
               uint8_t PrePriority, uint8_t SubPriority,
               exti_trig_type_enum EXTI_TRIG) {
  uint32_t Pin;
  uint8_t EXTI_SOURCE_GPIOx, EXTI_SOURCE_PINx;
  IRQn_Type EXTIx_IRQn;

  EXTI_select(0, EXTI_x, &Pin, &EXTI_SOURCE_GPIOx, &EXTI_SOURCE_PINx, &EXTIx_IRQn);
  
  rcu_periph_clock_enable(RCU_SYSCFG);
  exti_init(EXTI_x, EXTI_INTERRUPT, EXTI_TRIG);
  exti_interrupt_enable(EXTI_x);
  nvic_irq_enable(EXTIx_IRQn, PrePriority, SubPriority);
  exti_interrupt_flag_clear(EXTI_x);
}


#ifdef EXTI_0_RUN 
void EXTI0_IRQHandler(){
  //判断标记并且清理标记
	if(exti_interrupt_flag_get(EXTI_0) == SET){
		exti_interrupt_flag_clear(EXTI_0);
        EXTI0_IRQHandler_RUN();
	}
}
#endif

#ifdef EXTI_1_RUN
void EXTI1_IRQHandler(){
  //判断标记并且清理标记
	if(exti_interrupt_flag_get(EXTI_1) == SET){
		exti_interrupt_flag_clear(EXTI_1);
        EXTI1_IRQHandler_RUN();
	}
}
#endif

#ifdef EXTI_2_RUN
void EXTI2_IRQHandler(){
  //判断标记并且清理标记
	if(exti_interrupt_flag_get(EXTI_2) == SET){
		exti_interrupt_flag_clear(EXTI_2);
        EXTI2_IRQHandler_RUN();
	}
}
#endif

#ifdef EXTI_3_RUN
void EXTI3_IRQHandler(){
  //判断标记并且清理标记
	if(exti_interrupt_flag_get(EXTI_3) == SET){
		exti_interrupt_flag_clear(EXTI_3);
        EXTI3_IRQHandler_RUN();
	}
}
#endif

#ifdef EXTI_4_RUN
void EXTI4_IRQHandler(){
  //判断标记并且清理标记
	if(exti_interrupt_flag_get(EXTI_4) == SET){
		exti_interrupt_flag_clear(EXTI_4);
        EXTI4_IRQHandler_RUN();
	}
}
#endif

#if defined(EXTI_5_RUN) || defined(EXTI_6_RUN) || defined(EXTI_7_RUN) || defined(EXTI_8_RUN) || defined(EXTI_9_RUN)
void EXTI5_9_IRQHandler(){
  #ifdef EXTI_5_RUN
  //判断标记并且清理标记
	if(exti_interrupt_flag_get(EXTI_5) == SET){
		exti_interrupt_flag_clear(EXTI_5);
        EXTI5_IRQHandler_RUN();
	}
  #endif
  #ifdef EXTI_6_RUN
  //判断标记并且清理标记
	if(exti_interrupt_flag_get(EXTI_6) == SET){
		exti_interrupt_flag_clear(EXTI_6);
        EXTI6_IRQHandler_RUN();
	}
  #endif
  #ifdef EXTI_7_RUN
    //判断标记并且清理标记
	if(exti_interrupt_flag_get(EXTI_7) == SET){
		exti_interrupt_flag_clear(EXTI_7);
        EXTI7_IRQHandler_RUN();
	}
  #endif
  #ifdef EXTI_8_RUN
    //判断标记并且清理标记
	if(exti_interrupt_flag_get(EXTI_8) == SET){
		exti_interrupt_flag_clear(EXTI_8);
        EXTI8_IRQHandler_RUN();
	}
  #endif
  #ifdef EXTI_9_RUN
    //判断标记并且清理标记
	if(exti_interrupt_flag_get(EXTI_9) == SET){
		exti_interrupt_flag_clear(EXTI_9);
        EXTI9_IRQHandler_RUN();
	}
  #endif
}
#endif



#if defined(EXTI_10_RUN) || defined(EXTI_11_RUN) || defined(EXTI_12_RUN) || defined(EXTI_13_RUN) || defined(EXTI_14_RUN) || defined(EXTI_15_RUN)
void EXTI10_15_IRQHandler(){
  #ifdef EXTI_10_RUN
  //判断标记并且清理标记
	if(exti_interrupt_flag_get(EXTI_10) == SET){
		exti_interrupt_flag_clear(EXTI_10);
        EXTI10_IRQHandler_RUN();
	}
  #endif
  #ifdef EXTI_11_RUN
  //判断标记并且清理标记
	if(exti_interrupt_flag_get(EXTI_11) == SET){
		exti_interrupt_flag_clear(EXTI_11);
        EXTI11_IRQHandler_RUN();
	}
  #endif
  #ifdef EXTI_12_RUN
    //判断标记并且清理标记
	if(exti_interrupt_flag_get(EXTI_12) == SET){
		exti_interrupt_flag_clear(EXTI_12);
        EXTI12_IRQHandler_RUN();
	}
  #endif
  #ifdef EXTI_13_RUN
    //判断标记并且清理标记
	if(exti_interrupt_flag_get(EXTI_13) == SET){
		exti_interrupt_flag_clear(EXTI_13);
        EXTI13_IRQHandler_RUN();
	}
  #endif
  #ifdef EXTI_14_RUN
    //判断标记并且清理标记
	if(exti_interrupt_flag_get(EXTI_14) == SET){
		exti_interrupt_flag_clear(EXTI_14);
        EXTI14_IRQHandler_RUN();
	}
  #endif
  #ifdef EXTI_15_RUN
    //判断标记并且清理标记
	if(exti_interrupt_flag_get(EXTI_15) == SET){
		exti_interrupt_flag_clear(EXTI_15);
        EXTI15_IRQHandler_RUN();
	}
  #endif
}
#endif

