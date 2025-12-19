/**
 * @file socket_udp.c
 * @author Rougga (1055700185@qq.com)
 * @brief socket udp example based on lwip socket API
 * @version 0.1
 * @date 2025-12-16 23:03:32    +08:00
 * 
 * @copyright Copyright (c) 2023 Rougga(1055700185qq.com). All rights reserved.
 * 
 */

#include "socket_udp.h"

#if APP_EXAMPLES_UDP_ECHO_SOCKET_ENABLED

#include "lwip/opt.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "elog.h"

static void socket_udp_thread(void *pvParameters)
{
    int sock = -1;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    char *recv_buffer = NULL;
    int recv_len = 0;
    int ret;

    LWIP_UNUSED_ARG(pvParameters);

    /* malloc recv buffer */
    recv_buffer = (char *)pvPortMalloc(SOCKET_UDP_RECV_BUFFER_SIZE);
    if (recv_buffer == NULL) {
        log_e("socket_udp: malloc recv buffer failed.");
        goto __exit;
    }

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        log_e("socket_udp: create socket failed.");
        goto __exit;
    }

    /* Configure the server address */
    server_addr.sin_len = sizeof(server_addr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SOCKET_UDP_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

    ret = bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        log_e("socket_udp: bind socket failed %d.", ret);
        goto __exit;
    }

    log_i("socket_udp: UDP server is up and listening on port %d.", SOCKET_UDP_PORT);
    while (1) {
        recv_len = recvfrom(sock, recv_buffer, SOCKET_UDP_RECV_BUFFER_SIZE, 0,
                            (struct sockaddr *)&client_addr, &client_len);
        if (recv_len < 0) {
            log_e("socket_udp: recvfrom failed %d.", recv_len);
            continue;
        }
        log_i("socket_udp: received %d bytes from (%s, %d).", recv_len,
              inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        /* Echo the received data back to the client */
        ret = sendto(sock, recv_buffer, recv_len, 0,
                     (struct sockaddr *)&client_addr, client_len);
        if (ret < 0) {
            log_e("socket_udp: sendto failed %d.", ret);
            continue;
        }
    }

__exit:
    if (sock >= 0) {
        close(sock);
    }
    if (recv_buffer != NULL) {
        vPortFree(recv_buffer);
    }
    vTaskDelete(NULL);
}

void socket_udp_init(void)
{
    sys_thread_new("socket_udp", socket_udp_thread, NULL, 1024, 5);
}
#endif /* APP_EXAMPLES_UDP_ECHO_SOCKET_ENABLED */
