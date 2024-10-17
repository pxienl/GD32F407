#include "servo.h"
#include "GPIO.h"
#include "Timer.h"

#define SERVO_PERIOD 5000
#define SERVO_PRESCALER 100
#define SERVO_TIMER TIMER8
#define SERVO_TIMER_CHANNEL TIMER_CH_0

volatile static uint16_t pulse;

void servo_init(){
  GPIO_af_init(GPIOE,GPIO_AF_3,GPIO_PUPD_NONE,GPIO_PIN_5);
  PWM_init(SERVO_TIMER,SERVO_PRESCALER,SERVO_PERIOD,SERVO_TIMER_CHANNEL,TIMER_OC_MODE_PWM0,0);
  PWM_state_start(SERVO_TIMER,SERVO_TIMER_CHANNEL);
}

// angle 0~180
void servo_update(uint16_t angle){
  if(angle > 180) return;
  // pulse = angle * (period / 10 / 180) + 0.5ms_pulse
  pulse = (angle + 45) * Timer_get_period(SERVO_TIMER) /  1800;
  PWM_update(SERVO_TIMER,SERVO_TIMER_CHANNEL,pulse);
}