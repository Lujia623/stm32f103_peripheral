/**
 * @file lwip_init.h
 * @author Rougga (1055700185@qq.com)
 * @brief netif initialization header file
 * @version 0.1
 * @date 2025-12-19 22:26:55    +08:00
 * 
 * @copyright Copyright (c) 2023 Rougga(1055700185qq.com). All rights reserved.
 * 
 */

#ifndef LWIP_INIT_H
#define LWIP_INIT_H 

#ifdef __cplusplus
extern "C" {
#endif

/* USER CODE BEGIN 0 */
#define DEST_IP_ADDR0               192
#define DEST_IP_ADDR1               168
#define DEST_IP_ADDR2                 0
#define DEST_IP_ADDR3               100

#define DEST_PORT                  5001

#define UDP_SERVER_PORT            5002   /* define the UDP local connection port */
#define UDP_CLIENT_PORT            5002   /* define the UDP remote connection port */

#define LOCAL_PORT                 5001

/*Static IP ADDRESS: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
#define IP_ADDR0                    192
#define IP_ADDR1                    168
#define IP_ADDR2                      0
#define IP_ADDR3                    122

/*NETMASK*/
#define NETMASK_ADDR0               255
#define NETMASK_ADDR1               255
#define NETMASK_ADDR2               255
#define NETMASK_ADDR3                 0

/*Gateway Address*/
#define GW_ADDR0                    192
#define GW_ADDR1                    168
#define GW_ADDR2                      0
#define GW_ADDR3                      1
/* USER CODE END 0 */


/**
 * @brief init lwip stack and netif
 * 
 */
void TCPIP_Init(void);

#ifdef __cplusplus
}
#endif
#endif /* LWIP_INIT_H */
