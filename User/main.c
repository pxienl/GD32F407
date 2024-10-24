#include "main.h"
#include "GPIO.h"
#include "USART.h"
#include "systick.h"
#include "SPI.h"
#include "st7789/st7789.h"
#include "cst816/cst816t.h"

#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"

// 事件回调
void event_handler(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_VALUE_CHANGED) {
    printf("toggled btn\n");
  }
}
void demo_button_checkable() {
  // 获取显示图层
  lv_obj_t* screen = lv_scr_act();
  // 创建按钮
  lv_obj_t* btn = lv_btn_create(screen);
  // 设置按钮尺寸
  lv_obj_set_size(btn, 120, 50);

  // 4. 在按钮上创建文本并居中显示
  lv_obj_t* label = lv_label_create(btn);
  lv_label_set_text(label, "Toggle");
  // lv_obj_center(label);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
  // 5. 设置按钮可选中
  lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);

  // 默认选中
//  lv_obj_add_state(btn, LV_STATE_CHECKED);

  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_VALUE_CHANGED, NULL);
  // 居中
  lv_obj_center(btn);
}


void RX0_recv(uint8_t *rxbuffer, uint32_t len) {
}

int main(void) {
  nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
  systick_config();
  usart_init(USART0, GPIOA, GPIO_AF_7, GPIO_PIN_9 | GPIO_PIN_10);
  usart_dma_tx_init(USART0,DMA1,DMA_CH7);
  usart_dma_rx_init(USART0,DMA1,DMA_CH5);

  printf("init\n");

// 1. 初始化LVGL
  lv_init();
  // 2.显示屏驱动初始化
  lv_port_disp_init();
  // 3. 触摸屏(输入设备要在屏幕初始化之后再init，否则会失效)
  lv_port_indev_init();

  demo_button_checkable();

  while (1) {
    lv_tick_inc(1);
    // 5. 执行定时任务（屏幕渲染，事件处理）
    lv_timer_handler();
    // 休眠1ms
    delay_1ms(1);
  }
}
