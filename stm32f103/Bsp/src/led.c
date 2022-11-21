#include "led.h"

void led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE, ENABLE); //要先开时钟，不然配置不成功

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;

    GPIO_Init(GPIOE, &GPIO_InitStruct);
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIOE->ODR = GPIO_Pin_5;
    GPIOB->ODR = GPIO_Pin_5;
}

void delay(uint32_t i)
{
    while (i--)
    {
        /* code */
    }
}

void led_Display(void)
{
    uint32_t i = 100;

    delay(0xFFFF);
    GPIOE->ODR &= ~GPIO_Pin_5;
    while (i--)
    {
        delay(0xFFFFF);
        GPIOB->ODR &= ~GPIO_Pin_5;
        delay(0xFFFFF);
        GPIOB->ODR = GPIO_Pin_5;
    }
}
