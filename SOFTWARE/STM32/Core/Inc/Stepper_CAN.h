/*
 * Stepper_CAN.h
 *
 *  Created on: Dec 10, 2025
 *      Author: lorenzo
 */

#ifndef INC_STEPPER_CAN_H_
#define INC_STEPPER_CAN_H_

#include "main.h"

typedef struct{
	uint8_t header;
	uint8_t data[3];
}StepperManualMode_t;

typedef struct{
	uint8_t header;
	uint8_t previous_angle;
	uint8_t init_angle;
	uint8_t data[2];
}StepperAngleMode_t;

typedef struct{
	uint8_t header;
}StepperSetMode_t;

void StepperManualMode(StepperManualMode_t *data);
void StepperAngleMode(StepperAngleMode_t *data);
void StepperSetMode(StepperSetMode_t *data);

void SetManualData(StepperManualMode_t *mode, uint8_t rotation, uint8_t steps, uint8_t speed);
void SetAngleData(StepperAngleMode_t *mode, uint8_t rotation, uint8_t steps);

#endif /* INC_STEPPER_CAN_H_ */
