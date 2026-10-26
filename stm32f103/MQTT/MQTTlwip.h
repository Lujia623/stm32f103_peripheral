/**
 * @file MQTTlwip.h
 * @author Rougga (1055700185@qq.com)
 * @brief lwIP adapter for MQTTClient.
 * @version 0.1
 * @date 2026-09-22 23:37:57    +08:00
 * 
 * @copyright Copyright (c) 2023 Rougga(1055700185qq.com). All rights reserved.
 * 
 */

#ifndef MQTTLWIP_H
#define MQTTLWIP_H

#include "lwip/opt.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/inet.h"
#include "lwip/sys.h"

#include <stdint.h>

#if defined(MQTT_TASK)
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Timer
 ******************************************************************************/

typedef struct Timer
{
    uint32_t end_time;   /* 到期时间，单位 ms */
} Timer;

void TimerInit(Timer *timer);
char TimerIsExpired(Timer *timer);
void TimerCountdownMS(Timer *timer, unsigned int timeout_ms);
void TimerCountdown(Timer *timer, unsigned int timeout_s);
int TimerLeftMS(Timer *timer);

/*******************************************************************************
 * Network
 ******************************************************************************/

typedef struct Network Network;

struct Network
{
    int my_socket;

    int (*mqttread)(Network *, unsigned char *, int, int);
    int (*mqttwrite)(Network *, unsigned char *, int, int);
    void (*disconnect)(Network *);
};

void NetworkInit(Network *n);
int NetworkConnect(Network *n, char *addr, int port);
int NetworkConnectTimeout(Network *n, char *addr, int port, int timeout_ms);

int lwip_mqtt_read(Network *n, unsigned char *buffer, int len, int timeout_ms);
int lwip_mqtt_write(Network *n, unsigned char *buffer, int len, int timeout_ms);
void lwip_mqtt_disconnect(Network *n);

/*******************************************************************************
 * MQTT_TASK 可选支持
 *
 * 如果你定义了 MQTT_TASK，则需要 FreeRTOS。
 * 如果你不使用 MQTT_TASK，可以不定义它，也不需要下面的 Mutex / Thread。
 ******************************************************************************/

#if defined(MQTT_TASK)

typedef struct Mutex
{
    SemaphoreHandle_t sem;
} Mutex;

void MutexInit(Mutex *mutex);
int MutexLock(Mutex *mutex);
int MutexUnlock(Mutex *mutex);

typedef struct Thread
{
    TaskHandle_t task;
} Thread;

int ThreadStart(Thread *thread, void (*fn)(void *), void *arg);

#endif /* MQTT_TASK */

#ifdef __cplusplus
}
#endif

#endif /* MQTTLWIP_H */
