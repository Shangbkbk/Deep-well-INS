#include "xsingle.h"
#include<string.h>

/* USART2 DMA接收缓冲区 */
//#define RS485_TX_EN() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET)//发送模式
//#define RS485_RX_EN() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET)//接收模式
//#define RS4852_RX_EN() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET)//发送模式
volatile uint8_t uart1_tx_busy = 0;
uint8_t uart2_rxbuf[PACK_SIZE];

void PC_SendBlock(uint8_t type,const uint8_t* data,uint16_t len)
{
    if(len == 0)
        return;

    if(uart1_tx_busy)
        return;

    uart1_tx_busy = 1;

    HAL_UART_Transmit_DMA(&huart1,(uint8_t*)data,len);
}

/* 开启USART2 ReceiveToIdle DMA接收 */
void Sensor_Start_DMA_Receive(void)
{
    /* 防止重复启动导致 HAL_BUSY：先停再启 */
    (void)HAL_UARTEx_ReceiveToIdle_DMA(&huart2, uart2_rxbuf, PACK_SIZE);
    __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
}

/* 发送并等待TC，保证自动收发RS485模块能切回接收 */
static void U2_SendAndWaitTC(uint8_t *buf, uint16_t len)
{
    (void)HAL_UART_Transmit(&huart2, buf, len, 200);
    while(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC) == RESET) {}
}

//crc校验位计算
uint16_t Modbus_CRC16(uint8_t *buf, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    uint16_t i, j;

    for(i = 0; i < len; i++)
    {
        crc ^= buf[i];

        for(j = 0; j < 8; j++)
        {
            if(crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }

    return crc;
}


uint8_t modbus_cmd[8];//采集帧

/************************************************
 * 函数名: Modbus_WriteReg
 * 功能  : 发送参数配置命令
 ************************************************/
void Modbus_WriteReg(uint16_t addr, uint16_t value)
{
    modbus_cmd[0] = 0x01; // ID
    modbus_cmd[1] = 0x06; // 写单寄存器
    modbus_cmd[2] = addr >> 8;
    modbus_cmd[3] = addr & 0xFF;
    modbus_cmd[4] = value >> 8;
    modbus_cmd[5] = value & 0xFF;

    uint16_t crc = Modbus_CRC16(modbus_cmd, 6);
    modbus_cmd[6] = crc & 0xFF;
    modbus_cmd[7] = crc >> 8;

    RS485_TX_EN();
		U2_SendAndWaitTC(modbus_cmd, 8);
    RS485_RX_EN();
}


/************************************************
 * 函数名: Modbus_Read
 * 功能  : 发送采集命令
 ************************************************/
void Modbus_Read(uint16_t addr, uint16_t num)
{
    uint8_t cmd[8];

    cmd[0] = 0x01;
    cmd[1] = 0x03;
    cmd[2] = addr >> 8;
    cmd[3] = addr & 0xFF;
    cmd[4] = (num >> 8) & 0xFF;
    cmd[5] = num & 0xFF;

    uint16_t crc = Modbus_CRC16(cmd, 6);
    cmd[6] = crc & 0xFF;
    cmd[7] = (crc >> 8) & 0xFF;

    RS485_TX_EN();
		U2_SendAndWaitTC(cmd, 8);
		RS485_RX_EN();
}

/************************************************
 * 函数名: Sensor_Stop
 * 功能  : 发送停止采集命令
 ************************************************/
void Sensor_Stop(void)
{
    uint8_t cmd[8];

    cmd[0] = 0x01;
    cmd[1] = 0x06;
    cmd[2] = 0x00;
    cmd[3] = 0x3A;
    cmd[4] = 0x00;
    cmd[5] = 0x01;
    cmd[6] = 0x68;
    cmd[7] = 0x07;
    // 停DMA接收
    HAL_UART_DMAStop(&huart2);
    RS485_TX_EN();
    U2_SendAndWaitTC(cmd, 8);
    RS485_RX_EN();
    Sensor_Start_DMA_Receive();
}

