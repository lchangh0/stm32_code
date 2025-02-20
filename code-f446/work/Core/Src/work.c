/*
 * work.c
 *
 *  Created on: Feb 19, 2025
 *      Author: user
 */

#include <stdio.h>
#include "work.h"

uint32_t tm_run_led = 0;
uint32_t run_led_interval = 500;

extern UART_HandleTypeDef huart2;
uint8_t rx2data;
uint8_t rx2buff[256];
uint8_t rx2_r_idx = 0;
uint8_t rx2_w_idx = 0;

void InitWork()
{
	printf("Start\r\n");

	// UART2 RX 인터럽트 활성
	HAL_UART_Receive_IT(&huart2, &rx2data, 1);
}


void DoWork()
{
	ReadButton();
	CheckButtonPressed();
	ChangeRunLed();

	CheckUart2Rx();
}


void CheckUart2Rx()
{
	uint8_t rx;
	if (GetUart2RxByte(&rx))
	{
		// Echo Back
		HAL_UART_Transmit(&huart2, &rx, 1, 0xffff);

		if (rx == '1')
		{
			run_led_interval = 100;
		}
		else if (rx == '2')
		{
			run_led_interval = 200;
		}
	}
}


// UART2 수신 인터럽트 처리
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART2)
	{
		rx2buff[rx2_w_idx] = rx2data;
		rx2_w_idx = (rx2_w_idx + 1) % sizeof(rx2buff);

		// UART2 RX 인터럽트 활성
		HAL_UART_Receive_IT(&huart2, &rx2data, 1);
	}
}

uint8_t GetUart2RxByte(uint8_t* rx)
{
	if (rx2_r_idx != rx2_w_idx)
	{
		*rx = rx2buff[rx2_r_idx];
		rx2_r_idx = (rx2_r_idx + 1) % sizeof(rx2buff);
		return 1;
	}
	else
		return 0;
}


// printf()함수를 실행했을 때 UART2로 출력한다.
// printf() 출력내용에 개행문자 \n이 있어야 UART로 출력된다.
int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, 0xffff);
	return ch;
}

uint32_t GetElapsedTick(uint32_t start)
{
	uint32_t now = HAL_GetTick();
	if (now >= start)
		return now - start;
	else
		return 0xffffffff - start + now;
}



void ChangeRunLed()
{
	if (GetElapsedTick(tm_run_led) >= run_led_interval)
	{
		tm_run_led = HAL_GetTick();
		ToggleRunLed();
	}
}

void ToggleRunLed()
{
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}

uint32_t tm_read_button = 0;
uint8_t rb_cont_cnt = 0;
uint8_t btn1_pressed = 0;
#define RB_CONT 50

void ReadButton()
{
	if (GetElapsedTick(tm_read_button) >= 1)
	{
		tm_read_button = HAL_GetTick();

		// Read Button1
		uint8_t val = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
		// button pressed
		if (!val)
		{
			if (rb_cont_cnt == RB_CONT)
			{
				btn1_pressed = 1;
				rb_cont_cnt++;
			}
			else if(rb_cont_cnt > RB_CONT)
			{}
			else
				rb_cont_cnt++;
		}
		else
		{
			rb_cont_cnt = 0;
		}
	}
}


void CheckButtonPressed()
{
	if (btn1_pressed)
	{
		btn1_pressed = 0;
		if (run_led_interval == 500)
			run_led_interval = 200;
		else
			run_led_interval = 500;
	}
}

