/**
 * @file http_client.h
 * @author Rougga (1055700185@qq.com)
 * @brief HTTP client example header file.
 * @version 0.1
 * @date 2025-12-21 21:10:13    +08:00
 * 
 * @copyright Copyright (c) 2023 Rougga(1055700185qq.com). All rights reserved.
 * 
 */


#ifndef HTTP_CLIENT_EXAMPLES_H
#define HTTP_CLIENT_EXAMPLES_H

#include "app_examples_config.h"

#if APP_EXAMPLES_HTTP_CLIENT_ENABLED

#define CLIENT_RECV_BUFFER_SIZE     1024
#define HOST_NAME                   "www.baidu.com"
#define HTTP_HOST_PORT              80

#ifdef __cplusplus
extern "C" {
#endif

void http_client_init(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_EXAMPLES_HTTP_CLIENT_ENABLED */
#endif /* HTTP_CLIENT_EXAMPLES_H */
