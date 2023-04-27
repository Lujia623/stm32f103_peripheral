#ifndef __BSP_SYSTICK_H__
#define __BSP_SYSTICK_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f10x.h"

#define USE_SYSTICK_CLKCOURCE_AHB

void Systick_Init(void);
uint32_t SystickInit(void);
void IncTick(void);
uint32_t HAL_GetTick(void);
void Delay(uint32_t Delay);
void delay_us(u32 nus);
void delay_ms(u32 nms);
uint32_t tick_get(void);
#ifdef __cplusplus
}
#endif
#endif /*__BSP_SYSTICK_H__*/
