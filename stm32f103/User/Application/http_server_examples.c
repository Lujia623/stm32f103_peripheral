#include "http_server_examples.h"
#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/sys.h"
#include "elog.h"
#include <string.h>

#if APP_EXAMPLES_HTTP_SERVER_ENABLED

#define HTTP_SERVER_PORT    80

static uint8_t led = false;

// Response header: It contains the status line, type, length, and a declaration to close the connection.
// Note that there must be two \r\n at the end, indicating the end of the head.
static const char http_response_header[] = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=utf-8\r\n"
    "Connection: close\r\n"
    "Server: LwIP-STM32\r\n"
    "\r\n";

// HTML page segment definition
static const char http_html_start[] = 
    "<!DOCTYPE html><html><head><title>LED Control</title>"
    "<style>body{text-align:center;font-family:sans-serif;margin-top:50px;}"
    ".btn{padding:15px 30px;font-size:18px;cursor:pointer;border-radius:5px;border:none;color:white;}"
    ".on{background:#4CAF50;}.off{background:#f44336;}</style></head>"
    "<body><h1>Ethernet Console</h1>";

static const char http_html_on[] = 
    "<p>LED Status: <b style='color:green'>On</b></p>"
    "<form method='POST' action='/off'><input type='submit' class='btn off' value='Turn Off LED'></form>";

static const char http_html_off[] = 
    "<p>LED Status: <b style='color:red'>Off</b></p>"
    "<form method='POST' action='/on'><input type='submit' class='btn on' value='Turn On LED'></form>";

static const char http_html_end[] = 
    "<br><hr><a href='http://www.firebbs.cn'>Ethernet Console</a></body></html>";

static void http_server_send_response(struct netconn *conn, uint8_t led_status)
{
    netconn_write(conn, http_response_header, strlen(http_response_header), NETCONN_NOCOPY);
    /* Send our HTML page */
    netconn_write(conn, http_html_start, sizeof(http_html_start) - 1, NETCONN_NOCOPY);
    if (led_status == true)
        netconn_write(conn, http_html_on, sizeof(http_html_on) - 1, NETCONN_NOCOPY);
    else
        netconn_write(conn, http_html_off, sizeof(http_html_off) - 1, NETCONN_NOCOPY);
    netconn_write(conn, http_html_end, sizeof(http_html_end) - 1, NETCONN_NOCOPY);
}

static void http_server_process(struct netconn *conn)
{
    struct netbuf *buf = NULL;
    char *data;
    u16_t len;
    err_t err;

    err = netconn_recv(conn, &buf);

    if (err == ERR_OK && buf != NULL) {
        err = netbuf_data(buf, (void**)&data, &len);

        if (strncmp(data, "GET /", 5) == 0) {
            log_i("HTTP GET request received.");
        } else if (strncmp(data, "POST", 4) == 0) {
            log_i("HTTP POST request received.");
            if (strncmp(data + 6, "on", 2) == 0) {
                led = true;
                log_i("LED turned ON.");
            } else if (strncmp(data + 6, "off", 3) == 0) {
                led = false;
                log_i("LED turned OFF.");
            }
        }
        http_server_send_response(conn, led);
        netbuf_delete(buf);
    }

    netconn_close(conn);
}

static void http_server_thread(void *arg)
{
    struct netconn *conn = NULL, *newconn = NULL;
    err_t err;

    LWIP_UNUSED_ARG(arg);

    conn = netconn_new(NETCONN_TCP);
    if (conn == NULL) {
        log_e("http server netconn_new failed.");
        goto exit;
    }

    err = netconn_bind(conn, NULL, HTTP_SERVER_PORT);
    if (err != ERR_OK) {
        log_e("http server netconn_bind failed.");
        goto exit;
    }

    err = netconn_listen(conn);
    if (err != ERR_OK) {
        log_e("http server netconn_listen failed.");
        goto exit;
    }

    while (1) {
        /* Receive the request */
        err = netconn_accept(conn, &newconn);
        if (err != ERR_OK) {
            log_e("http server netconn_accept failed.");
            break;
        }
        log_i("http server netconn_accept success.");
        
        http_server_process(newconn);
        netconn_delete(newconn);
    }

exit:
    if (conn != NULL) {
        netconn_close(conn);
        netconn_delete(conn);
    }
}

void http_server_init(void)
{
    sys_thread_new("http_server_thread", http_server_thread, NULL, 1024, 2);
}

#endif /* APP_EXAMPLES_HTTP_SERVER_ENABLED */
