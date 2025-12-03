#ifndef PROTO_UART_H
#define PROTO_UART_H

#include "stm32f4xx_hal.h"   // adapte: stm32f1xx_hal.h, stm32h7xx_hal.h, etc.
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Taille du buffer DMA circulaire
#define PROTO_UART_RX_BUF_SIZE   128
void PROTO_Init(UART_HandleTypeDef *huart);
void PROTO_Process(void);

#ifdef __cplusplus
}
#endif

#endif // PROTO_UART_H
