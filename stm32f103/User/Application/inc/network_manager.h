/**
 * @file network_maneger.h
 * @author Rougga (1055700185@qq.com)
 * @brief Provide unified management of network status.
 * @version 0.1
 * @date 2025-12-21 15:32:37    +08:00
 * 
 * @copyright Copyright (c) 2023 Rougga(1055700185qq.com). All rights reserved.
 * 
 */

#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include "app_examples_config.h"

#if NETWORK_MANAGER_ENABLED

#include "lwip/opt.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/netifapi.h"
#include "lwip/dhcp.h"

#ifdef __cplusplus
extern "C" {
#endif

/* IP Address */
#define IP_ADDR0                    192
#define IP_ADDR1                    168
#define IP_ADDR2                      0
#define IP_ADDR3                    122

/* NETMASK Address */
#define NETMASK_ADDR0               255
#define NETMASK_ADDR1               255
#define NETMASK_ADDR2               255
#define NETMASK_ADDR3                 0

/* Gateway Address */
#define GW_ADDR0                    192
#define GW_ADDR1                    168
#define GW_ADDR2                      0
#define GW_ADDR3                      1

#define NET_QUEUE_SIZE               32

typedef enum {
    NET_STATUS_UNKNOWN = 0,
    NET_STATUS_LINK_DOWN,
    NET_STATUS_LINK_UP,
    NET_STATUS_CONNECTED,
    NET_STATUS_DISCONNECTED,
    NET_STATUS_MAX
} net_status_t;

typedef struct {
    net_status_t status;
    struct netif *netif;
} net_status_info_t;

typedef void (*net_status_callback_t)(net_status_t status, struct netif *netif);

/**
 * @brief Network manager initialization
 * 
 * @return int 
 */
int network_manager_init(void);

#ifdef __cplusplus
}
#endif
#endif /* NETWORK_MANAGER_H */
#endif /* NETWORK_MANAGER_H */
