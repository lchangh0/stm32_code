/*
 * work.c
 *
 *  Created on: Feb 15, 2025
 *      Author: user
 */

#include <stdio.h>
#include "lib.h"
#include "work.h"


extern CAN_HandleTypeDef hcan;
extern UART_HandleTypeDef huart1;

uint8_t rx1data;
volatile uint8_t rx1_buff[256];	// 링버퍼
volatile uint16_t rx1_w_idx = 0;
volatile uint16_t rx1_r_idx = 0;

void InitWork()
{
	// UART1 RX 인터럽트 활성
	HAL_UART_Receive_IT(&huart1, &rx1data, 1);

	// CAN 통신
	HAL_CAN_Start(&hcan);
	if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	{
		printf("CAN Activate Error\r\n");
	}
}

// UART1 RX 인터럽트 핸들러
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		rx1_buff[rx1_w_idx] = rx1data;
		rx1_w_idx = (rx1_w_idx + 1) % sizeof(rx1_buff);

		// UART1 RX 인터럽트 활성
		HAL_UART_Receive_IT(&huart1, &rx1data, 1);
	}
}

#define MAX_CAN_RX_FRAMES 50

typedef struct {
	CAN_RxHeaderTypeDef header;
	uint8_t data[8];
} can_rx_frame_t;

can_rx_frame_t can_rx_frames[MAX_CAN_RX_FRAMES];
volatile uint8_t can_rx_in = 0;
volatile uint8_t can_rx_out = 0;

// CAN RX_FIFO0 인터럽트 핸들러
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0,
			&can_rx_frames[can_rx_in].header,
			can_rx_frames[can_rx_in].data) == HAL_OK)
	{
		can_rx_in = (can_rx_in + 1) % MAX_CAN_RX_FRAMES;
	}
	else
	{
		printf("CAN RX Error\r\n");
	}
}



uint16_t GetUart1RxCount()
{
	if (rx1_w_idx >= rx1_r_idx)
	{
		return rx1_w_idx - rx1_r_idx;
	}
	else
	{
		return sizeof(rx1_buff) - rx1_r_idx + rx1_w_idx;
	}
}

uint8_t ReadUart1Byte()
{
	uint8_t data = rx1_buff[rx1_r_idx];
	rx1_r_idx = (rx1_r_idx + 1) % sizeof(rx1_buff);
	return data;
}

void WriteUart1Bytes(uint8_t* buff, int len)
{
	HAL_UART_Transmit(&huart1, buff, len, 0xffff);
}


uint32_t tm_sw4=0;
uint8_t old_sw4=0;
uint8_t sw4_cnt=0;
uint8_t tmp_sw4=0;

uint32_t tm_sw5=0;
uint8_t old_sw5=0;
uint8_t sw5_cnt=0;
uint8_t tmp_sw5=0;


void ReadButton()
{
	// 10msec마다 sw4상태를 읽고 3번 연속 같은 값이면 입력 처리
	if(GetElapsedTick(tm_sw4) >= 10) {
		tm_sw4 = HAL_GetTick();
		tmp_sw4 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12);

		if(old_sw4 != tmp_sw4) {
			old_sw4 = tmp_sw4;
			sw4_cnt = 3;
		} else {
			if(sw4_cnt) {
				sw4_cnt--;
				if(sw4_cnt == 0) {
					// pushed down
					if (tmp_sw4 == 0)
					{
						SendCanMessageButton1(tmp_sw4);
						ToggleLedD1();
						printf("Button1 pushed\r\n");
					}
				}
			}
		}
	}

	// 10msec마다 sw5상태를 읽고 3번 연속 같은 값이면 입력 처리
	if(GetElapsedTick(tm_sw5) >= 10) {
		tm_sw5 = HAL_GetTick();
		tmp_sw5 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13);

		if(old_sw5 != tmp_sw5) {
			old_sw5 = tmp_sw5;
			sw5_cnt = 3;
		} else {
			if(sw5_cnt) {
				sw5_cnt--;
				if(sw5_cnt == 0) {
					// pushed down
					if (tmp_sw5 == 0)
					{
						SendCanMessageButton2(tmp_sw5);
						ToggleLedD1();
						printf("Bbutton2 pushed\r\n");
					}
				}
			}
		}
	}
}



uint32_t tm_run_led = 0;

void ToggleRunLed()
{
	if(GetElapsedTick(tm_run_led) >= 500)
	{
		tm_run_led = HAL_GetTick();

		// RUN LED Toggle
		ToggleLedD2();
	}
}

void SetLedD1(uint8_t on)
{
	if(on)
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
}

void ToggleLedD1()
{
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
}

void SetLedD2(uint8_t on)
{
	if(on)
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
}

void ToggleLedD2()
{
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_15);
}


void DoWork()
{
	ToggleRunLed();
	ReadButton();

	DoWorkAlways();
	DoWorkEvery1Sec();
}


void DoWorkAlways()
{

	uint16_t rx1Cnt = GetUart1RxCount();
	if (rx1Cnt > 0)
	{
		for(int i = 0; i < rx1Cnt; i++)
		{
			uint8_t rx = ReadUart1Byte();
			// Echo Back
			WriteUart1Bytes(&rx, 1);
		}
	}

	if (can_rx_in != can_rx_out)
	{
		can_rx_frame_t *frame = &can_rx_frames[can_rx_out];
		can_rx_out = (can_rx_out + 1) % MAX_CAN_RX_FRAMES;

		printf("CAN RX ID:%3lX, DLC:%ld, Data", frame->header.StdId, frame->header.DLC);
		for(int i = 0; i < frame->header.DLC; i++)
			printf("%02X ", frame->data[i]);
		printf("\r\n");
	}
}


uint32_t tm_work_1sec = 0;

void DoWorkEvery1Sec()
{
	if (GetElapsedTick(tm_work_1sec) >= 1000)
	{
		tm_work_1sec = HAL_GetTick();

		// CAN Poll Message
		SendCanMessagePoll();
	}
}




int SendCanFrame(uint32_t id, int data_len, uint8_t data[])
{
	CAN_TxHeaderTypeDef can_tx_header;
	uint8_t             can_tx_data[8];
	uint32_t            can_tx_mailbox;

	can_tx_header.StdId = id;
	can_tx_header.RTR = CAN_RTR_DATA;
	can_tx_header.IDE = CAN_ID_STD;
	can_tx_header.DLC = data_len;
	can_tx_header.TransmitGlobalTime = DISABLE;

	for(int i = 0; i < data_len; i++)
		can_tx_data[i] = data[i];

	if (HAL_CAN_AddTxMessage(&hcan, &can_tx_header, can_tx_data, &can_tx_mailbox) != HAL_OK)
	{
		return -1;
	}
	return 0;
}

int iPollCnt;

int SendCanMessagePoll()
{
	iPollCnt += 100000;

	uint8_t can_tx_data[8];

	can_tx_data[0] = 0;
	can_tx_data[1] = 0;
	can_tx_data[2] = 0;
	can_tx_data[3] = 0;
	can_tx_data[4] = (uint8_t)((iPollCnt >> 24) & 0xff);
	can_tx_data[5] = (uint8_t)((iPollCnt >> 16) & 0xff);
	can_tx_data[6] = (uint8_t)((iPollCnt >> 8) & 0xff);
	can_tx_data[7] = (uint8_t)(iPollCnt & 0xff);
	return SendCanFrame(0x03, 8, can_tx_data);
}

int SendCanMessageButton1(uint8_t button)
{
	uint8_t can_tx_data[8];

	can_tx_data[0] = 0;
	can_tx_data[1] = 0;
	can_tx_data[2] = 0;
	can_tx_data[3] = 0;
	can_tx_data[4] = 0;
	can_tx_data[5] = 0;
	can_tx_data[6] = 0;
	can_tx_data[7] = button;
	return SendCanFrame(0x04, 8, can_tx_data);
}

int SendCanMessageButton2(uint8_t button)
{
	uint8_t can_tx_data[8];

	can_tx_data[0] = 0;
	can_tx_data[1] = 0;
	can_tx_data[2] = 0;
	can_tx_data[3] = 0;
	can_tx_data[4] = 0;
	can_tx_data[5] = 0;
	can_tx_data[6] = 0;
	can_tx_data[7] = button;
	return SendCanFrame(0x05, 8, can_tx_data);
}


