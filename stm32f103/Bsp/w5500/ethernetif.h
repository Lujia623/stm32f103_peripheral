#ifndef __ETHERNETIF_H_
#define __ETHERNETIF_H_

#include "lwip/err.h"
#include "lwip/netif.h"

/* the maximum transfer unit of the Ethernet.1516 is the maximum frame size of the 802.3 standard. 
* 2bytes is the length of the W5500*/
#define MAX_MTU_BUFFER_SIZE                 (1518U)
#define MAX_MTU                             (1500U)
#define NETIF_IN_TASK_STACK_SIZE			( 1024 )
#define NETIF_IN_TASK_PRIORITY			    ( 3 )
#define NETIF_INPUT_SEM_TIMEOUT             ( 500U )

err_t ethernetif_init(struct netif *netif);

#if NO_SYS
void ethernetif_input(struct netif *netif);
#endif /* NO_SYS */
void ethernetif_output( void *argument );

#endif

