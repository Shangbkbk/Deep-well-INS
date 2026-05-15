#include "Bluetooth.h"


// 定义全局变量（实际分配内存）
volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
volatile uint16_t rx_index = 0;
volatile uint8_t rx_complete = 0;

// 硬件配置宏
#define USARTx                  USART1
#define USARTx_CLK              RCC_APB2Periph_USART1
#define USARTx_GPIO_CLK         RCC_AHB1Periph_GPIOB
#define USARTx_TX_PIN           GPIO_Pin_6
#define USARTx_RX_PIN           GPIO_Pin_7
#define USARTx_GPIO_PORT        GPIOB
#define USARTx_TX_SOURCE        GPIO_PinSource6
#define USARTx_RX_SOURCE        GPIO_PinSource7
#define USARTx_AF               GPIO_AF_USART1

#define CONNECT_STATUS_PORT     GPIOB
#define CONNECT_STATUS_PIN      GPIO_Pin_8
#define CONNECT_STATUS_CLK      RCC_AHB1Periph_GPIOB

// 串口参数
#define BLE_BAUDRATE            115200

void BLUETOOTH_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    // 1. 使能 USART1 和 GPIOB 时钟
    RCC_APB2PeriphClockCmd(USARTx_CLK, ENABLE);
    RCC_AHB1PeriphClockCmd(USARTx_GPIO_CLK, ENABLE);
    RCC_AHB1PeriphClockCmd(CONNECT_STATUS_CLK, ENABLE);

    // 2. 配置连接检测引脚 PB8 为浮空输入
    GPIO_InitStruct.GPIO_Pin = CONNECT_STATUS_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(CONNECT_STATUS_PORT, &GPIO_InitStruct);

    // 3. 配置 USART1 复用功能
    GPIO_PinAFConfig(USARTx_GPIO_PORT, USARTx_TX_SOURCE, USARTx_AF);
    GPIO_PinAFConfig(USARTx_GPIO_PORT, USARTx_RX_SOURCE, USARTx_AF);

    // 4. 配置 TX (PB6) 为复用推挽
    GPIO_InitStruct.GPIO_Pin = USARTx_TX_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(USARTx_GPIO_PORT, &GPIO_InitStruct);

    // 5. 配置 RX (PB7) 为复用（默认浮空）
    GPIO_InitStruct.GPIO_Pin = USARTx_RX_PIN;
    GPIO_Init(USARTx_GPIO_PORT, &GPIO_InitStruct);

    // 6. 配置 USART1 参数
    USART_InitStruct.USART_BaudRate = BLE_BAUDRATE;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USARTx, &USART_InitStruct);

    // 7. 使能接收中断，配置优先级
    USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    // 8. 使能 USART1
    USART_Cmd(USARTx, ENABLE);
}

void BLUETOOTH_SendChar(char ch)
{
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    USART_SendData(USARTx, (uint8_t)ch);
}

void BLUETOOTH_SendString(char *str)
{
    while (*str)
    {
        BLUETOOTH_SendChar(*str++);
    }
}

void BLUETOOTH_SendBuffer(uint8_t *buffer, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        BLUETOOTH_SendChar(buffer[i]);
    }
}

uint8_t BLUETOOTH_IsConnected(void)
{
    // 连接状态引脚：未连接=高电平，已连接=低电平
    return (GPIO_ReadInputDataBit(CONNECT_STATUS_PORT, CONNECT_STATUS_PIN) == Bit_RESET);
}

uint8_t* BLUETOOTH_GetRxBuffer(void)
{
    return (uint8_t*)rx_buffer;
}

uint16_t BLUETOOTH_GetRxLength(void)
{
    return rx_index;
}

void BLUETOOTH_ClearRxBuffer(void)
{
    rx_index = 0;
    rx_complete = 0;
}
