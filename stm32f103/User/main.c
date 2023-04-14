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

#define LED_PRIORITY    		2
#define tskSTACK_SIZE       (128)
TaskHandle_t xHandle1 = NULL;
TaskHandle_t xHandle2 = NULL;
TaskHandle_t xHandleApp = NULL;

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

static void Application(void * pvParameters)
{
    BaseType_t xReturn = pdPASS;

    TCPIP_Init();
    taskENTER_CRITICAL();
    xTaskCreate(task1, "task1", tskSTACK_SIZE, NULL, tskIDLE_PRIORITY, &xHandle1);
    if(xReturn == pdPASS)
        printf("task1 creat success\r\n");
    xTaskCreate(task2, "task2", tskSTACK_SIZE, NULL, LED_PRIORITY, &xHandle2);
    if(xReturn == pdPASS)
        printf("task1 creat success\r\n");
    vTaskDelete(xHandleApp);
    taskENTER_CRITICAL();
}

 

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

#define TCP_ECHO_PORT 5001


int main(void)
{
//    uint8_t send_data[256];
//    uint32_t i;
    BaseType_t xReturn = pdPASS;
    SystickInit();
    led_init();
    SerialPortInit(BAUD_RATE_115200);
    // spi_Init();
    printf("SPI SLAVE\r\n");
    printf("W5500\r\n");

    xReturn = xTaskCreate(Application, "task1", 512, NULL, tskIDLE_PRIORITY, &xHandleApp);
    if(xReturn == pdPASS)
        vTaskStartScheduler();
    else
        printf("Application creat fail\r\n");
    while(1);
#if NO_SYS
    while (1)
    {
        //ethernetif_input(&gnetif);
        //sys_check_timeouts();
    }
#endif /* NO_SYS */
}

