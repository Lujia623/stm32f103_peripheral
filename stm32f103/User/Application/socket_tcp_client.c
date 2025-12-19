/**
 * @file socket_tcp_client.c
 * @author Rougga (1055700185@qq.com)
 * @brief socket tcp client example based on lwip socket API
 * @version 0.1
 * @date 2025-12-15 22:01:16    +08:00
 * 
 * @copyright Copyright (c) 2023 Rougga(1055700185qq.com). All rights reserved.
 * 
 */

#include "socket_tcp_client.h"

#if APP_EXAMPLES_TCP_CLIENT_SOCKET_ENABLED

#include "lwip/opt.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/inet.h"
#include "elog.h"

static void socket_tcp_client_thread(void *arg)
{
    int sockfd = -1;
    int ret;
    struct sockaddr_in server_addr;
    static const char send_buf[] = "Hello, socket TCP Server!";
    int bytes_sent;

    LWIP_UNUSED_ARG(arg);

    while (1) {
        /* Create a TCP socket */
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            log_d("socket_tcp_client: create socket failed:%d", sockfd);
            sys_msleep(10);
            continue;
        }
    
        /* Configure the server address */
        server_addr.sin_len = sizeof(server_addr);
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(SOCKET_TCP_CLIENT_PORT);
        server_addr.sin_addr.s_addr = inet_addr("192.168.0.100");
        memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));
        ret = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (ret < 0) {
            log_d("socket_tcp_client: connect to server failed:%d", ret);
            close(sockfd);
            sys_msleep(10);
            continue;
        }

        log_i("socket_tcp_client: connect to server success.");

        while (1) {
            bytes_sent = send(sockfd, send_buf, sizeof(send_buf) - 1, 0);
            sys_msleep(1000);
            if (bytes_sent < 0) {
                log_d("socket_tcp_client: send data failed:%d", bytes_sent);
                close(sockfd);
                break;
            }
        }
    }
}

void socket_tcp_client_init(void)
{
    sys_thread_new("socket_tcp_client", socket_tcp_client_thread, NULL, 1024, 5);
}

#endif /* APP_EXAMPLES_TCP_CLIENT_SOCKET_ENABLED */
