/*
 * mpu9250.c
 *
 *  Created on: Nov 24, 2025
 *      Author: lorenzo
 */

#include "mpu9250.h"
static float accel_scale = 16384.0f;
static float gyro_scale = 131.0f;
static float mag_adjust[3] = {1.f,1.f,1.f};


// --- Bas niveau I2C ----------------------------------------------------------
uint8_t MPU9250_WriteReg(uint8_t reg, uint8_t val) {
uint8_t tx[2]={reg,val};
return (HAL_I2C_Master_Transmit(&hi2c1, MPU9250_ADDR, tx, 2, 100)==HAL_OK);
}


uint8_t MPU9250_ReadRegs(uint8_t reg, uint8_t *buf, uint16_t len) {
if(HAL_I2C_Master_Transmit(&hi2c1, MPU9250_ADDR, &reg, 1, 100)!=HAL_OK) return 0;
return (HAL_I2C_Master_Receive(&hi2c1, MPU9250_ADDR, buf, len, 100)==HAL_OK);
}


// --- Magnétomètre AK8963 -----------------------------------------------------
static uint8_t ak8963_write(uint8_t reg, uint8_t val) {
uint8_t tx[2]={reg,val};
return (HAL_I2C_Master_Transmit(&hi2c1, AK8963_ADDR, tx, 2, 100)==HAL_OK);
}


static uint8_t ak8963_read(uint8_t reg, uint8_t *buf, uint16_t len) {
if(HAL_I2C_Master_Transmit(&hi2c1, AK8963_ADDR, &reg, 1, 100)!=HAL_OK) return 0;
return (HAL_I2C_Master_Receive(&hi2c1, AK8963_ADDR, buf, len, 100)==HAL_OK);
}


static uint8_t AK8963_Init(void) {
HAL_Delay(10);
// Power down
ak8963_write(AK8963_CNTL1, 0x00);
HAL_Delay(10);
// Enter fuse ROM
ak8963_write(AK8963_CNTL1, 0x0F);
HAL_Delay(10);


uint8_t asa[3];
if(!ak8963_read(AK8963_ASAX, asa, 3)) return 0;


mag_adjust[0] = ((float)(asa[0]-128)/256.f)+1.f;
mag_adjust[1] = ((float)(asa[1]-128)/256.f)+1.f;
mag_adjust[2] = ((float)(asa[2]-128)/256.f)+1.f;


// Power down
ak8963_write(AK8963_CNTL1, 0x00);
HAL_Delay(10);
// Mode continu, 16 bits, 100Hz
ak8963_write(AK8963_CNTL1, 0x16);
HAL_Delay(10);
return 1;
}

// --- MPU9250 INIT ------------------------------------------------------------
uint8_t MPU9250_Init(void) {
uint8_t who;
if(!MPU9250_ReadRegs(WHO_AM_I_MPU9250, &who, 1)) return 0;


// Reset
MPU9250_WriteReg(PWR_MGMT_1, 0x80);
HAL_Delay(100);


// Clock PLL
MPU9250_WriteReg(PWR_MGMT_1, 0x01);
HAL_Delay(10);


MPU9250_WriteReg(PWR_MGMT_2, 0x00);
MPU9250_WriteReg(INT_PIN_CFG, 0x02); // BYPASS_EN


// Config défaut
MPU9250_SetAccelRange(MPU9250_ACCEL_2G);
MPU9250_SetGyroRange(MPU9250_GYRO_250DPS);
MPU9250_WriteReg(ACCEL_CONFIG2, 0x03);
MPU9250_SetSampleRate(100); // 100 Hz


// Magnétomètre
if(!AK8963_Init()) return 0;


return 1;
}


// --- Config range ------------------------------------------------------------
uint8_t MPU9250_SetAccelRange(mpu9250_accel_range_t range) {
uint8_t val = (range << 3);
MPU9250_WriteReg(ACCEL_CONFIG, val);


switch(range) {
case MPU9250_ACCEL_2G: accel_scale=16384.f; break;
case MPU9250_ACCEL_4G: accel_scale=8192.f; break;
case MPU9250_ACCEL_8G: accel_scale=4096.f; break;
case MPU9250_ACCEL_16G: accel_scale=2048.f; break;
}
return 1;
}


uint8_t MPU9250_SetGyroRange(mpu9250_gyro_range_t range) {
uint8_t val = (range << 3);
MPU9250_WriteReg(GYRO_CONFIG, val);


switch(range) {
case MPU9250_GYRO_250DPS: gyro_scale=131.f; break;
case MPU9250_GYRO_500DPS: gyro_scale=65.5f; break;
case MPU9250_GYRO_1000DPS: gyro_scale=32.8f; break;
case MPU9250_GYRO_2000DPS: gyro_scale=16.4f; break;
}
return 1;
}

uint8_t MPU9250_SetSampleRate(uint16_t rate) {
// SampleRate = GyroOutputRate / (1 + SMPLRT_DIV)
// GyroOutputRate = 1kHz
uint8_t div = (1000/rate) - 1;
return MPU9250_WriteReg(SMPLRT_DIV, div);
}


uint8_t MPU9250_EnableBypass(uint8_t enable) {
return MPU9250_WriteReg(INT_PIN_CFG, enable?0x02:0x00);
}


// --- Lectures brutes ----------------------------------------------------------
uint8_t MPU9250_ReadAccelRaw(int16_t *ax, int16_t *ay, int16_t *az) {
uint8_t d[6];
if(!MPU9250_ReadRegs(ACCEL_XOUT_H, d, 6)) return 0;
*ax = (d[0]<<8)|d[1];
*ay = (d[2]<<8)|d[3];
*az = (d[4]<<8)|d[5];
return 1;
}


uint8_t MPU9250_ReadGyroRaw(int16_t *gx, int16_t *gy, int16_t *gz) {
uint8_t d[6];
if(!MPU9250_ReadRegs(GYRO_XOUT_H, d, 6)) return 0;
*gx = (d[0]<<8)|d[1];
*gy = (d[2]<<8)|d[3];
*gz = (d[4]<<8)|d[5];
return 1;
}


uint8_t MPU9250_ReadTempRaw(int16_t *t) {
uint8_t d[2];
if(!MPU9250_ReadRegs(TEMP_OUT_H, d, 2)) return 0;
*t = (d[0]<<8)|d[1];
return 1;
}


uint8_t MPU9250_ReadMagRaw(int16_t *mx, int16_t *my, int16_t *mz) {
uint8_t st1;
if(!ak8963_read(AK8963_ST1, &st1, 1)) return 0;
if(!(st1 & 0x01)) return 0;


uint8_t d[7];
if(!ak8963_read(AK8963_HXL, d, 7)) return 0;
if(d[6] & 0x08) return 0; // overflow


*mx = (d[1]<<8)|d[0];
*my = (d[3]<<8)|d[2];
*mz = (d[5]<<8)|d[4];
return 1;
}

// --- Lecture complète ---------------------------------------------------------
uint8_t MPU9250_ReadAll(mpu9250_data_t *out) {
int16_t ax,ay,az,gx,gy,gz,mx,my,mz,t;


if(!MPU9250_ReadAccelRaw(&ax,&ay,&az)) return 0;
if(!MPU9250_ReadGyroRaw(&gx,&gy,&gz)) return 0;
MPU9250_ReadMagRaw(&mx,&my,&mz);
MPU9250_ReadTempRaw(&t);


out->ax = ax / accel_scale;
out->ay = ay / accel_scale;
out->az = az / accel_scale;


out->gx = gx / gyro_scale;
out->gy = gy / gyro_scale;
out->gz = gz / gyro_scale;


out->mx = mx * mag_adjust[0] * 0.15f;
out->my = my * mag_adjust[1] * 0.15f;
out->mz = mz * mag_adjust[2] * 0.15f;


out->temperature = ((float)t)/333.87f + 21.0f;


return 1;
}
