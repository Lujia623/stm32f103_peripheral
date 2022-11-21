#ifndef __BSP_SYSTICK_H__
#define __BSP_SYSTICK_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f10x.h"

#define USE_SYSTICK_CLKCOURCE_AHB

void Systick_Init(void);
void delay_us(u32 nus);
uint32_t tick_get(void);
#ifdef __cplusplus
}
#endif
#endif /*__BSP_SYSTICK_H__*/
