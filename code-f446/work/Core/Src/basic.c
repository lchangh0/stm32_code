/*
 * basic.c
 *
 *  Created on: Feb 22, 2025
 *      Author: user
 */

#include "basic.h"

extern UART_HandleTypeDef huart2;


uint32_t GetElapsedTick(uint32_t start)
{
	uint32_t now = HAL_GetTick();
	if (now >= start)
		return now - start;
	else
		return 0xffffffff - start + now;
}



// printf()함수를 실행했을 때 UART2로 출력한다.
// printf() 출력내용에 개행문자 \n이 있어야 UART로 출력된다.
int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, 0xffff);
	return ch;
}

