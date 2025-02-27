/*
 * sg90_servo.c
 *
 *  Created on: Feb 22, 2025
 *      Author: user
 */

#include "basic.h"
#include "sg90_servo.h"
#include "hc-sr04.h"

void Servo_Set_Angle(uint8_t angle)
{
	if (angle > 180)
		angle = 180;

	// 0도=0.5ms, 180도=2.5ms
	uint32_t pulse = 500 + ((angle * 2000) / 180);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, pulse);
}


uint32_t tm_sg90;
uint8_t sg90_angle = 0;

void ChangeServoSG90Pwm()
{
	/*
	if (GetElapsedTick(tm_sg90) >= 1000)
	{
		tm_sg90 = HAL_GetTick();

		switch(sg90_angle)
		{
			case 0:
				sg90_angle =90;
				break;

			case 90:
				sg90_angle = 180;
				break;

			case 180:
				sg90_angle = 0;
				break;

			default:
				sg90_angle = 0;
				break;
		}

		Servo_Set_Angle(sg90_angle);
	}
	*/

	if (sonic_distance <= 15)
		sg90_angle = 180;
	else if (sonic_distance <= 30)
		sg90_angle = 90;
	else
		sg90_angle = 0;

	Servo_Set_Angle(sg90_angle);
}
