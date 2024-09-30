#include "main.h"
#include "systick.h"
#include "DMA.h"
#include "USART.h"
#include "RTC.h"

void RX0_recv(uint8_t *rxbuffer,uint32_t len){
    usart_dma_send(USART0,rxbuffer,len);
}

void alarm_handler(){
    printf("alarm_handler\n");
}


int main(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    systick_config();
    usart_init(USART0,GPIOA,GPIO_AF_7,GPIO_PIN_9 | GPIO_PIN_10);
    usart_dma_tx_init(USART0,DMA1,DMA_CH7);
    usart_dma_rx_init(USART0,DMA1,DMA_CH5);

    rtc_parameter_struct rps = RTC_init();
    rtc_deinit();
    rps.year           = 0x24;
    rps.month          = 0x12;
    rps.date           = 0x31;
    rps.day_of_week    = RTC_TUESDAY;
    rps.hour           = 0x23;
    rps.minute         = 0x59;
    rps.second         = 0x56;
    rps.factor_asyn    = 0x7F;
    RTC_factor_syn_set(&rps,1);
    printf("factor_syn: %x\n",rps.factor_syn);
    rps.am_pm          = RTC_PM;
    rps.display_format = RTC_24HOUR;
    rtc_init(&rps);

    rtc_alarm_struct alarm;

    alarm.alarm_mask = RTC_ALARM_DATE_MASK | RTC_ALARM_HOUR_MASK | RTC_ALARM_MINUTE_MASK;
    alarm.weekday_or_date = RTC_ALARM_DATE_SELECTED;
    alarm.alarm_day = 1;
    alarm.alarm_hour = 0;
    alarm.alarm_minute = 0;
    alarm.alarm_second = 5;
    alarm.am_pm = RTC_AM;
    RTC_alarm_init(&alarm,alarm_handler);

    while(1) {
        rtc_parameter_struct rps;
        rtc_current_time_get(&rps);

        printf("Current Time: %x-%x-%x %x:%x:%x", rps.year, rps.month, rps.date, rps.hour, rps.minute, rps.second);
        delay_1ms(1000);
    }
}
