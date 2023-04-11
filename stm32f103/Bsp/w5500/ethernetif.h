#ifndef __ETHERNETIF_H_
#define __ETHERNETIF_H_

#include "lwip/err.h"
#include "lwip/netif.h"

#define MAX_MTU             (1500U)


err_t ethernetif_init(struct netif *netif);

void ethernetif_input(struct netif *netif);
void ethernetif_output( void *argument );

#endif

