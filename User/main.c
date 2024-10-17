#include "main.h"
#include "GPIO.h"
#include "USART.h"
#include "servo.h"
#include "systick.h"
#include "vl53l0x.h"

#define MAX_ANGLE 150
#define MIN_ANGLE 90

static uint16_t angle = 130;
static int target = 150;
static float current = 0.0f;

static uint32_t delay = 1;

static float Kp = 0.1f;
static float Ki = 0.001f;
static float Kd = 0.02f;

float current_error = 0.0f;
float prev_error = 0.0f;
float error_sum = 0.0f;

void set_angle(uint16_t *angle) {
  if (*angle > 150)
    *angle = 150;
  else if (*angle < 90)
    *angle = 90;
}

uint16_t kpt;
uint16_t kit;
uint16_t kdt;

void RX0_recv(uint8_t *rxbuffer, uint32_t len) {
  if (len == 9) {
    kpt = (rxbuffer[0] << 8) + rxbuffer[1];
    Kp = (float)kpt / 1000.0f;
    kit = (rxbuffer[2] << 8) + rxbuffer[3];
    Ki = (float)kit / 1000.0f;
    kdt = (rxbuffer[4] << 8) + rxbuffer[5];
    Kd = (float)kdt / 1000.0f;

    target = 0;
    target += (rxbuffer[6] << 8) + rxbuffer[7];
    delay = rxbuffer[8];
  }
}

int main(void) {
  nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
  systick_config();
  usart_init(USART0, GPIOA, GPIO_AF_7, GPIO_PIN_9 | GPIO_PIN_10);
  usart_dma_tx_init(USART0,DMA1,DMA_CH7);
  usart_dma_rx_init(USART0,DMA1,DMA_CH5);

  printf("init\n");
  // I2C GPIO init
  GPIO_af_init(GPIOB, GPIO_AF_4, GPIO_PUPD_PULLUP, GPIO_PIN_6 | GPIO_PIN_7);
  vl53l0x_Init();

  servo_init();

  float i=0;
  while (1) {

    current = (float)vl53l0x_get();

    prev_error = current_error;
    current_error = (float)target - current;
    error_sum += current_error;

    if(error_sum > 10000) error_sum = 10000;
    else if(error_sum < -10000) error_sum = -10000;

    i = Kp * current_error + Ki * error_sum + Kd * (prev_error - current_error);

    angle = 130 + i;
    set_angle(&angle);
    servo_update(angle);

    printf("%f,%d,%f,%f,%f,%f,%d,%f,%d\n",current,angle,error_sum,Kp,Ki,Kd,target,i,delay);

    delay_1ms(delay);
  }
}
