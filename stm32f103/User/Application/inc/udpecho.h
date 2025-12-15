/**
 * @file udpecho.h
 * @author Rougga (1055700185@qq.com)
 * @brief upd echo server example based on lwip netconn API
 * @version 0.1
 * @date 2025-12-14 22:40:23    +08:00
 * 
 * @copyright Copyright (c) 2023 Rougga(1055700185qq.com). All rights reserved.
 * 
 */

#ifndef UDPECHO_H
#define UDPECHO_H

#ifdef __cplusplus
extern "C" {
#endif

#define UDPECHO_PORT    7000   /* The port on which to listen for incoming UDP packets */

void udpecho_init(void);

#ifdef __cplusplus
}
#endif
#endif /* UDPECHO_H */
