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
    SysTick->VAL=0x00;        					//清空计数器
    SysTick->LOAD=0x00ffffff; 					//时间加载		 
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数
    fac_us = SystemCoreClock / 8000000;
    fac_ms = fac_us * 1000;
}

uint32_t SystickInit(void)
{
	// 1ms
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

//延时nus
//nus为要延时的us数.		    								   
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; 					//时间加载	  		 
	SysTick->VAL=0x00;        					//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数	  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL =0X00;      					 //清空计数器	 
}

void delay_ms(u32 nms)
{		
	u32 temp;	    	 
	SysTick->LOAD=nms*fac_ms; 					//时间加载	  		 
	SysTick->VAL=0x00;        					//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数	  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL =0X00;      					 //清空计数器	 
}

void IncTick(void)
{
	uwTick += uwTickFreq;
}

void tick_set(uint32_t tick)
{
    
}

uint32_t tick_get(void)
{
    return uwTick;
}
