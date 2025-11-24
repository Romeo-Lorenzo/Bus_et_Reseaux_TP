#include "bmp280.h"
#include "i2c.h"
/* ==== Helpers I2C - uniquement Master_Transmit / Master_Receive ==== */

HAL_StatusTypeDef bmp280_write_reg(BMP280_HandleTypeDef *dev,uint8_t reg,uint8_t value)
{
    uint8_t buf[2] = { reg, value };
    return HAL_I2C_Master_Transmit(&hi2c1,(BMP280_I2C_ADDR_SDO_HIGH << 1),buf,2,HAL_MAX_DELAY);
}

HAL_StatusTypeDef bmp280_read_regs(BMP280_HandleTypeDef *dev,uint8_t reg,uint8_t *pData,uint16_t size)
{
    HAL_StatusTypeDef status;
    status = HAL_I2C_Master_Transmit(&hi2c1,(BMP280_I2C_ADDR_SDO_HIGH << 1),&reg,1,HAL_MAX_DELAY);
    if (status != HAL_OK){
        return status;
    }
    status = HAL_I2C_Master_Receive(&hi2c1,(BMP280_I2C_ADDR_SDO_HIGH << 1),pData,size,HAL_MAX_DELAY);
    return status;
}


HAL_StatusTypeDef bmp280_read_calibration(BMP280_HandleTypeDef *dev)
{
    uint8_t calib[BMP280_CALIB_LENGTH];
    HAL_StatusTypeDef status;

    status = bmp280_read_regs(dev, BMP280_REG_CALIB_START, calib, BMP280_CALIB_LENGTH);
    if (status != HAL_OK) return status;

    dev->calib.dig_T1 = (uint16_t)((calib[1] << 8) | calib[0]);
    dev->calib.dig_T2 = (int16_t)((calib[3] << 8) | calib[2]);
    dev->calib.dig_T3 = (int16_t)((calib[5] << 8) | calib[4]);

    dev->calib.dig_P1 = (uint16_t)((calib[7] << 8) | calib[6]);
    dev->calib.dig_P2 = (int16_t)((calib[9] << 8) | calib[8]);
    dev->calib.dig_P3 = (int16_t)((calib[11] << 8) | calib[10]);
    dev->calib.dig_P4 = (int16_t)((calib[13] << 8) | calib[12]);
    dev->calib.dig_P5 = (int16_t)((calib[15] << 8) | calib[14]);
    dev->calib.dig_P6 = (int16_t)((calib[17] << 8) | calib[16]);
    dev->calib.dig_P7 = (int16_t)((calib[19] << 8) | calib[18]);
    dev->calib.dig_P8 = (int16_t)((calib[21] << 8) | calib[20]);
    dev->calib.dig_P9 = (int16_t)((calib[23] << 8) | calib[22]);

    return HAL_OK;
}

/* ==== Public API ==== */

HAL_StatusTypeDef BMP280_Reset(BMP280_HandleTypeDef *dev)
{
    return bmp280_write_reg(dev, BMP280_REG_RESET, BMP280_SOFTRESET_VALUE);
}

HAL_StatusTypeDef BMP280_Init(BMP280_HandleTypeDef *dev)
{
    HAL_StatusTypeDef status;
    uint8_t id;

    status = bmp280_read_regs(dev, BMP280_REG_CHIP_ID, &id, 1);
    if (status != HAL_OK) return status;
    if (id != BMP280_CHIP_ID_VALUE) return HAL_ERROR;

    status = BMP280_Reset(dev);
    if (status != HAL_OK) return status;

    HAL_Delay(5);

    uint8_t ctrl_meas = BMP280_OSRS_T_x2 | BMP280_OSRS_P_x16 | BMP280_MODE_NORMAL;
    status = bmp280_write_reg(dev, BMP280_REG_CTRL_MEAS, ctrl_meas);
    if (status != HAL_OK) return status;


    status = bmp280_read_calibration(dev);
    if (status != HAL_OK) return status;


    return HAL_OK;
}


HAL_StatusTypeDef BMP280_ReadRaw(BMP280_HandleTypeDef *dev)
{
    uint8_t data[6];
    HAL_StatusTypeDef status;

    status = bmp280_read_regs(dev, BMP280_REG_PRESS_MSB, data, 6);
    if (status != HAL_OK) return status;

    int32_t adc_P = ((int32_t)data[0] << 12) | ((int32_t)data[1] << 4) | ((int32_t)data[2] >> 4);
    int32_t adc_T = ((int32_t)data[3] << 12) | ((int32_t)data[4] << 4) | ((int32_t)data[5] >> 4);

    dev->raw_press = adc_P;
    dev->raw_temp  = adc_T;

    return HAL_OK;
}

void BMP280_Compensate_T_int32(BMP280_HandleTypeDef *dev)
{
    int32_t var1, var2;
    BMP280_CalibData *c = &dev->calib;

    var1 = ((((dev->raw_temp >> 3) - ((int32_t)c->dig_T1 << 1))) * ((int32_t)c->dig_T2)) >> 11;
    var2 = (((((dev->raw_temp >> 4) - ((int32_t)c->dig_T1)) *((dev->raw_temp >> 4) - ((int32_t)c->dig_T1))) >> 12) *
    		((int32_t)c->dig_T3)) >> 14;
    c->t_fine = var1 + var2;
    dev->cal_temp = (c->t_fine * 5 + 128) >> 8;  /* 0.01 °C */
}

void BMP280_Compensate_P_uint32(BMP280_HandleTypeDef *dev)
{
    BMP280_CalibData *c = &dev->calib;
    int32_t var1, var2;
    uint32_t p;

    var1 = (((int32_t)c->t_fine) >> 1) - (int32_t)64000;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * (int32_t)c->dig_P6;
    var2 = var2 + ((var1 * (int32_t)c->dig_P5) << 1);
    var2 = (var2 >> 2) + ((int32_t)c->dig_P4 << 16);
    var1 = (((c->dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) +
           (((int32_t)c->dig_P2 * var1) >> 1)) >> 18;
    var1 = (((32768 + var1)) * (int32_t)c->dig_P1) >> 15;
    if (var1 == 0)
    {
        return 0;
    }

    p = ((uint32_t)(((int32_t)1048576) - dev->raw_press) - (var2 >> 12)) * 3125U;

    if (p < 0x80000000UL)
    {
        p = (p << 1) / (uint32_t)var1;
    }
    else
    {
        p = (p / (uint32_t)var1) * 2U;
    }

    var1 = ((int32_t)c->dig_P9 * (int32_t)(((p >> 3) * (p >> 3)) >> 13)) >> 12;
    var2 = ((int32_t)(p >> 2) * (int32_t)c->dig_P8) >> 13;
    p = (uint32_t)((int32_t)p + ((var1 + var2 + c->dig_P7) >> 4));

    dev->cal_press=p;
}


