#ifndef __LED_H_
#define __LED_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f10x.h"

    typedef struct
    {
        /* data */
			int a;
    }sLed_TypeDef;


    void led_init(void);
    void led_Display(void);
    void delay(uint32_t i);

#ifdef __cplusplus
}
#endif

#endif /*__LED_H_*/
