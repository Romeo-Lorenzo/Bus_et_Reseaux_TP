/*
 * mpu9250.h
 *
 *  Created on: Nov 24, 2025
 *      Author: lorenzo
 */

#ifndef INC_MPU9250_H_
#define INC_MPU9250_H_


#include "stdint.h"
#include "stm32f4xx_hal.h" // Adapter selon MCU


extern I2C_HandleTypeDef hi2c1; // changer si besoin


#define MPU9250_ADDR (0x68 << 1)
#define AK8963_ADDR (0x0C << 1)


// --- Registres MPU9250 ------------------------------------------------------
#define WHO_AM_I_MPU9250 0x75
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C
#define INT_PIN_CFG 0x37
#define INT_ENABLE 0x38
#define USER_CTRL 0x6A
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define ACCEL_CONFIG2 0x1D
#define SMPLRT_DIV 0x19
#define ACCEL_XOUT_H 0x3B
#define GYRO_XOUT_H 0x43
#define TEMP_OUT_H 0x41


// --- Registres AK8963 (magnétomètre) ----------------------------------------
#define AK8963_WHO_AM_I 0x00
#define AK8963_ST1 0x02
#define AK8963_HXL 0x03
#define AK8963_ST2 0x09
#define AK8963_CNTL1 0x0A
#define AK8963_CNTL2 0x0B
#define AK8963_ASAX 0x10


// --- Plages de mesure --------------------------------------------------------
typedef enum {
MPU9250_ACCEL_2G = 0,
MPU9250_ACCEL_4G,
MPU9250_ACCEL_8G,
MPU9250_ACCEL_16G
} mpu9250_accel_range_t;


typedef enum {
MPU9250_GYRO_250DPS = 0,
MPU9250_GYRO_500DPS,
MPU9250_GYRO_1000DPS,
MPU9250_GYRO_2000DPS
} mpu9250_gyro_range_t;


// --- Données physiques -------------------------------------------------------
typedef struct {
float ax, ay, az;
float gx, gy, gz;
float mx, my, mz;
float temperature;
} mpu9250_data_t;


// --- API ---------------------------------------------------------------------
uint8_t MPU9250_Init(void);
uint8_t MPU9250_SetAccelRange(mpu9250_accel_range_t range);
uint8_t MPU9250_SetGyroRange(mpu9250_gyro_range_t range);
uint8_t MPU9250_SetSampleRate(uint16_t rate);
uint8_t MPU9250_EnableBypass(uint8_t enable);


uint8_t MPU9250_ReadAccelRaw(int16_t *ax, int16_t *ay, int16_t *az);
uint8_t MPU9250_ReadGyroRaw(int16_t *gx, int16_t *gy, int16_t *gz);
uint8_t MPU9250_ReadMagRaw(int16_t *mx, int16_t *my, int16_t *mz);
uint8_t MPU9250_ReadTempRaw(int16_t *t);


uint8_t MPU9250_ReadAll(mpu9250_data_t *out);


uint8_t MPU9250_WriteReg(uint8_t reg, uint8_t val);
uint8_t MPU9250_ReadRegs(uint8_t reg, uint8_t *buf, uint16_t len);

#endif /* INC_MPU9250_H_ */
