#ifndef BMP280_H
#define BMP280_H

#include "stm32f4xx_hal.h"  // adapte: stm32g4xx_hal.h, stm32h7xx_hal.h, etc.

/* ========= I2C address (7-bit) ========= */
#define BMP280_I2C_ADDR_SDO_LOW   0x76u  /* SDO = GND */
#define BMP280_I2C_ADDR_SDO_HIGH  0x77u  /* SDO = VDDIO */


/* ========= Registers ========= */
#define BMP280_REG_CHIP_ID        0xD0u
#define BMP280_REG_RESET          0xE0u
#define BMP280_REG_STATUS         0xF3u
#define BMP280_REG_CTRL_MEAS      0xF4u
#define BMP280_REG_CONFIG         0xF5u

/* Raw data registers */
#define BMP280_REG_PRESS_MSB      0xF7u
#define BMP280_REG_PRESS_LSB      0xF8u
#define BMP280_REG_PRESS_XLSB     0xF9u
#define BMP280_REG_TEMP_MSB       0xFAu
#define BMP280_REG_TEMP_LSB       0xFBu
#define BMP280_REG_TEMP_XLSB      0xFCu

/* Calibration data registers */
#define BMP280_REG_CALIB_START    0x88u
#define BMP280_CALIB_LENGTH       24u

/* ========= Reset value ========= */
#define BMP280_SOFTRESET_VALUE    0xB6u
#define BMP280_CHIP_ID_VALUE      0x58u

/* ========= ctrl_meas bits ========= */
/* osrs_t[2:0] bits 7:5 */
#define BMP280_OSRS_T_SKIP        0x00u
#define BMP280_OSRS_T_x1          (1u << 5)
#define BMP280_OSRS_T_x2          (2u << 5)
#define BMP280_OSRS_T_x4          (3u << 5)
#define BMP280_OSRS_T_x8          (4u << 5)
#define BMP280_OSRS_T_x16         (5u << 5)

/* osrs_p[2:0] bits 4:2 */
#define BMP280_OSRS_P_SKIP        0x00u
#define BMP280_OSRS_P_x1          (1u << 2)
#define BMP280_OSRS_P_x2          (2u << 2)
#define BMP280_OSRS_P_x4          (3u << 2)
#define BMP280_OSRS_P_x8          (4u << 2)
#define BMP280_OSRS_P_x16         (5u << 2)

/* mode[1:0] bits 1:0 */
#define BMP280_MODE_SLEEP         0x00u
#define BMP280_MODE_FORCED        0x01u
#define BMP280_MODE_NORMAL        0x03u

/* ========= Types ========= */

typedef struct
{
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
    int32_t  t_fine;
} BMP280_CalibData;

typedef struct
{
	int32_t raw_temp;
	int32_t raw_press;
	uint32_t cal_temp;
	uint32_t cal_press;
    BMP280_CalibData   calib;
} BMP280_HandleTypeDef;

/* ========= API ========= */
HAL_StatusTypeDef bmp280_write_reg(BMP280_HandleTypeDef *dev,uint8_t reg,uint8_t value);
HAL_StatusTypeDef bmp280_read_regs(BMP280_HandleTypeDef *dev,uint8_t reg,uint8_t *pData,uint16_t size);
HAL_StatusTypeDef bmp280_read_calibration(BMP280_HandleTypeDef *dev);
HAL_StatusTypeDef BMP280_Init(BMP280_HandleTypeDef *dev);
HAL_StatusTypeDef BMP280_Reset(BMP280_HandleTypeDef *dev);
HAL_StatusTypeDef BMP280_ReadRaw(BMP280_HandleTypeDef *dev);

void BMP280_Compensate_T_int32(BMP280_HandleTypeDef *dev);  /* 0.01 °C */
void BMP280_Compensate_P_uint32(BMP280_HandleTypeDef *dev); /* Pa (Q24.8 / ~1/256 Pa) */

#endif /* BMP280_H */
