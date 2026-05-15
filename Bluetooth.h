#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include "stm32f4xx.h"
#include <stdint.h>

// 接收缓冲区大小
#define RX_BUFFER_SIZE   256

// 全局变量声明（定义在 Bluetooth.c 中）
extern volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
extern volatile uint16_t rx_index;
extern volatile uint8_t rx_complete;

// 函数声明
void BLUETOOTH_Init(void);
void BLUETOOTH_SendChar(char ch);
void BLUETOOTH_SendString(char *str);
void BLUETOOTH_SendBuffer(uint8_t *buffer, uint16_t len);
uint8_t BLUETOOTH_IsConnected(void);
uint8_t* BLUETOOTH_GetRxBuffer(void);
uint16_t BLUETOOTH_GetRxLength(void);
void BLUETOOTH_ClearRxBuffer(void);

#endif /* __BLUETOOTH_H */