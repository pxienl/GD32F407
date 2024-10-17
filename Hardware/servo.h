#ifndef __SERVO_H

#define __SERVO_H

#include "gd32f4xx.h"

void servo_init();
void servo_update(uint16_t angle);

#endif