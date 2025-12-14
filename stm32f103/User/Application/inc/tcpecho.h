/**
 * @file tcpecho.h
 * @author Rougga (1055700185@qq.com)
 * @brief 
 * @version 0.1
 * @date 2025-12-11 23:01:54    +08:00
 * 
 * @copyright Copyright (c) 2023 Rougga(1055700185qq.com). All rights reserved.
 * 
 */

#ifndef TCPECHO_H
#define TCPECHO_H

/* tcp server multi connection enable */
#define TCP_SERVER_MULCONN_ENABLE 1
#define MAX_CONNECTIONS           6

/* the TCP server local port */
#define TCPECHO_PORT        5000


#ifdef __cplusplus
extern "C" {
#endif

void tcpecho_init(void);

#ifdef __cplusplus
}
#endif
#endif /* TCPECHO_H */
