/**
 * @file MQTTlwip.c
 * @author Rougga (1055700185@qq.com)
 * @brief MQTTClient lwIP adapter implementation.
 * @version 0.1
 * @date 2026-09-22 23:38:59    +08:00
 * 
 * @copyright Copyright (c) 2023 Rougga(1055700185qq.com). All rights reserved.
 * 
 */

#include "MQTTlwip.h"

#include <string.h>
#include <errno.h>

#include "lwip/ip4_addr.h"
#include "lwip/tcp.h"

#ifndef MQTT_LWIP_CONNECT_TIMEOUT_MS
#define MQTT_LWIP_CONNECT_TIMEOUT_MS    10000
#endif

#if !defined(LWIP_SOCKET) || (LWIP_SOCKET == 0)
#error "MQTTlwip requires LWIP_SOCKET = 1"
#endif

/*******************************************************************************
 * 内部工具
 ******************************************************************************/

static uint32_t timer_now_ms(void)
{
    return (uint32_t)sys_now();
}

/**
 * @brief 等待 socket 可读或可写
 *
 * @param fd          socket 描述符
 * @param for_write   0：等待可读；1：等待可写
 * @param timeout_ms  超时时间，单位 ms。
 *                    timeout_ms < 0：永久等待
 *                    timeout_ms = 0：立即返回
 *
 * @return 1：就绪
 *         0：超时
 *        -1：错误
 */
static int wait_fd(int fd, int for_write, int timeout_ms)
{
    fd_set fds;
    struct timeval tv;
    struct timeval *ptv = &tv;
    int rc;

    if (fd < 0)
    {
        return -1;
    }

    if (timeout_ms < 0)
    {
        ptv = NULL;
    }
    else
    {
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
    }

    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    if (for_write)
    {
        rc = lwip_select(fd + 1, NULL, &fds, NULL, ptv);
    }
    else
    {
        rc = lwip_select(fd + 1, &fds, NULL, NULL, ptv);
    }

    if (rc < 0)
    {
        return -1;
    }

    if (rc == 0)
    {
        return 0;
    }

    return 1;
}

/**
 * @brief 初始化定时器，将其置为已过期状态
 */
void TimerInit(Timer *timer)
{
    if (timer != NULL)
    {
        timer->start_time = 0;
        timer->timeout_ms = 0;
        timer->state = TIMER_DISARMED;
    }
}

/**
 * @brief 检查定时器是否已经到期
 * 
 * @return 1：已到期（或未启动/指针为空）；0：未到期
 */
char TimerIsExpired(Timer *timer)
{
    if (timer == NULL || timer->state != TIMER_ARMED)
    {
        return 1; /* 未启动的定时器视为已过期 */
    }

    /* 利用无符号数模 2^32 减法，即使 timer_now_ms() 翻转回绕，差值依然严格等于流逝时间 */
    uint32_t elapsed_ms = timer_now_ms() - timer->start_time;

    if (elapsed_ms >= timer->timeout_ms)
    {
        timer->state = TIMER_DISARMED; /* 到期后自动转为未激活状态 */
        return 1;
    }

    return 0;
}

/**
 * @brief 启动毫秒级倒计时
 */
void TimerCountdownMS(Timer *timer, unsigned int timeout_ms)
{
    if (timer != NULL)
    {
        timer->start_time = timer_now_ms();
        timer->timeout_ms = (uint32_t)timeout_ms;
        timer->state = TIMER_ARMED;
    }
}

/**
 * @brief 启动秒级倒计时
 */
void TimerCountdown(Timer *timer, unsigned int timeout_s)
{
    /* 防止超大秒数相乘溢出，限定上限（例如最大 49 天） */
    if (timeout_s > (0xFFFFFFFFU / 1000U))
    {
        TimerCountdownMS(timer, 0xFFFFFFFFU);
    }
    else
    {
        TimerCountdownMS(timer, timeout_s * 1000U);
    }
}

/**
 * @brief 获取定时器剩余毫秒数
 */
int TimerLeftMS(Timer *timer)
{
    if (timer == NULL || timer->state != TIMER_ARMED)
    {
        return 0;
    }

    uint32_t elapsed_ms = timer_now_ms() - timer->start_time;

    if (elapsed_ms >= timer->timeout_ms)
    {
        timer->state = TIMER_DISARMED;
        return 0;
    }

    uint32_t left_ms = timer->timeout_ms - elapsed_ms;

    /* 限制返回值符合 int 范围（最大 INT_MAX） */
    if (left_ms > 0x7FFFFFFFU)
    {
        return 0x7FFFFFFF;
    }

    return (int)left_ms;
}

/*******************************************************************************
 * Network 读写实现
 ******************************************************************************/

/**
 * @brief lwIP MQTT 读函数
 *
 * Paho 期望的返回值语义：
 *
 *   > 0：读到的字节数
 *   = 0：超时，没有读到数据
 *   < 0：错误或连接关闭
 */
int lwip_mqtt_read(Network *n, unsigned char *buffer, int len, int timeout_ms)
{
    int attempted_wait = 0;
    int received = 0;
    uint32_t start_ms;

    if (n == NULL || buffer == NULL || n->my_socket < 0)
    {
        return -1;
    }

    if (len <= 0)
    {
        return 0;
    }

    start_ms = timer_now_ms();

    while (received < len) {
        int rc;
        int wait_rc;
        int wait_ms;
        int elapsed_ms;

        if (timeout_ms < 0) {
            wait_ms = -1;
        } else {
            elapsed_ms = timer_now_ms() - start_ms;
            if (elapsed_ms >= timeout_ms) {
                if (attempted_wait) {
                    return received == 0 ? 0 : -1;
                }
                
                wait_ms = 0; // force a final wait with zero timeout
            } else {
                wait_ms = timeout_ms - (int)elapsed_ms;
            }
        }

        attempted_wait = 1;
        wait_rc = wait_fd(n->my_socket, 0, wait_ms);
        if (wait_rc == 0) {
            return received == 0 ? 0 : received; // 读取部分后返回实际读取的字节数
        }

        if (wait_rc < 0) {
            /* wait_fd 出错，检查是否被中断 */
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }

        rc = lwip_recv(n->my_socket, buffer + received, len - received, 0);
        if (rc > 0)
        {
            received += rc;
            continue;
        }

        if (rc == 0)
        {
            return -1;
        }

        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
            continue;
        }

        return -1;
    }

    return received;
}

/**
 * @brief lwIP MQTT 写函数
 *
 * Paho 期望的返回值语义：
 *
 *   > 0：实际写出的字节数
 *   = 0：超时，没有写出数据
 *   < 0：错误
 *
 * 注意：
 *   这里允许返回部分写长度。
 *   因此 MQTTClient.c 的 sendPacket() 必须正确处理剩余长度：
 *
 *       length - sent
 */
int lwip_mqtt_write(Network *n, unsigned char *buffer, int len, int timeout_ms)
{
    uint32_t start_time;
    int sent = 0;
    int first_attempt = 1;

    if (n == NULL || buffer == NULL || n->my_socket < 0)
    {
        return -1;
    }

    if (len <= 0)
    {
        return 0;
    }

    start_time = timer_now_ms();

    while (sent < len)
    {
        int rc;

        if (!first_attempt && timeout_ms >= 0) {
            uint32_t elapsed_ms = timer_now_ms() - start_time;
            if (elapsed_ms >= (uint32_t)timeout_ms) {
                break;
            }
        }

        first_attempt = 0;
        rc = lwip_send(n->my_socket, buffer + sent, len - sent, 0);
        if (rc > 0)
        {
            sent += rc;
            continue;
        }

        if (rc == 0)
        {
            return sent > 0 ? sent : -1;
        }

        if (errno == EINTR) {
            continue;
        }

        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            int wait_ms, wait_rc;

            if (timeout_ms < 0) {
                wait_ms = -1;
            } else {
                uint32_t elapsed_ms = timer_now_ms() - start_time;
                if (elapsed_ms >= (uint32_t)timeout_ms) {
                    return sent;
                }

                wait_ms = timeout_ms - (int)elapsed_ms;
            }

            wait_rc = wait_fd(n->my_socket, 1, wait_ms);
            if (wait_rc > 0) {
                continue;
            }

            if (wait_rc == 0) {
                return sent;
            }

            if (errno == EINTR) {
                continue;
            }

            return -1;
        }

        return -1;
    }

    return sent;
}

/**
 * @brief 关闭 MQTT 底层 socket
 */
void lwip_mqtt_disconnect(Network *n)
{
    if (n == NULL)
    {
        return;
    }

    if (n->my_socket >= 0)
    {
        lwip_close(n->my_socket);
        n->my_socket = -1;
    }
}

/*******************************************************************************
 * Network 初始化 / 连接
 ******************************************************************************/

void NetworkInit(Network *n)
{
    if (n == NULL)
    {
        return;
    }

    n->my_socket = -1;
    n->mqttread = lwip_mqtt_read;
    n->mqttwrite = lwip_mqtt_write;
    n->disconnect = lwip_mqtt_disconnect;
}

/**
 * @brief 解析 IPv4 地址
 *
 * 支持：
 *   - 直接 IP 字符串，例如 "192.168.1.100"
 *   - 域名，例如 "broker.emqx.io"，需要 LWIP_DNS = 1
 */
static int resolve_ipv4(const char *host, uint32_t *out_ip)
{
    ip4_addr_t ip;

    if (host == NULL || out_ip == NULL)
    {
        return -1;
    }

    if (ip4addr_aton(host, &ip) != 0)
    {
        *out_ip = ip.addr;
        return 0;
    }

#if defined(LWIP_DNS) && (LWIP_DNS == 1)
    {
        struct hostent *he;

        he = lwip_gethostbyname(host);
        if (he != NULL &&
            he->h_addr_list != NULL &&
            he->h_addr_list[0] != NULL &&
            he->h_addrtype == AF_INET &&
            he->h_length == (int)sizeof(uint32_t))
        {
            memcpy(out_ip, he->h_addr_list[0], sizeof(uint32_t));
            return 0;
        }
    }
#else
    (void)host;
#endif

    return -1;
}

/**
 * @brief 连接 MQTT Broker，带超时
 *
 * @param n           Network 对象
 * @param addr        IP 字符串或域名
 * @param port        Broker 端口，例如 1883
 * @param timeout_ms  连接超时，单位 ms
 *
 * @return 0：成功
 *        -1：失败
 */
int NetworkConnectTimeout(Network *n, char *addr, int port, int timeout_ms)
{
    uint32_t ip;
    int fd;
    int rc;
    int nonblocking = 1;
    struct sockaddr_in server_addr;

    if (n == NULL || addr == NULL || port <= 0)
    {
        return -1;
    }

    if (timeout_ms <= 0)
    {
        timeout_ms = MQTT_LWIP_CONNECT_TIMEOUT_MS;
    }

    if (resolve_ipv4(addr, &ip) != 0)
    {
        return -1;
    }

    if (n->my_socket >= 0) {
        lwip_close(n->my_socket);
        n->my_socket = -1;
    }

    fd = lwip_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0)
    {
        return -1;
    }

    /*
     * 设置为非阻塞模式。
     * 后续读写依赖 select + 非阻塞 socket 实现超时。
     */
    rc = lwip_ioctl(fd, FIONBIO, &nonblocking);
    if (rc != 0)
    {
        lwip_close(fd);
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = lwip_htons((uint16_t)port);
    server_addr.sin_addr.s_addr = ip;

    rc = lwip_connect(fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (rc < 0)
    {
        if (errno != EINPROGRESS && errno != EALREADY)
        {
            lwip_close(fd);
            return -1;
        }

        /*
         * 非阻塞 connect 返回 EINPROGRESS 时，
         * 等待 socket 可写表示连接完成。
         */
        rc = wait_fd(fd, 1, timeout_ms);
        if (rc <= 0)
        {
            lwip_close(fd);
            return -1;
        }

        {
            int so_error = 0;
            socklen_t len = sizeof(so_error);

            rc = lwip_getsockopt(fd, SOL_SOCKET, SO_ERROR, &so_error, &len);
            if (rc < 0 || so_error != 0)
            {
                lwip_close(fd);
                return -1;
            }
        }
    }

#ifdef TCP_NODELAY
    {
        int flag = 1;

        /*
         * MQTT 通常是小包交互，打开 TCP_NODELAY 可以降低延迟。
         * 如果你的带宽更敏感、包很大，也可以不开。
         */
        lwip_setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
    }
#endif

    n->my_socket = fd;
    return 0;
}

/**
 * @brief 连接 MQTT Broker，默认超时
 */
int NetworkConnect(Network *n, char *addr, int port)
{
    return NetworkConnectTimeout(n, addr, port, MQTT_LWIP_CONNECT_TIMEOUT_MS);
}

/*******************************************************************************
 * MQTT_TASK 可选支持
 ******************************************************************************/

#if defined(MQTT_TASK)

#ifndef MQTT_TASK_STACK_SIZE
#define MQTT_TASK_STACK_SIZE    (configMINIMAL_STACK_SIZE * 5)
#endif

void MutexInit(Mutex *mutex)
{
    if (mutex != NULL)
    {
        mutex->sem = xSemaphoreCreateRecursiveMutex();
    }
}

int MutexLock(Mutex *mutex)
{
    if (mutex == NULL || mutex->sem == NULL)
    {
        return -1;
    }

    if (xSemaphoreTake(mutex->sem, portMAX_DELAY) == pdTRUE)
    {
        return 0;
    }

    return -1;
}

int MutexUnlock(Mutex *mutex)
{
    if (mutex == NULL || mutex->sem == NULL)
    {
        return -1;
    }

    if (xSemaphoreGive(mutex->sem) == pdTRUE)
    {
        return 0;
    }

    return -1;
}

int ThreadStart(Thread *thread, void (*fn)(void *), void *arg)
{
    BaseType_t rc;
    UBaseType_t prio;

    if (thread == NULL || fn == NULL)
    {
        return -1;
    }

    prio = uxTaskPriorityGet(NULL);
    if (prio == 0)
    {
        prio = tskIDLE_PRIORITY + 1;
    }

    rc = xTaskCreate(fn,
                     "mqtt_task",
                     MQTT_TASK_STACK_SIZE,
                     arg,
                     prio,
                     &thread->task);

    return (rc == pdPASS) ? 0 : -1;
}

#endif /* MQTT_TASK */
