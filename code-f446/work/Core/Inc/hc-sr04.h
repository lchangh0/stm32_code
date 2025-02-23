/*
 * hc-sr04.h
 *
 *  Created on: Feb 23, 2025
 *      Author: user
 */

#ifndef INC_HC_SR04_H_
#define INC_HC_SR04_H_

#include "stm32f4xx_hal.h"

extern TIM_HandleTypeDef htim1;

extern uint32_t sonic_distance;

void RunSonicSensor();

#endif /* INC_HC_SR04_H_ */
