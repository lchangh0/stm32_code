/*
 * lib.c
 *
 *  Created on: Feb 16, 2025
 *      Author: user
 */

#include "lib.h"

uint32_t GetElapsedTick(uint32_t start)
{
	uint32_t now = HAL_GetTick();

	if (now >= start)
		return now - start;
	else
		return (0xffff - start) + now;
}
