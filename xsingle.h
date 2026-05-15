#ifndef __XSINGLE_H
#define __XSINGLE_H

#include "main.h"
#include <stdio.h>
#include <string.h>

#define PACK_SIZE    240
#define RS485_TX_EN() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET)//发送模式
#define RS485_RX_EN() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET)//接收模式

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;

/* USART2 DMA接收缓冲区 */
extern uint8_t uart2_rxbuf[PACK_SIZE];

/* 开启 USART2 ReceiveToIdle DMA 接收 */
void Sensor_Start_DMA_Receive(void);

/* Modbus CRC16 */
uint16_t Modbus_CRC16(uint8_t *buf, uint16_t len);

/* 写寄存器（配置采样率/采样长度等） */
void Modbus_WriteReg(uint16_t addr, uint16_t value);

/* 读寄存器（触发采集） */
void Modbus_Read(uint16_t addr, uint16_t num);

//停止采集
void Sensor_Stop(void);

/* 把 USART2 收到的数据封装并从 USART1 发给PC */
void PC_SendBlock(uint8_t type, const uint8_t* data, uint16_t len);

#endif
