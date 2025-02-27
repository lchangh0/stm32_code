/*
 * sg90_servo.h
 *
 *  Created on: Feb 22, 2025
 *      Author: user
 */

#ifndef INC_SG90_SERVO_H_
#define INC_SG90_SERVO_H_

#include "stm32f4xx_hal.h"

extern TIM_HandleTypeDef htim4;

void Servo_Set_Angle(uint8_t angle);
void ChangeServoSG90Pwm();

#endif /* INC_SG90_SERVO_H_ */
