/**
 * @file http_client.c
 * @author Rougga (1055700185@qq.com)
 * @brief HTTP client example source file.
 * @version 0.1
 * @date 2025-12-21 21:10:36    +08:00
 * 
 * @copyright Copyright (c) 2023 Rougga(1055700185qq.com). All rights reserved.
 * 
 */

#include "http_client_examples.h"

#if APP_EXAMPLES_HTTP_CLIENT_ENABLED

#include "lwip/api.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/opt.h"
#include "elog.h"
#include "lwip/netdb.h"
#include <string.h>

static const uint8_t get_buffer[] = "GET / HTTP/1.1\r\nHost: www.baidu.com\r\nConnection: close\r\n\r\n";

static void http_client_thread(void *pvParameters)
{
    int socket_fd = -1;
    char *recv_buffer = NULL;
    struct hostent *host = NULL;
    struct in_addr **addr_list;
    struct sockaddr_in client_addr = {0};
    size_t get_buffer_len = strlen((const char *)get_buffer);

    recv_buffer = (char *)pvPortMalloc(CLIENT_RECV_BUFFER_SIZE);
    if (recv_buffer == NULL) {
        log_e("http client malloc recv buffer failed");
        goto exit;
    }

    while (1) {
        host = gethostbyname(HOST_NAME);
        if (host == NULL) {
            log_e("http client gethostbyname failed:%d", h_errno);
            vTaskDelay(1000);
            continue;
        }

        addr_list = (struct in_addr **)host->h_addr_list;
        
        if (addr_list[0] != NULL) {
            char *ip_str = inet_ntoa(*addr_list[0]);
            log_i("Resolved %s to %s\n", HOST_NAME, ip_str);
        } else {
            log_e("No addresses found for host: %s", HOST_NAME);
            vTaskDelay(1000);
            continue;
        }

        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd < 0) {
            log_e("http client create socket failed:%d", errno);
            vTaskDelay(1000);
            continue;
        }

        memset(&client_addr, 0, sizeof(client_addr));
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(HTTP_HOST_PORT);
        client_addr.sin_addr = *addr_list[0];
        client_addr.sin_len = sizeof(client_addr);

        if (connect(socket_fd, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0) {
            log_e("http client connect failed:%d", errno);
            closesocket(socket_fd);
            socket_fd = -1;
            vTaskDelay(1000);
            continue;
        }

        if (send(socket_fd, get_buffer, get_buffer_len, 0) < 0) {
            log_e("http client send failed:%d", errno);
            closesocket(socket_fd);
            socket_fd = -1;
            vTaskDelay(1000);
            continue;
        }

        log_d("http client send success");
        elog_raw("\r\n========================================================================================================\r\n");
        while (1) {
            int len = recv(socket_fd, recv_buffer, CLIENT_RECV_BUFFER_SIZE - 1, 0);
            if (len > 0) {
                recv_buffer[len] = '\0';
                elog_raw("%s", recv_buffer);
            } else if (len == 0) {
                elog_raw("\r\n");
                log_d("http client recv completed");
                break;
            } else {
                log_e("http client recv error: %d", len);
                break;
            }
        }
        elog_raw("\r\n========================================================================================================\r\n");
        closesocket(socket_fd);
        socket_fd = -1;

        sys_msleep(20000); // Wait for 20 seconds before the next request
    }

exit:
    if (socket_fd >= 0) {
        closesocket(socket_fd);
    }
    if (recv_buffer != NULL) {
        vPortFree(recv_buffer);
    }
    vTaskDelete(NULL);
}

void http_client_init(void)
{
    sys_thread_new("http_client", http_client_thread, NULL, 1024, 2);
}

#endif /* APP_EXAMPLES_HTTP_CLIENT_ENABLED */
