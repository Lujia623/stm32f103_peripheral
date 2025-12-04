#include "bsp_systick.h"
#include "error.h"

uint32_t __IO tick;
__IO uint32_t uwTick;
uint32_t uwTickFreq = 1U;
static uint8_t fac_us;
static uint16_t fac_ms;

void Systick_Init(void)
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    SysTick->VAL=0x00;        					// Clear counter
    SysTick->LOAD=0x00ffffff; 					// Set reload value		 
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	// Start countdown
    fac_us = SystemCoreClock / 8000000;
    fac_ms = fac_us * 1000;
}

uint32_t SystickInit(void)
{
	// 1ms
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	if (SysTick_Config(SystemCoreClock / 1000) > 0)
	{
		return RS_ERROR;
	}
	return RS_OK;
}

void Delay(uint32_t Delay)
{
	uint32_t tickstart = tick_get();
	uint32_t wait = Delay;

	if(wait < 0xFFFFFFFF)
	{
		wait += (uint32_t)uwTickFreq;
	}
	while ((tick_get() - tickstart) < wait)
	{
		
	}
	
}

// Delay nus
// nus is the number of microseconds to delay.		    								   
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; 					// Set reload value	  		 
	SysTick->VAL=0x00;        					// Clear counter
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	// Start countdown	  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		// Wait for time to reach   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	// Stop counter
	SysTick->VAL =0X00;      					 // Clear counter	 
}

void delay_ms(u32 nms)
{		
	u32 temp;	    	 
	SysTick->LOAD=nms*fac_ms; 					// Set reload value
	SysTick->VAL=0x00;        					// Clear counter
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	// Start countdown
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		// Wait for time to reach   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	// Stop counter
	SysTick->VAL =0X00;      					 // Clear counter	 
}

void IncTick(void)
{
	uwTick += uwTickFreq;
}

uint32_t HAL_GetTick(void)
{
	return uwTick;
}

void tick_set(uint32_t tick)
{
    
}

uint32_t tick_get(void)
{
    return uwTick;
}
