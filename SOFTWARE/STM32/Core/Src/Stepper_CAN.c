/*
 * Stepper_CAN.c
 *
 *  Created on: Dec 10, 2025
 *      Author: lorenzo
 */

#include "Stepper_CAN.h"

void StepperManualMode(StepperManualMode_t *data){
	CAN_TxHeaderTypeDef header;
	uint32_t mailbox;
	header.StdId = data->header;
	header.IDE = CAN_ID_STD;
	header.RTR = CAN_RTR_DATA;
	header.DLC = sizeof(data->data)/sizeof(data->data[0]);

	HAL_CAN_AddTxMessage(&hcan1, &header, data->data, &mailbox);
}

void StepperAngleMode(StepperAngleMode_t *data){
	CAN_TxHeaderTypeDef header;
	uint32_t mailbox;
	header.StdId = data->header;
	header.IDE = CAN_ID_STD;
	header.RTR = CAN_RTR_DATA;
	header.DLC = sizeof(data->data)/sizeof(data->data[0]);

	HAL_CAN_AddTxMessage(&hcan1, &header, data->data, &mailbox);
}

void StepperSetMode(StepperSetMode_t *data){
	CAN_TxHeaderTypeDef header;
	uint32_t mailbox;
	header.StdId = data->header;
	header.IDE = CAN_ID_STD;
	header.RTR = CAN_RTR_DATA;
	header.DLC = 0;

	HAL_CAN_AddTxMessage(&hcan1, &header, NULL, &mailbox);
}

void SetManualData(StepperManualMode_t *mode, uint8_t rotation, uint8_t steps, uint8_t speed){
	mode->data[0]=rotation;
	mode->data[1]=steps;
	mode->data[2]=speed;

	StepperManualMode(mode);

}

void SetAngleData(StepperAngleMode_t *mode, uint8_t rotation, uint8_t steps){
	mode->data[0]=rotation;
	mode->data[1]=steps;

	StepperAngleMode(mode);
}

