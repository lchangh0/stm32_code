/*
 * can1.c
 *
 *  Created on: Feb 25, 2025
 *      Author: user
 */

#include "can1.h"


// 수신된 CAN 메시지 처리
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t RxData[8];

    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {
        printf("Received ID: 0x%lX, Data: ", RxHeader.StdId);
        for (int i = 0; i < RxHeader.DLC; i++) {
            printf("%02X ", RxData[i]);
        }
        printf("\r\n");
    }
}


// CAN 메시지 송신 함수
void CAN_SendMessage(void) {
    CAN_TxHeaderTypeDef TxHeader;
    uint8_t TxData[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    uint32_t TxMailbox;

    TxHeader.StdId = 0x321;  // 표준 ID (0x321)
    TxHeader.ExtId = 0;
    TxHeader.IDE = CAN_ID_STD; // 표준 ID 사용
    TxHeader.RTR = CAN_RTR_DATA; // 데이터 프레임
    TxHeader.DLC = 8;       // 데이터 길이 (8바이트)
    TxHeader.TransmitGlobalTime = DISABLE;

    if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK) {
        Error_Handler();
    }
}


void InitCan1()
{
  // CAN 필터 설정
  CAN_FilterTypeDef canFilter;
  canFilter.FilterBank = 0;
  canFilter.FilterMode = CAN_FILTERMODE_IDMASK;
  canFilter.FilterScale = CAN_FILTERSCALE_32BIT;
  canFilter.FilterIdHigh = 0x0000;
  canFilter.FilterIdLow = 0x0000;
  canFilter.FilterMaskIdHigh = 0x0000;
  canFilter.FilterMaskIdLow = 0x0000;
  canFilter.FilterFIFOAssignment = CAN_RX_FIFO0;
  canFilter.FilterActivation = ENABLE;

  if (HAL_CAN_ConfigFilter(&hcan1, &canFilter) != HAL_OK) {
	  Error_Handler();
  }

  // CAN 시작 및 인터럽트 활성화
  if (HAL_CAN_Start(&hcan1) != HAL_OK) {
	  Error_Handler();
  }

  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
	  Error_Handler();
  }
}


uint32_t tm_can1;

void DoCan1Work()
{
	if (GetElapsedTick(tm_can1) >= 1000)
	{
		tm_can1 = HAL_GetTick();

		CAN_SendMessage();
	}
}
