/**
 * @file iperf.c
 * @author Rougga (1055700185@qq.com)
 * @brief iperf test examples
 * @version 0.1
 * @date 2025-12-17 22:30:52    +08:00
 * 
 * @copyright Copyright (c) 2023 Rougga(1055700185qq.com). All rights reserved.
 * 
 */


#include "iperf.h"

#if APP_EXAMPLES_IPERF_ENABLED

#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "elog.h"


#if APP_EXAMPLES_IPERF_CLIENT_NETCONN_ENABLED
static void iperf_client_thread(void *arg)
{
    struct netconn *conn = NULL;
    int ret;
    char *send_buffer = NULL;
    size_t send_total_len = 0;
    ip_addr_t g_iperf_server_ip;
    u32_t tick1, tick2;


    LWIP_UNUSED_ARG(arg);

    send_buffer = pvPortMalloc(IPERF_BUFFER_SIZE);
    if (send_buffer == NULL) {
        log_e("malloc send buffer failed");
        goto exit;
    }

    for (size_t i = 0; i < IPERF_BUFFER_SIZE; i++) {
        send_buffer[i] = (char)(i % 256);
    }

    IP4_ADDR(&g_iperf_server_ip, IPV4_DST_ADDR0, IPV4_DST_ADDR1, IPV4_DST_ADDR2, IPV4_DST_ADDR3); // set your iperf server ip address

    while (1) {
        conn = netconn_new(NETCONN_TCP);
        if (conn == NULL) {
            log_e("new netconn failed");
            sys_msleep(10);
            continue;
        }

        ret = netconn_connect(conn, (ip_addr_t *)&g_iperf_server_ip, IPV4_DST_PORT);
        if (ret != ERR_OK) {
            log_e("connect to server failed");
            netconn_delete(conn);
            sys_msleep(10);
            continue;
        }

        log_i("connect to server success");

        tick1 = sys_now();
        while (1) {
            tick2 = sys_now();
            if (tick2 - tick1 >= configTICK_RATE_HZ * 5) {
                float speed = ((float)send_total_len * 8.0f) / ((float)(tick2 - tick1) / 1000.0f) / 1000000.0f;
                log_i("send total len: %d, speed: %.2f MB/s", send_total_len, speed);
                tick1 = tick2;
                send_total_len = 0;
            }
            ret = netconn_write(conn, send_buffer, IPERF_BUFFER_SIZE, NETCONN_COPY);
            if (ret == ERR_OK) {
                send_total_len += IPERF_BUFFER_SIZE;
            } else {
                netconn_close(conn);
                netconn_delete(conn);
                log_e("write to server failed");
                sys_msleep(10);
                break;
            }
        }
    }

exit:
    if (send_buffer) {
        vPortFree(send_buffer);
    }
}
#endif /* APP_EXAMPLES_IPERF_CLIENT_NETCONN_ENABLED */

#if APP_EXAMPLES_IPERF_SERVER_NETCONN_ENABLED
static void iperf_server_thread(void *arg)
{
    struct netconn *conn = NULL, *new_conn = NULL;
    err_t err = ERR_OK;

    LWIP_UNUSED_ARG(arg);

    conn = netconn_new(NETCONN_TCP);
    if (conn == NULL) {
        log_e("new netconn failed");
        netconn_close(conn);
        goto exit;
    }

    err = netconn_bind(conn, NULL, IPV4_LISTEN_PORT);
    if (err != ERR_OK) {
        log_e("netconn bind failed");
        netconn_close(conn);
        goto exit;
    }

    err = netconn_listen(conn);
    if (err != ERR_OK) {
        log_e("netconn listen failed");
        netconn_close(conn);
        goto exit;
    }

    while (1) {
        err = netconn_accept(conn, &new_conn);
        if (err == ERR_OK) {
            log_i("new client connected.");
            struct netbuf *buf = NULL;
            while (netconn_recv(new_conn, &buf) == ERR_OK) {
                /* do nothing. */
                netbuf_delete(buf);
                buf = NULL;
            }
            netconn_close(new_conn);
            netconn_delete(new_conn);
            new_conn = NULL;
        } else {
            sys_msleep(10);
        }
    }

exit:
    if (conn) {
        netconn_delete(conn);
    }
    vTaskDelete(NULL);
}
#endif /* APP_EXAMPLES_IPERF_SERVER_NETCONN_ENABLED */

#if APP_EXAMPLES_IPERF_CLIENT_SOCKET_ENABLED
static void iperf_client_socket_thread(void *arg)
{
    int sockfd = -1;
    struct sockaddr_in client_addr = {0};
    char *send_buffer = NULL;
    size_t send_total_len = 0;
    u32_t tick1, tick2;

    LWIP_UNUSED_ARG(arg);

    send_buffer = pvPortMalloc(IPERF_BUFFER_SIZE);
    if (send_buffer == NULL) {
        log_e("malloc send buffer failed");
        goto exit;
    }

    for (size_t i = 0; i < IPERF_BUFFER_SIZE; i++) {
        send_buffer[i] = (char)(i % 256);
    }

    while (1) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            log_e("create socket failed");
            sys_msleep(10);
            continue;
        }

        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(IPV4_DST_PORT);
        client_addr.sin_len = sizeof(client_addr);
        client_addr.sin_addr.s_addr = inet_addr("192.168.0.100");
        memset(&client_addr.sin_zero, 0, sizeof(client_addr.sin_zero));

        if (connect(sockfd, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0) {
            log_e("connect to server failed");
            closesocket(sockfd);
            sys_msleep(10);
            continue;
        }

        log_i("connect to server success");

        tick1 = sys_now();
        while (1) {
            tick2 = sys_now();
            if (tick2 - tick1 >= configTICK_RATE_HZ * 5) {
                float speed = ((float)send_total_len * 8.0f) / ((float)(tick2 - tick1) / 1000.0f) / 1000000.0f;
                log_i("send total len: %d, speed: %.2f MB/s", send_total_len, speed);
                tick1 = tick2;
                send_total_len = 0;
            }
            ssize_t sent_len = send(sockfd, send_buffer, IPERF_BUFFER_SIZE, 0);
            if (sent_len > 0) {
                send_total_len += sent_len;
            } else {
                closesocket(sockfd);
                log_e("send to server failed");
                sys_msleep(10);
                break;
            }
        }
    }


exit:
    if (send_buffer) {
        vPortFree(send_buffer);
    }
    vTaskDelete(NULL);
}
#endif /* APP_EXAMPLES_IPERF_CLIENT_SOCKET_ENABLED */

#if APP_EXAMPLES_IPERF_SERVER_SOCKET_ENABLED
static void iperf_server_socket_thread(void *arg)
{
    int sockfd = -1, client_fd = -1;
    int ret;
    char *recv_buffer = NULL;
    uint64_t recv_len;
    fd_set read_set;
    struct timeval timeout = {0};
    struct sockaddr_in server_addr = {0};
    struct sockaddr_in client_addr = {0};
    socklen_t client_len = sizeof(client_addr);

    LWIP_UNUSED_ARG(arg);

    recv_buffer = pvPortMalloc(IPERF_BUFFER_SIZE);
    if (recv_buffer == NULL) {
        log_e("malloc recv buffer failed");
        goto exit;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        log_e("create socket failed");
        goto exit;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(IPV4_LISTEN_PORT);
    server_addr.sin_len = sizeof(server_addr);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

    ret = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        log_e("bind socket failed");
        goto exit;
    }

    /* iperf test client must 1 connection */
    ret = listen(sockfd, 1);
    if (ret < 0) {
        log_e("listen socket failed");
        goto exit;
    }

    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    while (1) {
        FD_ZERO(&read_set);
        FD_SET(sockfd, &read_set);
        ret = select(FD_SETSIZE, &read_set, NULL, NULL, &timeout);
        if (ret == 0) {
            log_i("no data");
            continue;
        } else if (ret < 0) {
            log_e("select socket failed:%d", ret);
            continue;
        } else if (FD_ISSET(sockfd, &read_set)) {
            log_i("new client connected.");
        }

        client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            log_e("accept socket failed");
            sys_msleep(10);
            continue;
        }

        int flag = 1;
        setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, (void *)&flag, sizeof(flag));

        while (1) {
            ret = recv(client_fd, recv_buffer, IPERF_BUFFER_SIZE , 0);
            if (ret <= 0) {
                log_i("client disconnect");
                closesocket(client_fd);
                client_fd = -1;
                break;
            }
        }
    }

exit:
    if (recv_buffer) {
        vPortFree(recv_buffer);
    }
    if (sockfd >= 0) {
        closesocket(sockfd);
    }
    vTaskDelete(NULL);
}
#endif /* APP_EXAMPLES_IPERF_SERVER_SOCKET_ENABLED */

void iperf_init(void)
{
#if APP_EXAMPLES_IPERF_SERVER_NETCONN_ENABLED
    sys_thread_new("iperf_server", iperf_server_thread, NULL, 1024, 5);
#endif /* APP_EXAMPLES_IPERF_SERVER_NETCONN_ENABLED */

#if APP_EXAMPLES_IPERF_CLIENT_NETCONN_ENABLED
    sys_thread_new("iperf_client", iperf_client_thread, NULL, 1024, 5);
#endif /* APP_EXAMPLES_IPERF_CLIENT_NETCONN_ENABLED */

#if APP_EXAMPLES_IPERF_CLIENT_SOCKET_ENABLED
    sys_thread_new("iperf_client_socket", iperf_client_socket_thread, NULL, 1024, 5);
#endif /* APP_EXAMPLES_IPERF_CLIENT_SOCKET_ENABLED */

#if APP_EXAMPLES_IPERF_SERVER_SOCKET_ENABLED
    sys_thread_new("iperf_server_socket", iperf_server_socket_thread, NULL, 1024, 5);
#endif /* APP_EXAMPLES_IPERF_SERVER_SOCKET_ENABLED */
}

#endif /* APP_EXAMPLES_IPERF_ENABLED */
