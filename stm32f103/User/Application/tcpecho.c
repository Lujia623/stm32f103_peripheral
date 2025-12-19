/**
 * @file tcpecho.c
 * @author Rougga (1055700185@qq.com)
 * @brief TCP echo server
 * @version 0.1
 * @date 2025-12-11 23:02:12    +08:00
 * 
 * @copyright Copyright (c) 2023 Rougga(1055700185qq.com). All rights reserved.
 * 
 */

#include "tcpecho.h"

#if APP_EXAMPLES_TCP_SERVER_NETCONN_ENABLED

#include "lwip/opt.h"

#if LWIP_NETCONN
#include "lwip/api.h"
#include "lwip/sys.h"
#include "elog.h"

#if TCP_SERVER_MULCONN_ENABLE
static struct netconn *netconn_list[MAX_CONNECTIONS];
static uint8_t netconn_count = 0;
#endif /* TCP_SERVER_MULCONN_ENABLE */

static void tcpecho_thread(void *arg)
{
    struct netconn *conn, *newconn;
    err_t err;
    LWIP_UNUSED_ARG(arg);

    /* Create a new TCP connection handle */
#if LWIP_IPV6
    conn = netconn_new(NETCONN_TCP_IPV6);
    netconn_bind(conn, IP6_ADDR_ANY, TCPECHO_PORT);
#else /* LWIP_IPV6 */
    conn = netconn_new(NETCONN_TCP);
    netconn_bind(conn, IP_ADDR_ANY, TCPECHO_PORT);
#endif /* LWIP_IPV6 */

#if !TCP_SERVER_MULCONN_ENABLE
    /* Tell connection to go into listening mode */
    netconn_listen(conn);

    while (1) {
        /* Grab new connection */
        err = netconn_accept(conn, &newconn);
        if (err == ERR_OK) {
            struct netbuf *buf;
            void *data;
            u16_t len;

            /* Receive data until the connection is closed */
            while ((err = netconn_recv(newconn, &buf)) == ERR_OK) {
                do {
                    netbuf_data(buf, &data, &len);
                    netconn_write(newconn, data, len, NETCONN_NOCOPY);
                } while (netbuf_next(buf) >= 0);
                netbuf_delete(buf);
            }
            /* Close connection and discard connection identifier */
            netconn_close(newconn);
            netconn_delete(newconn);
        }
    }
#else /* TCP_SERVER_MULCONN_ENABLE */

    netconn_set_nonblocking(conn, 1);
    netconn_listen(conn);

    while (1) {
        u8_t has_data = 0;
        err = netconn_accept(conn, &newconn);
        if (err == ERR_OK) {
            if (netconn_count < MAX_CONNECTIONS) {
                /* set the TCP client to non-blocking mode */
                netconn_set_nonblocking(newconn, 1);
                netconn_list[netconn_count++] = newconn;
                has_data = 1;
                log_i("New client connected. Total: %d", netconn_count);
            } else {
                netconn_close(newconn);
                netconn_delete(newconn);
                log_w("Max connections reached. Rejected.");
            }
        }

        for (uint8_t i = 0; i < netconn_count; i++) {
            /* if the connection is closed, delete it */
            if (netconn_list[i] == NULL) {
                continue;
            }
            struct netconn *client = netconn_list[i];
            struct netbuf *buf;
            void *data;
            u16_t len;

            /* Receive data until the connection is closed */
            err = netconn_recv(client, &buf);
            if (err == ERR_OK) {
                has_data = 1;
                do {
                    netbuf_data(buf, &data, &len);
                    size_t bytes_written = 0;
                    err_t write_err = netconn_write_partly(client, data, len, NETCONN_NOCOPY, &bytes_written);
                    if (write_err != ERR_OK) {
                         log_e("Echo write failed: %d", write_err);
                    } else {
                         // log_d("Echoed %d bytes", len);
                    }
                } while (netbuf_next(buf) >= 0);
                netbuf_delete(buf);
            } else if (err == ERR_CLSD || err == ERR_RST || err == ERR_ABRT || err == ERR_CONN) {
                log_i("Connection %d closed.", i);
                netconn_close(client);
                netconn_delete(client);
                netconn_list[i] = NULL;
                for (uint8_t j = i; j < netconn_count - 1; j++) {
                    netconn_list[j] = netconn_list[j + 1];
                }
                netconn_count--;
                /* reset i */
                i--;
            } else if (err != ERR_WOULDBLOCK) {
                log_e("tcp recv error: %d", err);
                /* also close on other fatal errors to avoid stuck connections */
                netconn_close(client);
                netconn_delete(client);
                netconn_list[i] = NULL;
                for (uint8_t j = i; j < netconn_count - 1; j++) {
                    netconn_list[j] = netconn_list[j + 1];
                }
                netconn_count--;
                i--;
            }
        }

        /* if there is no data, sleep 10ms */
        if (!has_data) {
            sys_msleep(10);
        }
    }

#endif /* TCP_SERVER_MULCONN_ENABLE */
}

void tcpecho_init(void)
{
  sys_thread_new("tcpecho_thread", tcpecho_thread, NULL, 1024, 4);
}

#endif /* LWIP_NETCONN */

#endif /* APP_EXAMPLES_TCP_SERVER_NETCONN_ENABLED */
