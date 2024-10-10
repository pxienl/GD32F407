#include "main.h"
#include "systick.h"
#include "DMA.h"
#include "USART.h"
#include "GPIO.h"
#include "EXTI.h"

void sleep_mode();
void deepsleep_mode();
void standby_mode();

void RX0_recv(uint8_t *rxbuffer,uint32_t len){
  if(rxbuffer[0] == 1){
    sleep_mode();
  }else if(rxbuffer[0] == 2){
    deepsleep_mode();
  }else if(rxbuffer[0] == 3){
    standby_mode();
  }
}

void delay(){
    volatile uint32_t x = 20000000;
    // volatile uint32_t y = UINT32_MAX;
    // volatile uint32_t z = UINT32_MAX;
    // while(z--){
    //   y = UINT32_MAX;
    //   while(y--){
    //     x = UINT32_MAX;
        while(x--);
    //   }
    // }
}

void sleep_mode() {
  rcu_periph_clock_enable(RCU_PMU);
  printf("Sleep begin");
  pmu_to_sleepmode(WFI_CMD);
  printf("Sleep over");
}

void deepsleep_mode() {
  rcu_periph_clock_enable(RCU_PMU);
  printf("deep sleep begin");
  pmu_to_deepsleepmode(PMU_LDO_LOWPOWER, PMU_LOWDRIVER_ENABLE, WFI_CMD);
  printf("deep sleep over");
  SystemInit();
}

void standby_mode() {
  rcu_periph_clock_enable(RCU_PMU);
  pmu_flag_clear(PMU_FLAG_RESET_STANDBY);

  // enbale wakeup button
  pmu_wakeup_pin_enable();


  printf("standby begin");
  pmu_to_standbymode();
  printf("standby over");
}

void EXTI0_IRQHandler_RUN(){
  printf("EXTI0 interrupt handler\n");
}

int main(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    // systick_config();
    usart_init(USART0,GPIOA,GPIO_AF_7,GPIO_PIN_9 | GPIO_PIN_10);
    // usart_dma_tx_init(USART0,DMA1,DMA_CH7);
    // usart_dma_rx_init(USART0,DMA1,DMA_CH5);

    GPIO_output_init(GPIOB,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_PIN_2);


    EXTI_init(GPIOC,GPIO_PUPD_PULLUP,EXTI_0,1,1,EXTI_TRIG_BOTH);
    // GPIO_input_init(GPIOC,GPIO_PUPD_PULLUP,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    printf("init\n");
		while(1) 
		{
      gpio_bit_set(GPIOB,GPIO_PIN_2);
      delay();
      
      gpio_bit_reset(GPIOB,GPIO_PIN_2);
      delay();
		}
}
