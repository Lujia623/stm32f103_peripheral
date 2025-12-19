/**
 * @file updecho.c
 * @author Rougga (1055700185@qq.com)
 * @brief upd echo server example based on lwip netconn API
 * @version 0.1
 * @date 2025-12-14 22:37:26    +08:00
 * 
 * @copyright Copyright (c) 2023 Rougga(1055700185qq.com). All rights reserved.
 * 
 */

#include "udpecho.h"

#if APP_EXAMPLES_UDP_ECHO_NETCONN_ENABLED

#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/sys.h"

static void udpecho_thread(void *arg)
{
    struct netconn *conn;
    err_t err;
    struct netbuf *buf;

    LWIP_UNUSED_ARG(arg);

#if LWIP_IPV6
    conn = netconn_new(NETCONN_UDP_IPV6);
#else
    conn = netconn_new(NETCONN_UDP);
#endif
    LWIP_ERROR("udpecho: invalid conn", (conn != NULL), return;);
    err = netconn_bind(conn, NULL, UDPECHO_PORT);
    LWIP_ERROR("udpecho: netconn_bind failed", (err == ERR_OK), return;);

    while (1) {
        err = netconn_recv(conn, &buf);
        if (err == ERR_OK) {
            /* the udp data is noncopy, directly send it back. */
            err = netconn_send(conn, buf);
            if (err != ERR_OK) {
                LWIP_DEBUGF(LWIP_DBG_ON, ("udpecho: netconn_send failed: %d\n", err));
            }
            netbuf_delete(buf);
        } else {
            LWIP_DEBUGF(LWIP_DBG_ON, ("udpecho: netconn_recv failed: %d\n", err));
        }
    }
}


void udpecho_init(void)
{
    sys_thread_new("udpecho", udpecho_thread, NULL, 1024, 5);
}

#endif /* APP_EXAMPLES_UDP_ECHO_NETCONN_ENABLED */
