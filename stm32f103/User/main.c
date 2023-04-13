#include "stm32f10x.h"
#include "led.h"
#include "bsp_key.h"
#include "bsp_system.h"
#include "bsp_serial.h"
#include "bsp_usart.h"
#include "bsp_systick.h"
#include "FreeRTOS.h"
#include "task.h"
#include "spi.h"
#include "bsp_eth.h"
#include "main.h"
//#include <lwip/opt.h>
//#include <lwip/arch.h>
//#include "tcpip.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "ethernetif.h"
#include "netif/ethernet.h"
//#include "lwip/def.h"
//#include "lwip/stats.h"
//#include "lwip/etharp.h"
//#include "lwip/ip.h"
//#include "lwip/snmp.h"
#include "lwip/timeouts.h"
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
#if 0
#define LED_PRIORITY    		2
#define tskSTACK_SIZE       (128)
TaskHandle_t xHandle1 = NULL;
TaskHandle_t xHandle2 = NULL;

static void task1(void * pvParameters)
{
    for(;;) {
        PBout(5) = !PBout(5);
        printf("tesk1\r\n");
        vTaskDelay(20);
    }
}

static void task2(void * pvParameters)
{
    for(;;) {
        PAout(5) = !PAout(5);
        printf("tesk2\r\n");
        vTaskDelay(20);
    }
}
#endif

 

#if NO_SYS
struct netif gnetif;
 ip4_addr_t ipaddr;
 ip4_addr_t netmask;
 ip4_addr_t gw;
 void LwIP_Init(void)
 {
  /* IP addresses initialization */
  /* USER CODE BEGIN 0 */
  IP4_ADDR(&ipaddr,IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
  IP4_ADDR(&netmask,NETMASK_ADDR0,NETMASK_ADDR1,NETMASK_ADDR2,NETMASK_ADDR3);
  IP4_ADDR(&gw,GW_ADDR0,GW_ADDR1,GW_ADDR2,GW_ADDR3);
 
  /* USER CODE END 0 */
   
  /* Initilialize the LwIP stack without RTOS */
  lwip_init();
 
  /* add the network interface (IPv4/IPv6) without RTOS */
  netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

  /* Registers the default network interface */
  netif_set_default(&gnetif);

  if (netif_is_link_up(&gnetif))
  {
    /* When the netif is fully configured this function must be called */
    netif_set_up(&gnetif);
		printf("netif success \r\n");
  }
  else
  {
    /* When the netif link is down this function must be called */
    netif_set_down(&gnetif);
		printf("netif failed \r\n");
  }
  printf("本地IP地址是:%ld.%ld.%ld.%ld\n\n",  \
        ((gnetif.ip_addr.addr)&0x000000ff),       \
        (((gnetif.ip_addr.addr)&0x0000ff00)>>8),  \
        (((gnetif.ip_addr.addr)&0x00ff0000)>>16), \
        ((gnetif.ip_addr.addr)&0xff000000)>>24);
 }
#endif /* NO_SYS */
void LwIP_Init(void)
{
    
}

#define TCP_ECHO_PORT 5001


int main(void)
{
//    uint8_t send_data[256];
//    uint32_t i;
    SystickInit();
    led_init();
    SerialPortInit(BAUD_RATE_115200);
    // spi_Init();
    printf("SPI SLAVE\r\n");
    printf("W5500\r\n");
    LwIP_Init();

    while (1)
    {
        //ethernetif_input(&gnetif);
        //sys_check_timeouts();
    }

    
    // while(1) {
    //     recv_display();
    // }
    // xTaskCreate(task1, "task1", tskSTACK_SIZE, NULL, tskIDLE_PRIORITY, &xHandle1);
    // xTaskCreate(task2, "task2", tskSTACK_SIZE, NULL, LED_PRIORITY, &xHandle2);
    // //configASSERT(xHandle1);
    // configASSERT(xHandle2);
    // vTaskStartScheduler();
}

