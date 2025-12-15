#include "com.h"
#include <string.h>
#include <stdio.h>
#include "usart.h"
#include "bmp280.h"
#include "mpu9250.h"

extern BMP280_HandleTypeDef bmp280;
extern mpu9250_data_t imu;
extern float K;

uint8_t rx_dma_buf[PROTO_UART_RX_BUF_SIZE];

uint16_t read_idx = 0;
uint16_t write_idx = 0;

extern uint8_t actual_angle;

static char s_line_buf[32];
static uint8_t s_line_pos = 0;

static char s_tx_buf[32];

static void PROTO_HandleCommand(const char *cmd)
{
    if (strcmp(cmd, "GET_T") == 0)
    {
        float T =(float) (bmp280.cal_temp)*0.01f;
        uint16_t len=0;
        len=snprintf((char *)s_tx_buf, 32, "%06.2f\n\r", T);
        HAL_UART_Transmit(&huart1, (uint8_t*)s_tx_buf, (uint16_t)len, HAL_MAX_DELAY);
    }
    else if (strcmp(cmd, "GET_P") == 0)
    {
        float P =(float) (bmp280.cal_press)*0.001f;
        uint16_t len=0;
        len=snprintf((char *)s_tx_buf,32, "%f\n\r", P);
        HAL_UART_Transmit(&huart1, (uint8_t*)s_tx_buf, (uint16_t)len, HAL_MAX_DELAY);
    }
    else if (strncmp(cmd, "SET_K=", 6) == 0)
    {
        int32_t k_raw = atoi(&cmd[6]);
        K=(float)k_raw*0.01;
        uint16_t len=0;
        len=snprintf((char *)s_tx_buf,32, "OK\n\r");
        HAL_UART_Transmit(&huart1, (uint8_t*)s_tx_buf, (uint16_t)len, HAL_MAX_DELAY);
    }
    else if (strcmp(cmd, "GET_K") == 0)
    {
        uint16_t len=0;
        len=snprintf((char *)s_tx_buf,32, "%08.5f\n\r", K);
        HAL_UART_Transmit(&huart1, (uint8_t*)s_tx_buf, (uint16_t)len, HAL_MAX_DELAY);
    }
    else if (strcmp(cmd, "GET_A") == 0)
    {
        float A = imu.mz;
        uint16_t len=0;
        len=snprintf((char *)s_tx_buf, 32, "%0.2f\n\r",(float) actual_angle * 180.0f/255.0f);
        HAL_UART_Transmit(&huart1, (uint8_t*)s_tx_buf, (uint16_t)len, HAL_MAX_DELAY);
    }
    else
    {
        uint16_t len=0;
        len=snprintf((char *)s_tx_buf, 32, "ERR\n\r");
        HAL_UART_Transmit(&huart1, (uint8_t*)s_tx_buf, (uint16_t)len, HAL_MAX_DELAY);
    }
}


void PROTO_Process(void)
{
    // Calculer l'index d'écriture actuel dans le buffer DMA
    // NDTR = nombre d'éléments restant à transférer
    uint16_t dma_ndtr = __HAL_DMA_GET_COUNTER(huart1.hdmarx);
    uint16_t write_idx = PROTO_UART_RX_BUF_SIZE - dma_ndtr;

    while (read_idx != write_idx)
    {
        uint8_t c = rx_dma_buf[read_idx];
        read_idx++;
        if (read_idx >= PROTO_UART_RX_BUF_SIZE){
            read_idx = 0;
        }

        if (c == '\r' || c == '\n')
        {
            if (s_line_pos > 0)
            {
                s_line_buf[s_line_pos] = '\0';
                PROTO_HandleCommand(s_line_buf);
                s_line_pos = 0;
            }
        }
        else
        {
            if (s_line_pos < sizeof(s_line_buf) - 1)
            {
                s_line_buf[s_line_pos++] = (char)c;
            }
        }
    }
}



