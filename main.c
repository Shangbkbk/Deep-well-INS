#include "stm32f4xx.h"
#include "bluetooth.h"
#include "delay.h"

void SystemClock_Config(void);

int main(void)
{
    SystemClock_Config();
    Delay_Init();
    BLUETOOTH_Init();

    Delay_Ms(2000);
    
    // 直接发送固定字符串，不带任何变量
    BLUETOOTH_SendString("Hello123\r\n");

    while (1)
    {
        // 每隔 2 秒发送一次固定字符串
        BLUETOOTH_SendString("Test\r\n");
        Delay_Ms(2000);
    }
}