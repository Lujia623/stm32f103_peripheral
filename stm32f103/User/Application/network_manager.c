/**
 * @file network_manager.c
 * @author Rougga (1055700185@qq.com)
 * @brief Provide unified management of network status.
 * @version 0.1
 * @date 2025-12-21 15:33:20    +08:00
 *
 * @copyright Copyright (c) 2023 Rougga(1055700185qq.com). All rights reserved.
 *
 */

#include "network_manager.h"

#if NETWORK_MANAGER_ENABLED

#include "tcpip.h"
#include "ethernetif.h"
#include "elog.h"

int errno;
static struct netif gnetif;
static QueueHandle_t netif_status_queue;
static TaskHandle_t net_mgr_handle;

static void netif_link_callback(struct netif *netif)
{
    net_status_info_t info;
    if (netif_is_link_up(netif)) {
        info.status = NET_STATUS_LINK_UP;
    } else {
        info.status = NET_STATUS_LINK_DOWN;
    }
    info.netif = netif;
    xQueueSend(netif_status_queue, &info, 0);
}

static void netif_status_callback(struct netif *netif)
{
    net_status_info_t info;
    info.netif = netif;
    if (netif_is_up(netif) && !ip_addr_isany_val(*netif_ip4_addr(netif))) {
        info.status = NET_STATUS_CONNECTED;
    } else {
        info.status = NET_STATUS_DISCONNECTED;
    }
    xQueueSend(netif_status_queue, &info, 0);
}

static void network_manager_task(void *arg)
{
    net_status_info_t status_info;

    LWIP_UNUSED_ARG(arg);

    while (1) {
        if (xQueueReceive(netif_status_queue, &status_info, portMAX_DELAY) == pdTRUE) {
            switch (status_info.status) {
                case NET_STATUS_LINK_UP:
                    netifapi_netif_set_up(status_info.netif);
                    log_i("Network link is up");
                    #if LWIP_DHCP
                    netifapi_dhcp_start(status_info.netif);
                    log_i("DHCP Start...");
                    #endif /* LWIP_DHCP */
                    break;
                case NET_STATUS_LINK_DOWN:
                    /* stop dhcp first and set link down */
                    #if LWIP_DHCP
                    /*  Stop DHCP */
                    netifapi_dhcp_stop(status_info.netif);
                    log_i("DHCP Stop.");
                    #endif /* LWIP_DHCP */
                    log_i("Network link is down");
                    netifapi_netif_set_down(status_info.netif);
                    /* clear ip */
                    ip_addr_set_zero_ip4(&status_info.netif->ip_addr);
                    ip_addr_set_zero_ip4(&status_info.netif->netmask);
                    ip_addr_set_zero_ip4(&status_info.netif->gw);
                    break;
                case NET_STATUS_CONNECTED:
                    log_i("Network is connected");
                    log_i("IP: %s", ip4addr_ntoa(netif_ip4_addr(status_info.netif)));
                    log_i("MASK: %s", ip4addr_ntoa(netif_ip4_netmask(status_info.netif)));
                    log_i("GW: %s", ip4addr_ntoa(netif_ip4_gw(status_info.netif)));
                    break;
                case NET_STATUS_DISCONNECTED:
                    log_i("Network is disconnected");
                    break;
                default:
                    log_i("Network status unknown");
                    break;
            }
        }
    }
}

int network_manager_init(void)
{
    ip4_addr_t ipaddr, netmask, gw;

    tcpip_init(NULL, NULL);

    /* IP addresses initialization */
    /* USER CODE BEGIN 0 */
#if LWIP_DHCP
    ip_addr_set_zero_ip4(&ipaddr);
    ip_addr_set_zero_ip4(&netmask);
    ip_addr_set_zero_ip4(&gw);
#else
    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
#endif /* USE_DHCP */

    /* if in RTOS, must call netifapi_netif_add is safe */
    netifapi_netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

    /*  set netif */
    netifapi_netif_set_default(&gnetif);

    netif_set_link_callback(&gnetif, netif_link_callback);
    netif_set_status_callback(&gnetif, netif_status_callback);

    if (netif_is_link_up(&gnetif)) {
        netifapi_netif_set_up(&gnetif);
    #if LWIP_DHCP
        netifapi_dhcp_start(&gnetif);
    #endif /* LWIP_DHCP */
    }

    netif_status_queue = xQueueCreate(NET_QUEUE_SIZE, sizeof(net_status_info_t));
    xTaskCreate(network_manager_task, "net_mgr", 512, NULL, tskIDLE_PRIORITY + 3, &net_mgr_handle);

    if (netif_status_queue == NULL || net_mgr_handle == NULL) {
        log_e("Network manager init failed");
        return -1;
    }

    return 0;
}

#endif /* NETWORK_MANAGER_ENABLED */
