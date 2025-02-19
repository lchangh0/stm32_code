/*
 * work.h
 *
 *  Created on: Feb 15, 2025
 *      Author: user
 */

#ifndef INC_WORK_H_
#define INC_WORK_H_

#include "stm32f1xx_hal.h"

extern void InitWork();
extern void DoWork();
extern void DoWorkAlways();
extern void DoWorkEvery1Sec();

extern void ReadButton();
void SetLedD1(uint8_t on);
void SetLedD2(uint8_t on);
extern void ToggleLedD1();
extern void ToggleLedD2();
extern void ToggleRunLed();

extern uint16_t GetUart1RxCount();
extern uint8_t ReadUart1RxByte();

extern int SendCanFrame(uint32_t id, int data_len, uint8_t data[]);
extern int SendCanMessagePoll();
extern int SendCanMessageButton1(uint8_t button);
extern int SendCanMessageButton2(uint8_t button);


#endif /* INC_WORK_H_ */
