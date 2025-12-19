#ifndef __CLIENT_H_
#define __CLIENT_H_

#include "app_examples_config.h"

#define DEST_IP_ADDR0               192
#define DEST_IP_ADDR1               168
#define DEST_IP_ADDR2                 0
#define DEST_IP_ADDR3               100

#define DEST_PORT                  5001

#ifdef __cplusplus
extern "C" {
#endif 

#if APP_EXAMPLES_TCP_CLIENT_NETCONN_ENABLED

void client_init(void);

#endif /* APP_EXAMPLES_TCP_CLIENT_NETCONN_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* __CLIENT_H_ */
