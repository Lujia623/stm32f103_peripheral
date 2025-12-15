/**
 * @file socket_tcp_client.h
 * @author Rougga (1055700185@qq.com)
 * @brief socket tcp client example based on lwip socket API
 * @version 0.1
 * @date 2025-12-15 21:56:36    +08:00
 * 
 * @copyright Copyright (c) 2023 Rougga(1055700185qq.com). All rights reserved.
 * 
 */

#ifndef SOCKET_TCP_CLIENT_H
#define SOCKET_TCP_CLIENT_H 

#ifdef __cplusplus
extern "C" {
#endif 

#define SOCKET_TCP_CLIENT_PORT    5001   /* The port on which to listen for incoming TCP packets */


void socket_tcp_client_init(void);

#ifdef __cplusplus
}
#endif
#endif /* SOCKET_TCP_CLIENT_H */
