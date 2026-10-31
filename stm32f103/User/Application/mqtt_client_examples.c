/**
 * @file mqtt_client_examples.c
 * @author Rougga (1055700185@qq.com)
 * @brief mqtt client examples source file
 * @version 0.1
 * @date 2026-10-27 21:45:22    +08:00
 * 
 * @copyright Copyright (c) 2023 Rougga(1055700185qq.com). All rights reserved.
 * 
 */

#include "mqtt_client_examples.h"

#include "MQTTClient.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "lwip/netif.h"
#include "elog.h"
#include <stdio.h>

static MQTTClient mqtt_client;
static Network mqtt_network;
static uint8_t mqtt_sendbuf[512];
static uint8_t mqtt_readbuf[512];
static TimerHandle_t mqtt_publish_timer;
static volatile BaseType_t mqtt_publish_due;

static void mqtt_publish_timer_callback(TimerHandle_t timer)
{
    (void)timer;
    mqtt_publish_due = pdTRUE;
}

static int mqtt_publish_message(void)
{
    MQTTMessage message;
    static uint32_t publish_count = 0;
    static char publish_payload[64] = {0};
    int rc;

    int len = snprintf(publish_payload, sizeof(publish_payload), "hello from STM32, count: %lu", publish_count);
    if (len < 0 || len >= sizeof(publish_payload)) {
        log_e("Payload format error!\r\n");
        return -1; 
    }

    message.qos = QOS0;
    message.retained = 0;
    message.dup = 0;
    message.id = 0;
    message.payload = publish_payload;
    message.payloadlen = strlen(publish_payload);

    rc = MQTTPublish(&mqtt_client, "device/stm32/out", &message);
    if (rc == MQTT_SUCCESS) {
        publish_count++;
        log_i("MQTT message published\r\n");
    }

    return rc;
}

static int mqtt_network_has_ip(void)
{
    struct netif *netif = netif_default;

    if (netif == NULL) {
        return 0;
    }

    return netif_is_up(netif) && netif_is_link_up(netif) && !ip_addr_isany_val(*netif_ip4_addr(netif));
}

static void mqtt_message_handler(MessageData *data)
{
    log_i("topic=%.*s, payload=%.*s\r\n",
              data->topicName->lenstring.len,
              data->topicName->lenstring.data,
              (int)data->message->payloadlen,
           (char *)data->message->payload);
}

static void mqtt_client_task(void *argument)
{
    MQTTPacket_connectData connect_data =
        MQTTPacket_connectData_initializer;
    MQTTConnackData connack_data;
    static char client_id[] = "stm32f103-w5500";

    (void)argument;

    for (;;) {
        int rc;

        /* network_manager_init() 启动后，DHCP 不一定已经取得地址。 */
        while (!mqtt_network_has_ip()) {
            vTaskDelay(pdMS_TO_TICKS(500));
        }

        NetworkInit(&mqtt_network);
        MQTTClientInit(&mqtt_client,
                       &mqtt_network,
                       MQTT_COMMAND_TIMEOUT,
                       mqtt_sendbuf,
                       sizeof(mqtt_sendbuf),
                       mqtt_readbuf,
                       sizeof(mqtt_readbuf));

        rc = NetworkConnectTimeout(&mqtt_network,
                                   MQTT_CLIENT_EXAMPLES_BROKER_ADDR,
                                   MQTT_CLIENT_EXAMPLES_BROKER_PORT,
                                   MQTT_SOCKET_TIMEOUT);
        if (rc != 0) {
            log_e("MQTT TCP connect failed\r\n");
            vTaskDelay(pdMS_TO_TICKS(3000));
            continue;
        }

        log_i("MQTT TCP connected\r\n");
        connect_data.clientID.cstring = client_id;
        connect_data.keepAliveInterval = 30;
        connect_data.cleansession = 1;

        rc = MQTTConnectWithResults(&mqtt_client, &connect_data, &connack_data);
        if (rc != MQTT_SUCCESS) {
            if (rc > 0) {
                log_e("MQTT broker rejected CONNECT, CONNACK rc=%d\r\n", rc);
            } else {
                log_e("MQTT CONNECT failed, rc=%d\r\n", rc);
            }
        }
        if (rc == MQTT_SUCCESS) {
            rc = MQTTSubscribe(&mqtt_client,
                               "device/stm32/in",
                               QOS0,
                               mqtt_message_handler);
            if (rc != MQTT_SUCCESS) {
                log_e("MQTT SUBSCRIBE failed, rc=%d\r\n", rc);
            }
        }

        if (rc == MQTT_SUCCESS) {
            rc = mqtt_publish_message();
            if (rc != MQTT_SUCCESS) {
                log_e("MQTT PUBLISH failed, rc=%d\r\n", rc);
            }
        }

        if (rc == MQTT_SUCCESS) {
            mqtt_publish_due = pdFALSE;
            if (xTimerStart(mqtt_publish_timer, 0) != pdPASS) {
                log_e("Failed to start MQTT publish timer\r\n");
                rc = FAILURE;
            }
        }

        while (rc == MQTT_SUCCESS && MQTTIsConnected(&mqtt_client)) {
            rc = MQTTYield(&mqtt_client, 200);
            if (rc == MQTT_SUCCESS && mqtt_publish_due) {
                mqtt_publish_due = pdFALSE;
                rc = mqtt_publish_message();
                if (rc != MQTT_SUCCESS) {
                    log_e("MQTT periodic PUBLISH failed, rc=%d\r\n", rc);
                }
            }
        }

        if (xTimerStop(mqtt_publish_timer, 0) != pdPASS) {
            log_e("Failed to stop MQTT publish timer\r\n");
        }

        /* MQTTDisconnect() 发送协议包，但不会关闭底层 lwIP socket。 */
        if (MQTTIsConnected(&mqtt_client)) {
            (void)MQTTDisconnect(&mqtt_client);
        }
        mqtt_network.disconnect(&mqtt_network);

        log_i("MQTT disconnected, retrying\r\n");
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

void mqtt_client_examples_init(void)
{
    mqtt_publish_timer = xTimerCreate("mqtt_pub",
                                      pdMS_TO_TICKS(MQTT_PUBLISH_INTERVAL_MS),
                                      pdTRUE,
                                      NULL,
                                      mqtt_publish_timer_callback);
    if (mqtt_publish_timer == NULL) {
        log_e("Failed to create MQTT publish timer\r\n");
        return;
    }

    if (xTaskCreate(mqtt_client_task,
                    "mqtt_client",
                    768,
                    NULL,
                    tskIDLE_PRIORITY + 2,
                    NULL) != pdPASS) {
        log_e("Failed to create MQTT task\r\n");
        (void)xTimerDelete(mqtt_publish_timer, 0);
        mqtt_publish_timer = NULL;
    }
}
