/**
 * @file socket_tcp_server.c
 * @author Rougga (1055700185@qq.com)
 * @brief socket tcp server used socket api
 * @version 0.1
 * @date 2025-12-16 22:06:32    +08:00
 * 
 * @copyright Copyright (c) 2023 Rougga(1055700185qq.com). All rights reserved.
 * 
 */

#include "socket_tcp_server.h"

#if APP_EXAMPLES_TCP_SERVER_SOCKET_ENABLED

#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"

#define TCP_SERVER_RECV_BUFFER_SIZE    1024

static void socket_tcp_server_thread(void *arg)
{
    int socket_fd = -1, client_fd = -1;
    uint8_t *recv_buffer = NULL;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int len, flag;
    int ret;

    LWIP_UNUSED_ARG(arg);

    /* malloc recv buffer */
    recv_buffer = (uint8_t *)pvPortMalloc(TCP_SERVER_RECV_BUFFER_SIZE);
    if (recv_buffer == NULL) {
        log_e("socket_tcp_server: malloc recv buffer failed.");
        goto __exit;
    }
    /* Create a TCP socket */
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        log_e("socket_tcp_server: create socket failed.");
        goto __exit;
    }
    /* Configure the server address */
    server_addr.sin_len = sizeof(server_addr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SOCKET_TCP_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));
    /* Bind the socket to the specified port */
    ret = bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        log_e("socket_tcp_server: bind socket failed %d.", ret);
        goto __exit;
    }

    /* Listen for incoming connections */
    ret = listen(socket_fd, 5);
    if (ret < 0) {
        log_e("socket_tcp_server: listen socket failed %d.", ret);
        goto __exit;
    }

    log_i("socket_tcp_server: waiting for connections...");
    while (1) {
        /* Accept a new client connection */
        client_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_fd < 0) {
            log_e("socket_tcp_server: accept client failed %d.", client_fd);
            sys_msleep(10);
            continue;
        }
        log_i("socket_tcp_server: (%s, %d)client connected.", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, (const char *)&flag, sizeof(flag));

        /* Receive data from the client */
        while (1) {
            len = recv(client_fd, recv_buffer, TCP_SERVER_RECV_BUFFER_SIZE, 0);
            if (len < 0) {
                log_e("socket_tcp_server: recv data failed %d.", len);
                break;
            } else if (len == 0) {
                log_i("socket_tcp_server: client disconnected.");
                break;
            } else {
                /* Echo the received data back to the client */
                ret = send(client_fd, recv_buffer, len, 0);
                if (ret < 0) {
                    log_e("socket_tcp_server: send data failed %d.", ret);
                    break;
                }
            }
        }

        /* Close the client connection */
        close(client_fd);
        client_fd = -1;
    }


__exit:
    if (socket_fd >= 0) {
        close(socket_fd);
    }
    if (recv_buffer) {
        vPortFree(recv_buffer);
    }
    vTaskDelete(NULL);
}

void socket_tcp_server_init(void)
{
    sys_thread_new("socket_tcp_server", socket_tcp_server_thread, NULL, 1024, 5);
}

#endif /* APP_EXAMPLES_TCP_SERVER_SOCKET_ENABLED */
