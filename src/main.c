#include "CH57x_common.h"

#define LED_PIN GPIO_Pin_7

static uint8_t hello_str[] = "HelloWorld\r\n";

__HIGH_CODE
int main(void)
{
    R16_PIN_ALTERNATE &= ~RB_PIN_DEBUG_EN;

    HSECFG_Capacitance(HSECap_18p);
    SetSysClock(CLK_SOURCE_HSE_PLL_100MHz);

    GPIOA_SetBits(LED_PIN);
    GPIOA_ModeCfg(LED_PIN, GPIO_ModeOut_PP_5mA);

    GPIOA_SetBits(GPIO_Pin_2);
    GPIOA_ModeCfg(GPIO_Pin_3, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(GPIO_Pin_2, GPIO_ModeOut_PP_5mA);
    UART_Remap(ENABLE, UART_TX_REMAP_PA2, UART_RX_REMAP_PA3);
    UART_DefInit();

    while(1)
    {
        GPIOA_InverseBits(LED_PIN);
        UART_SendString(hello_str, sizeof(hello_str) - 1);
        DelayMs(1000);
    }
}
