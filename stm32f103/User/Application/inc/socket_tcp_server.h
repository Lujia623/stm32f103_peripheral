/**
 * @file socket_tcp_server.h
 * @author Rougga (1055700185@qq.com)
 * @brief socket tcp server used socket api
 * @version 0.1
 * @date 2025-12-16 22:04:33    +08:00
 * 
 * @copyright Copyright (c) 2023 Rougga(1055700185qq.com). All rights reserved.
 * 
 */

#ifndef SOCKET_TCP_SERVER_H
#define SOCKET_TCP_SERVER_H

#include "app_examples_config.h"

#define SOCKET_TCP_PORT     5000

#ifdef __cplusplus
extern "C" {
#endif 

#if APP_EXAMPLES_TCP_SERVER_SOCKET_ENABLED

void socket_tcp_server_init(void);

#endif /* APP_EXAMPLES_TCP_SERVER_SOCKET_ENABLED */

#ifdef __cplusplus
}
#endif
#endif /* SOCKET_TCP_SERVER_H */
