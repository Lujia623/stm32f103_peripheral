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
#include "elog.h"
#include "client.h"
#include "tcpecho.h"
#include "udpecho.h"
#include "socket_tcp_client.h"
#include "socket_tcp_server.h"
#include "socket_udp.h"
#include "iperf.h"
#include "lwip_init.h"
#include "network_manager.h"
#include "http_client_examples.h"
#include "http_server_examples.h"
#include "mqtt_client_examples.h"
#include "SEGGER_RTT.h"

#define LED_PRIORITY    		2
#define tskSTACK_SIZE       (128)
TaskHandle_t xHandle1 = NULL;
TaskHandle_t xHandle2 = NULL;
TaskHandle_t xHandleApp = NULL;

static void app_examples(void);

static void task1(void * pvParameters)
{
    for(;;) {
        PBout(5) = !PBout(5);
        // log_d("tesk1\r\n");
        log_d("task num:%0x\r\n", uxTaskGetNumberOfTasks());
        // SEGGER_RTT_WriteString(0, "task1\r\n");
        SEGGER_RTT_printf(0, "task1\r\n");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

static void task2(void * pvParameters)
{
    for(;;) {
        // PAout(5) = !PAout(5);
        // log_d("tesk2:%d\r\n", __DATE__);
        GPIO_SetBits(GPIOC, GPIO_Pin_14 | GPIO_Pin_15);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        GPIO_ResetBits(GPIOC, GPIO_Pin_14 | GPIO_Pin_15);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

static void monitor_task(void *pvParameters)
{
    static char info_buffer[1024]; 

    while (1) {
        printf("\r\n--- Task Status Statistics ---\r\n");
        printf("TaskName\tStatus\tPrio\tStackLeft\tID\r\n");
        vTaskList(info_buffer);
        printf("%s", info_buffer);

        // printf("\r\n--- CPU Usage Statistics ---\r\n");
        // vTaskGetRunTimeStats(info_buffer);
        // printf("%s", info_buffer);

        vTaskDelay(pdMS_TO_TICKS(5000)); // ÿ5���ӡһ��
    }
}

static void Application(void * pvParameters)
{
    BaseType_t xReturn = pdPASS;

#if !NETWORK_MANAGER_ENABLED
    TCPIP_Init();
#else
    network_manager_init();
#endif /* NETWORK_MANAGER_ENABLED */
    app_examples();
    xReturn = xTaskCreate(task1, "task1", 512, NULL, tskIDLE_PRIORITY, &xHandle1);
    if(xReturn == pdPASS)
        log_d("task1 creat success\r\n");
    xReturn = xTaskCreate(task2, "task2", tskSTACK_SIZE, NULL, LED_PRIORITY, &xHandle2);
    if(xReturn == pdPASS)
        log_d("task1 creat success\r\n");
    if(xReturn == pdPASS)
        log_d("adc_task creat success\r\n");
    // sys_thread_new("monitor", monitor_task, NULL, 512, tskIDLE_PRIORITY);
    vTaskDelete(xHandleApp);
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
		log_d("netif success \r\n");
  }
  else
  {
    /* When the netif link is down this function must be called */
    netif_set_down(&gnetif);
		log_d("netif failed \r\n");
  }
  log_d("local ip addr:%ld.%ld.%ld.%ld\n\n",  \
        ((gnetif.ip_addr.addr)&0x000000ff),       \
        (((gnetif.ip_addr.addr)&0x0000ff00)>>8),  \
        (((gnetif.ip_addr.addr)&0x00ff0000)>>16), \
        ((gnetif.ip_addr.addr)&0xff000000)>>24);
 }
#endif /* NO_SYS */

#define TCP_ECHO_PORT 5001

static void easyLogger_init(void)
{
    elog_init();
    /* set EasyLogger log format */
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
#ifdef ELOG_COLOR_ENABLE
    elog_set_text_color_enabled(true);
#endif
    /* start EasyLogger */
    elog_start();
}

static void io_test(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;

    GPIO_Init(GPIOC, &GPIO_InitStruct);
    log_i("io test");
}

int main(void)
{
//    uint8_t send_data[256];
//    uint32_t i;
    BaseType_t xReturn = pdPASS;
    SystickInit();
    led_init();
    SerialPortInit(BAUD_RATE_115200);
    // spi_Init();
    easyLogger_init();
    log_d("SPI SLAVE\r\n");
    log_d("W5500\r\n");
    io_test();
    RCC_ClocksTypeDef clock;
    RCC_GetClocksFreq(&clock);
    log_d("SYSCLK:%d, HCLK:%d, PCLK1:%d, PCLK2:%d, ADCCLK:%d\n", clock.SYSCLK_Frequency, clock.HCLK_Frequency, clock.PCLK1_Frequency, clock.PCLK2_Frequency, clock.ADCCLK_Frequency);

    xReturn = xTaskCreate(Application, "task1", 512, NULL, tskIDLE_PRIORITY, &xHandleApp);
    if(xReturn == pdPASS)
        vTaskStartScheduler();
    else
        log_d("Application creat fail\r\n");
    while(1);
#if NO_SYS
    while (1)
    {
        //ethernetif_input(&gnetif);
        //sys_check_timeouts();
    }
#endif /* NO_SYS */
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    printf("\r\n[CRITICAL] Stack overflow detected in task: %s !!\r\n", pcTaskName);
    portDISABLE_INTERRUPTS();
    while(1);
}

static void app_examples(void)
{

#if APP_EXAMPLES_TCP_CLIENT_NETCONN_ENABLED
    /* TCP client example based on lwip netconn API */
    client_init();
#endif /* APP_EXAMPLES_TCP_CLIENT_NETCONN_ENABLED */

#if APP_EXAMPLES_TCP_SERVER_NETCONN_ENABLED
    /* TCP server example based on lwip netconn API */
    tcpecho_init();
#endif /* APP_EXAMPLES_TCP_SERVER_NETCONN_ENABLED */

#if APP_EXAMPLES_UDP_ECHO_NETCONN_ENABLED
    /* UDP echo example based on lwip netconn API */
    udpecho_init();
#endif /* APP_EXAMPLES_UDP_ECHO_NETCONN_ENABLED */

#if APP_EXAMPLES_TCP_CLIENT_SOCKET_ENABLED
    /* TCP client example based on lwip socket API */
    socket_tcp_client_init();
#endif

#if APP_EXAMPLES_TCP_SERVER_SOCKET_ENABLED
    /* TCP server example based on lwip socket API */
    socket_tcp_server_init();
#endif /* APP_EXAMPLES_TCP_SERVER_SOCKET_ENABLED */

#if APP_EXAMPLES_UDP_ECHO_SOCKET_ENABLED
    /* UDP echo example based on lwip socket API */
    socket_udp_init();
#endif /* APP_EXAMPLES_UDP_ECHO_SOCKET_ENABLED */

#if APP_EXAMPLES_IPERF_ENABLED
    /* iperf example */
    iperf_init();
#endif /* APP_EXAMPLES_IPERF_ENABLED */

#if APP_EXAMPLES_HTTP_CLIENT_ENABLED
    /* http client example */
    http_client_init();
#endif /* APP_EXAMPLES_HTTP_CLIENT_ENABLED */

#if APP_EXAMPLES_HTTP_SERVER_ENABLED
    /* http server example */
    http_server_init();
#endif /* APP_EXAMPLES_HTTP_SERVER_ENABLED */

    mqtt_client_examples_init();
    log_i("app examples init complete.");
}

