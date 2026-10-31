/**
 * @file mqtt_client_examples.h
 * @author Rougga (1055700185@qq.com)
 * @brief mqtt client examples header file
 * @version 0.1
 * @date 2026-10-27 21:44:13    +08:00
 * 
 * @copyright Copyright (c) 2023 Rougga(1055700185qq.com). All rights reserved.
 * 
 */

#ifndef MQTT_CLIENT_EXAMPLES_H
#define MQTT_CLIENT_EXAMPLES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "app_examples_config.h"

#define MQTT_CLIENT_EXAMPLES_BROKER_ADDR    "192.168.1.6"
#define MQTT_CLIENT_EXAMPLES_BROKER_PORT    1883
#define MQTT_CLIENT_EXAMPLES_TOPIC          "test/mqtt_client_examples"
#define MQTT_COMMAND_TIMEOUT                 10000
#define MQTT_SOCKET_TIMEOUT                  5000
#define MQTT_PUBLISH_INTERVAL_MS             2000

void mqtt_client_examples_init(void);

#ifdef __cplusplus
}
#endif
#endif /* MQTT_CLIENT_EXAMPLES_H */
