/*
 * stepmotor-uln2003.c
 *
 *  Created on: Feb 25, 2025
 *      Author: user
 */

#include "stepmotor-uln2003.h"

#define STEP_DELAY 1	// 모터 속도 조절 (단위: ms)

// ULN2003 핀 설정
#define IN1 GPIO_PIN_5
#define IN2 GPIO_PIN_6
#define IN3 GPIO_PIN_8
#define IN4 GPIO_PIN_9
// GPIOC 사용
#define IN_PORT GPIOC

#define STEP 8

// **한 스텝씩 회전하는 함수**
void Stepper_Step(int step) {
    static const uint8_t stepSequence[STEP][4] = {
        {1, 0, 0, 0},
        {1, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 1, 1},
        {0, 0, 0, 1},
        {1, 0, 0, 1},
    };

    HAL_GPIO_WritePin(IN_PORT, IN1, stepSequence[step][0]);
    HAL_GPIO_WritePin(IN_PORT, IN2, stepSequence[step][1]);
    HAL_GPIO_WritePin(IN_PORT, IN3, stepSequence[step][2]);
    HAL_GPIO_WritePin(IN_PORT, IN4, stepSequence[step][3]);
    HAL_Delay(STEP_DELAY);
}

// **스테퍼 모터 회전 함수**
void Stepper_Rotate(int steps, int direction) {
    for (int i = 0; i < steps; i++) {
        if (direction == 1) {  // 시계 방향
            Stepper_Step(i % STEP);
        } else {  // 반시계 방향
            Stepper_Step(((STEP-1) - (i % STEP)) % STEP);
        }
    }
}

uint32_t tm_smotor;
uint8_t smotor_dir;

void DoStepMotorWork()
{
	if (GetElapsedTick(tm_smotor) >= 1000)
	{
		tm_smotor = HAL_GetTick();

		smotor_dir = (smotor_dir + 1) % 2;
		Stepper_Rotate(2048, smotor_dir);
	}
}
