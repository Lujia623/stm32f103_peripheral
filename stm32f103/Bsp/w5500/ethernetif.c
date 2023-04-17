/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

#include "main.h"
#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/timeouts.h"
#include "netif/ethernet.h"
#include "netif/etharp.h"
#include "lwip/ethip6.h"
#include "ethernetif.h"
#include <string.h>

/* Define those to better describe your network interface. */
#define IFNAME0 'R'
#define IFNAME1 'S'

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
};
SemaphoreHandle_t s_xSemaphore = NULL;
/* Forward declarations. */
void ethernetif_input(void *pvParameters);

void w5500_IR_clr(void) {
  setSn_IR(0, 0x1F);
  setSIR(0);
}

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void
low_level_init(struct netif *netif)
{
  uint8_t ret = 0;

  ret = W5500_lwip_init();
  if(ret == 0)
  {
    netif->flags |= NETIF_FLAG_LINK_UP;
  }
  

#if LWIP_ARP || LWIP_ETHERNET 

  /* set MAC hardware address length */
  netif->hwaddr_len = ETH_HWADDR_LEN;
  
  /* set MAC hardware address */
  netif->hwaddr[0] = 0x20;
  netif->hwaddr[1] = 0x00;
  netif->hwaddr[2] = 0x00;
  netif->hwaddr[3] = 0x00;
  netif->hwaddr[4] = 0x00;
  netif->hwaddr[5] = 0x00;

  /* maximum transfer unit */
  netif->mtu = MAX_MTU;
  
#if LWIP_ARP
  netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
#else 
  netif->flags |= NETIF_FLAG_BROADCAST;
#endif /* LWIP_ARP */

  s_xSemaphore = xSemaphoreCreateCounting(40, 0);
  sys_thread_new("ETHIN", 
                  ethernetif_input, 
                  netif, 
                  NETIF_IN_TASK_STACK_SIZE, 
                  NETIF_IN_TASK_PRIORITY);
#endif /* LWIP_ARP || LWIP_ETHERNET */

  /* Do whatever else is needed to initialize interface. */
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become available since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
  struct ethernetif *ethernetif = netif->state;
	(void)ethernetif;
  struct pbuf *q;
  uint16_t freeSize= getSn_TX_FSR(0);
  uint16_t length = p->tot_len;

  if (freeSize < length) {
    return ERR_BUF;
  }

  for (q = p; q != NULL; q = q->next) {
    /* Send the data from the pbuf to the interface, one pbuf at a
       time. The size of the data in each pbuf is kept in the ->len
       variable. */
    wiz_send_data(0, (uint8_t *)((uint8_t *)q->payload), q->len);
    // send data from(q->payload, q->len);
  }
  setSn_CR(0, Sn_CR_SEND);

  return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *
low_level_input(struct netif *netif)
{
  struct ethernetif *ethernetif = netif->state;
	(void)ethernetif;
  struct pbuf *p;
  u16_t len, framelen;

  uint16_t rxRd= getSn_RX_RD(0);
  len = getSn_RX_RSR(0);
  if(len < 4) {
    goto end;
  }
  printf("len:%0x\r\n", len);
  wiz_recv_data(0, (uint8_t *)&framelen, 2);
  printf("framelen before:%0x\r\n", framelen);
  setSn_CR(0, Sn_CR_RECV);
  framelen= (framelen << 8) | (framelen >> 8);
  printf("framelen after:%0x\r\n", framelen);
  if (framelen > 32000) {
    wiz_recv_ignore(0, framelen);
    setSn_CR(0, Sn_CR_RECV);
    goto end;
  }

  framelen -= 2;
  /* We allocate a pbuf chain of pbufs from the pool. */
  p = pbuf_alloc(PBUF_RAW, framelen, PBUF_RAM);

  if (p == NULL) {
    goto end;
  }

  wiz_recv_data(0, (uint8_t *)((uint8_t *)p->payload), framelen);
  setSn_CR(0, Sn_CR_RECV);

  return p;
#if 0
  if (p != NULL) {

    /* We iterate over the pbuf chain until we have read the entire
     * packet into the pbuf. */
    for (q = p; q != NULL; q = q->next) {
      /* Read enough bytes to fill this pbuf in the chain. The
       * available data in the pbuf is given by the q->len
       * variable.
       * This does not necessarily have to be a memcpy, you can also preallocate
       * pbufs for a DMA-enabled MAC and after receiving truncate it to the
       * actually received size. In this case, ensure the tot_len member of the
       * pbuf is the sum of the chained pbuf len members.
       */
      wiz_recv_data(0, (uint8_t *)((uint8_t *)q->payload), len);
      // memcpy(q->payload, q->len);
    }
  }
  setSn_CR(0, Sn_CR_RECV);
  return p;
#endif
end:
  return NULL;
}
#if NO_SYS
/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void ethernetif_input(struct netif *netif)
{
  err_t err;
  struct pbuf *p;
TRY_GET_NEXT_FRAGMENT:
  /* move received packet into a new pbuf */
  setSn_IR(0, 0x1F);
  setSIR(0);
  p = low_level_input(netif);
    
  /* no packet could be read, silently ignore this */
  if (p == NULL) return;
    
  /* entry point to the LwIP stack */
  err = netif->input(p, netif);
    
  if (err != ERR_OK)
  {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
    pbuf_free(p);
    p = NULL;    
  }
  else
  {
    goto TRY_GET_NEXT_FRAGMENT;
  }
  // struct ethernetif *ethernetif;
  // struct eth_hdr *ethhdr;
  // struct pbuf *p;

  // ethernetif = netif->state;

  // /* move received packet into a new pbuf */
  // p = low_level_input(netif);
  // /* if no packet could be read, silently ignore this */
  // if (p != NULL) {
  //   /* pass all packets to ethernet_input, which decides what packets it supports */
  //   if (netif->input(p, netif) != ERR_OK) {
  //     LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
  //     pbuf_free(p);
  //     p = NULL;
  //   }
  // }
}

#else
void ethernetif_input(void *pvParameters)
{
  struct netif *netif;
	struct pbuf *p = NULL;
  //uint8_t linkstate;
	netif = (struct netif*) pvParameters;
  LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
  
	while(1) 
  {
    // w5500_IR_clr();
    // volatile uint8_t phyReg = getPHYCFGR();
    // linkstate = phyReg & (1 << 0);
    // if (linkstate && !netif_is_link_up(netif))
    // {
    //   netif_set_link_up(netif);
    // }
    // else if (!linkstate && netif_is_link_up(netif))
    // {
    //   netif_set_link_down(netif);
    // }
    if(xSemaphoreTake( s_xSemaphore, portMAX_DELAY ) == pdTRUE)
    {
      /* move received packet into a new pbuf */
      printf("ethernetif_input\r\n");
      taskENTER_CRITICAL();
      w5500_IR_clr();
      p = low_level_input(netif);
      taskEXIT_CRITICAL();
      /* points to packet payload, which starts with an Ethernet header */
      if(p != NULL)
      {
        taskENTER_CRITICAL();
        /* full packet send to tcpip_thread to process */
        if (netif->input(p, netif) != ERR_OK)
        {
          LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
          pbuf_free(p);
          p = NULL;
        }
        taskEXIT_CRITICAL();
      }
    }
	}
}

#endif /* NO_SYS */


/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t
ethernetif_init(struct netif *netif)
{
  struct ethernetif *ethernetif;

  LWIP_ASSERT("netif != NULL", (netif != NULL));

  ethernetif = mem_malloc(sizeof(struct ethernetif));
  if (ethernetif == NULL) {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
    return ERR_MEM;
  }

  LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  // MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

  netif->state = ethernetif;
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */

#if LWIP_IPV4
#if LWIP_ARP || LWIP_ETHERNET
#if LWIP_ARP
  netif->output = etharp_output;
#else
  netif->output = low_level_output_arp_off;
#endif /* LWIP_ARP */
#endif /* LWIP_ARP || LWIP_ETHERNET */
#endif /* LWIP_IPV4 */

// #if LWIP_IPV4
//   netif->output = etharp_output;
// #endif /* LWIP_IPV4 */
#if LWIP_IPV6
  netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */


  netif->linkoutput = low_level_output;

  /* initialize the hardware */
  low_level_init(netif);
  ethernetif->ethaddr = (struct eth_addr *) & (netif->hwaddr[0]);

  return ERR_OK;
}

