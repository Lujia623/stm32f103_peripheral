/**
 * @file app_examples_config.h
 * @author Rougga (1055700185@qq.com)
 * @brief Configuration file for application examples.
 * @version 0.1
 * @date 2025-12-19 21:56:31    +08:00
 * 
 * @copyright Copyright (c) 2023 Rougga(1055700185qq.com). All rights reserved.
 * 
 */

#ifndef APP_EXAMPLES_CONFIG_H
#define APP_EXAMPLES_CONFIG_H

/* tcp client netconn examples enabled. */
#define APP_EXAMPLES_TCP_CLIENT_NETCONN_ENABLED     0
/* tcp server netconn examples enabled. */
#define APP_EXAMPLES_TCP_SERVER_NETCONN_ENABLED     0
/* udp echo netconn examples enabled. */
#define APP_EXAMPLES_UDP_ECHO_NETCONN_ENABLED       0
/* tcp client socket examples enabled. */
#define APP_EXAMPLES_TCP_CLIENT_SOCKET_ENABLED      0
/* tcp server socket examples enabled. */
#define APP_EXAMPLES_TCP_SERVER_SOCKET_ENABLED      0
/* udp echo socket examples enabled. */
#define APP_EXAMPLES_UDP_ECHO_SOCKET_ENABLED        0

/* iperf examples enabled, iperf server and iperf client.
 * iperf server and iperf client examples.
 * base on tcp protocol. */
#define APP_EXAMPLES_IPERF_ENABLED                  0
#if APP_EXAMPLES_IPERF_ENABLED
/* iperf client netconn examples enabled. */
#define APP_EXAMPLES_IPERF_CLIENT_NETCONN_ENABLED   0
/* iperf server netconn examples enabled. */
#define APP_EXAMPLES_IPERF_SERVER_NETCONN_ENABLED   0
/* iperf client socket examples enabled. */
#define APP_EXAMPLES_IPERF_CLIENT_SOCKET_ENABLED    0
/* iperf server socket examples enabled. */
#define APP_EXAMPLES_IPERF_SERVER_SOCKET_ENABLED    1
#endif /* APP_EXAMPLES_IPERF_ENABLED */

/* used network manager */
#define NETWORK_MANAGER_ENABLED                     1

/*  http client examples enabled. */
#define APP_EXAMPLES_HTTP_CLIENT_ENABLED            1

/* http server examples enabled. */
#define APP_EXAMPLES_HTTP_SERVER_ENABLED            1

#endif /* APP_EXAMPLES_CONFIG_H */
