#include "stm32f10x.h"
#include "led.h"
#include "bsp_key.h"
#include "bsp_system.h"
#include "bsp_serial.h"
#include "bsp_usart.h"
#include "bsp_systick.h"
#include "FreeRTOS.h"
#include "task.h"
#if 0
 // uint8_t flag = 0;
    uint32_t e;
    uint8_t error;
	int a;
	char b;
		long c;
	long long d;
	double e1;
	float f;
	float ff=1;

    SerialPortInit(BAUD_RATE_115200);
    Common_init(COMMON_USART1);
    Systick_Init();

    RCC->APB2ENR |= 0x0048;
    // RCC->APB2ENR
    GPIOB->LCKR |= 0x00010000;
    GPIOB->LCKR &= 0x0000FFFF;
    GPIOB->LCKR |= 0x00010000;
    e = GPIOB->LCKR;
    e = GPIOB->LCKR;
    // GPIOB->LCKR &= 0x0000FFFF;
    GPIOB->LCKR = (1<<5);
	e = GPIOB->LCKR;
	printf("e:%d\r\n",e);

    GPIOB->CRL &= 0xFF0FFFFF;
    GPIOB->CRL |= 0x00300000;
    GPIOE->CRL &= 0xFF0FFFFF;
    GPIOE->CRL |= 0x00300000;
    // led_init();
    // key_init();
    // led_Display();
    uint8_t ch = 0;
	printf("Hello World\r\n");
    GPIOB->BRR = (1<<5);
    GPIOE->BRR = (1<<5);
    uint8_t data[] = {65,66,67,68,69};
    Common_put_Tx_data(COMMON_USART1, data, sizeof(data)/sizeof(data[0]));
    rt_kprintf("demo\n");
    rt_kprintf("demo\n,%s","demo_2");
		rt_kprintf("%f",1.2345f);
		rt_kprintf("%d,%d,%d,%d,%d\n",sizeof(a),sizeof(b),sizeof(c),sizeof(d),sizeof(e1),sizeof(f));
		
		rt_kprintf("%d\n",sizeof(ff));
		rt_kprintf("%s\n",__FUNCTION__);
    // Send_Byte(USART1,Common_get_Tx_char(COMMON_USART1, &error));
    // Send_Byte(USART1,Common_get_Tx_char(COMMON_USART1, &error));
    // Send_Byte(USART1,Common_get_Tx_char(COMMON_USART1, &error));
    // Send_Byte(USART1,Common_get_Tx_char(COMMON_USART1, &error));
    // Send_Byte(USART1,Common_get_Tx_char(COMMON_USART1, &error));
    // Send_Byte(USART1,Common_get_Tx_char(COMMON_USART1, &error));
    Common_Send_data(COMMON_USART1, &error);
    // printf("%d",Common_get_Tx_char(COMMON_USART1, &error));
    while (1)
    {
        // ch = getchar();
        // printf("ch=%c\n",ch);
        switch (ch)
        {
        case '1':
            printf("IN\n");
				// Usart_SendByte(USART1, 0x65);
				USART_SendData(USART1, 65);
                while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    //             USART_SendData(USART1,65);
		
	// /* 等待发送数据寄存器为空 */
	// while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	
                
            PBout(5)=!PBout(5);
            PEout(5)=!PEout(5);
            //printf("IDR:%0x\n",(uint16_t)(GPIOB->IDR&0xffff));
            break;
        
        default:
            break;
        }
        rt_kprintf("SysTick->LOAD:%d,SysTick->VAL:%d\n",SysTick->LOAD,tick_get());
        // delay_us(1000);
        PBout(5)=!PBout(5);
        rt_kprintf("SysTick->LOAD:%d,SysTick->VAL:%d\n",SysTick->LOAD,tick_get());
        // if (Scan_key() == KEY_OFF && !flag)
        // {
        //     // GPIOE->ODR ^= GPIO_Pin_5;
        //     // GPIOB->ODR ^= GPIO_Pin_5;
        //     PBout(5)=!PBout(5);
        //     PEout(5)=!PEout(5);
        //     flag = 1;
        // }
        // else
        //     flag = 0;
        // delay(0xffff);
    }
#endif
#define tskIDLE_PRIORITY    (portPRIVILEGE_BIT + 1)
#define tskSTACK_SIZE       (128)
TaskHandle_t xHandle1 = NULL;
TaskHandle_t xHandle2 = NULL;

static void task1(void * pvParameters)
{
    for(;;) {
        PBout(5) = !PBout(5);
        vTaskDelay(200);
    }
}

static void task2(void * pvParameters)
{
    for(;;) {
        PAout(5) = !PAout(5);
        vTaskDelay(200);
    }
}
int main(void)
{
    xTaskCreate(task1, "task1", tskSTACK_SIZE, NULL, tskIDLE_PRIORITY, &xHandle1);
    xTaskCreate(task2, "task2", tskSTACK_SIZE, NULL, tskIDLE_PRIORITY, &xHandle2);
    configASSERT(xHandle1);
    configASSERT(xHandle2);
    vTaskStartScheduler();
}

