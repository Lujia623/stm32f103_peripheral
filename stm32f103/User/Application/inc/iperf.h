/**
 * @file iperf.h
 * @author Rougga (1055700185@qq.com)
 * @brief iperf test examples
 * @version 0.1
 * @date 2025-12-17 22:27:57    +08:00
 * 
 * @copyright Copyright (c) 2023 Rougga(1055700185qq.com). All rights reserved.
 * 
 */

#ifndef IPERF_H
#define IPERF_H

#define IPERF_NETCONN_CLIENT        1   /* iperf netconn client */
#define IPERF_BUFFER_SIZE           (4 * 1024)

#define IPV4_DST_ADDR0    192
#define IPV4_DST_ADDR1    168
#define IPV4_DST_ADDR2    0
#define IPV4_DST_ADDR3    100
#define IPV4_DST_PORT     5001

#define IPV4_LISTEN_PORT  5000

#include "app_examples_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if APP_EXAMPLES_IPERF_ENABLED

void iperf_init(void);

#endif /* APP_EXAMPLES_IPERF_ENABLED */
#ifdef __cplusplus
}
#endif

#endif /* IPERF_H */
