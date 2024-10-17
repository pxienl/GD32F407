#include "vl53l0x.h"
#include "bsp_VL53L0X.h"
#include "vl53l0x_platform.h"

static VL53L0X_Dev_t vl53l0x_dev; // 设备I2C数据参数

void vl53l0x_Init() {
  uint8_t mode = 0; // 0：默认;1:高精度;2:长距离;3:高速
  VL53L0X_Error Status = VL53L0X_ERROR_NONE; // 工作状态

  nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2); // 优先级分组
  printf("start\r\n");

  while (vl53l0x_init(&vl53l0x_dev)) // vl53l0x初始化
  {
    printf("VL53L0X Error!!!\n\r");
    delay_1ms(500);
  }
  printf("VL53L0X OK\r\n");

  while (vl53l0x_set_mode(&vl53l0x_dev, mode)) // 配置测量模式
  {
    printf("Mode Set Error\r\n");
    delay_1ms(500);
  }
}

uint16_t vl53l0x_get() {
  return VL53L0X_PerformSingleRangingMeasurement(&vl53l0x_dev, &vl53l0x_data) ==
                 VL53L0X_ERROR_NONE
             ? vl53l0x_data.RangeMilliMeter
             : 0;
}