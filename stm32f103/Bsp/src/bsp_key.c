#include "bsp_key.h"
#include "led.h"

void key_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);//要先开时钟，不然配置不成功

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
    // GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;

    GPIO_Init(GPIOE, &GPIO_InitStruct);
}

KEY_STATE Scan_key(void)
{
    static uint8_t is_btn_realse = 1;
    if (is_btn_realse && GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) == KEY_OFF)
    {
        while (is_btn_realse && (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) == KEY_OFF));
        is_btn_realse = 0;
        return KEY_OFF;
    }
    else
        return KEY_ON;
}
