#include "main.h"
#include "FreeRTOS.h"
#include "GPIO.h"
#include "systick.h"
#include "task.h"

TaskHandle_t StartTask_Handler;
TaskHandle_t Task1_Handler;
TaskHandle_t Task2_Handler;

void task1(void *pvParameters) {
  while (1) {
    vTaskDelay(250);
    gpio_bit_set(GPIOD, GPIO_PIN_8);
    vTaskDelay(250);
    gpio_bit_reset(GPIOD, GPIO_PIN_8);
  }
}

void task2(void *pvParameters) {
  while (1) {
    vTaskDelay(1000);
    gpio_bit_set(GPIOD, GPIO_PIN_9);
    vTaskDelay(1000);
    gpio_bit_reset(GPIOD, GPIO_PIN_9);
  }
}

void start_task(void *pvParameters) {
  GPIO_output_init(GPIOD, GPIO_PUPD_NONE, GPIO_OTYPE_PP,
                   GPIO_PIN_8 | GPIO_PIN_9);
  GPIO_output_init(GPIOC, GPIO_PUPD_NONE, GPIO_OTYPE_PP, GPIO_PIN_6);

  taskENTER_CRITICAL();

  xTaskCreate(task1, "task1", 50, NULL, 2, &Task1_Handler);
  xTaskCreate(task2, "task2", 50, NULL, 2, &Task2_Handler);

  vTaskDelete(StartTask_Handler);
  taskEXIT_CRITICAL();
}

int main(void) {
  xTaskCreate(start_task, "start_task", 128, NULL, 1, &StartTask_Handler);
  vTaskStartScheduler();

  while (1) {
  }
}
