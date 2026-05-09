#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

extern volatile uint16_t motor_timer;

void PwmMotor_Init(void);
void PwmMotor_SetValue(float value,uint8_t timeMs);

#endif