/**
 * @file socket_udp.h
 * @author Rougga (1055700185@qq.com)
 * @brief socket udp example based on lwip socket API
 * @version 0.1
 * @date 2025-12-16 23:03:06    +08:00
 * 
 * @copyright Copyright (c) 2023 Rougga(1055700185qq.com). All rights reserved.
 * 
 */

#ifndef SOCKET_UDP_H
#define SOCKET_UDP_H

#define SOCKET_UDP_PORT    6000
#define SOCKET_UDP_RECV_BUFFER_SIZE    512

#ifdef __cplusplus
extern "C" {
#endif


void socket_udp_init(void);

#ifdef __cplusplus
}
#endif
#endif /* SOCKET_UDP_H */
