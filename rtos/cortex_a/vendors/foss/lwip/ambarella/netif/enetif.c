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

#include "lwip/opt.h"

#if 1 /* don't build, this is only a skeleton, see previous comment */

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "lwip/etharp.h"
#include "netif/ppp/pppoe.h"

#if defined(CONFIG_LWIP_ENET)
#include <AmbaTypes.h>
#include <io/AmbaENET_Def.h>
#include <io/AmbaENET.h>
#include <string.h>
#endif

#include <svc/comsvc/print/AmbaPrint.h>

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
#if defined(CONFIG_LWIP_ENET)
  AMBA_ENET_CONFIG_s *pEnetConfig;
  struct netif *netif;
#endif
};
#if defined(CONFIG_LWIP_ENET)
struct ethernetif *priv_data[2];
#endif

/* Forward declarations. */
#if !defined(CONFIG_LWIP_ENET)
static void  ethernetif_input(struct netif *netif);
#else
UINT32 ethernetif_input(const UINT32 Idx, const void *frame,  const UINT16 RxLen, const UINT16 type);
#endif

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
  struct ethernetif *ethernetif = netif->state;
#if defined(CONFIG_LWIP_ENET)
  AMBA_ENET_CONFIG_s *pEnetConfig;
  UINT32 Idx = 0U;

  if ((netif->name[0] - '0') == 1) {
    Idx = 1U;
  }

  (void) AmbaEnet_GetConfig(Idx, &pEnetConfig);

  ethernetif->pEnetConfig = pEnetConfig;
  ethernetif->netif = netif;
  //pEnetConfig->pRxCb = ethernetif_input;
  memcpy(netif->hwaddr, pEnetConfig->Mac, 6);
#endif
  /* set MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;
#if !defined(CONFIG_LWIP_ENET)
  /* set MAC hardware address */
  netif->hwaddr[0] = ;
  ...
  netif->hwaddr[5] = ;
#endif
  /* maximum transfer unit */
  netif->mtu = ETH_FRAMES_SIZE - 14U;

  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

#if LWIP_IPV6 && LWIP_IPV6_MLD
  /*
   * For hardware/netifs that implement MAC filtering.
   * All-nodes link-local is handled by default, so we must let the hardware know
   * to allow multicast packets in.
   * Should set mld_mac_filter previously. */
  if (netif->mld_mac_filter != NULL) {
    ip6_addr_t ip6_allnodes_ll;
    ip6_addr_set_allnodes_linklocal(&ip6_allnodes_ll);
    netif->mld_mac_filter(netif, &ip6_allnodes_ll, NETIF_ADD_MAC_FILTER);
  }
#endif /* LWIP_IPV6 && LWIP_IPV6_MLD */

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
  struct pbuf *q;
/* TXFASTPATH: udp tx zero copy example */
//#define TXFASTPATH
#if defined(CONFIG_LWIP_ENET)
  UINT32 Idx = ethernetif->pEnetConfig->Idx;
#if !defined(TXFASTPATH)
   void *ptr;
   UINT32 err;
#endif
#else
  initiate transfer();
#endif
#if ETH_PAD_SIZE
  pbuf_remove_header(p, ETH_PAD_SIZE); /* drop the padding word */
#endif

#if defined(CONFIG_LWIP_ENET)
  if (p->tot_len > ETH_FRAMES_SIZE) {
    AmbaPrint_PrintUInt5("Err: %d > %d", p->tot_len, ETH_FRAMES_SIZE, 0U, 0U, 0U);
  }
#if !defined(TXFASTPATH)
  err = AmbaEnet_GetTxBuf(Idx, &ptr);
  if (err != 0U) {
    AmbaPrint_PrintStr5("AmbaEnet_GetTxBuf failed", NULL, NULL, NULL, NULL, NULL);
    return ERR_MEM;
  }
#endif
#endif
  for (q = p; q != NULL; q = q->next) {
    /* Send the data from the pbuf to the interface, one pbuf at a
       time. The size of the data in each pbuf is kept in the ->len
       variable. */
#if !defined(CONFIG_LWIP_ENET)
    send data from(q->payload, q->len);
#else
#if defined(TXFASTPATH)
    UINT8 FS = 0U, LS = 0U;
    const UINT8 CacheClean = 1U; //set to 0 when data is not in CPU cache
    if (q == p) {
      FS = 1U;
    }
    if (q->next == NULL) {
      LS = 1U;
    }

    (void)AmbaEnet_SetupTxDesc(Idx, q->payload, q->len, FS, LS, CacheClean);
#else
    memcpy(ptr, q->payload, q->len);
    ptr = (char *)ptr + q->len;
#endif
#endif
  }

#if !defined(CONFIG_LWIP_ENET)
  signal that packet should be sent();
#else
#if !defined(TXFASTPATH)
  (void) AmbaEnet_Tx(Idx, p->tot_len);
#endif
#endif
  MIB2_STATS_NETIF_ADD(netif, ifoutoctets, p->tot_len);
  if (((u8_t *)p->payload)[0] & 1) {
    /* broadcast or multicast packet*/
    MIB2_STATS_NETIF_INC(netif, ifoutnucastpkts);
  } else {
    /* unicast packet */
    MIB2_STATS_NETIF_INC(netif, ifoutucastpkts);
  }
  /* increase ifoutdiscards or ifouterrors on error */

#if ETH_PAD_SIZE
  pbuf_add_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

  LINK_STATS_INC(link.xmit);

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
#if !defined(CONFIG_LWIP_ENET)
static struct pbuf *
low_level_input(struct netif *netif)
#else
static struct pbuf *
low_level_input(struct netif *netif, const void *ptr, const UINT16 RxLen)
#endif
{
#if !defined(CONFIG_LWIP_ENET)
  struct ethernetif *ethernetif = netif->state;
#endif
  struct pbuf *p, *q;
  u16_t len;

  /* Obtain the size of the packet and put it into the "len"
     variable. */
#if !defined(CONFIG_LWIP_ENET)
  len = ;
#else
  (void)netif;
  len = RxLen;
#endif
#if ETH_PAD_SIZE
  len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

  /* We allocate a pbuf chain of pbufs from the pool. */
  p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

  if (p != NULL) {

#if ETH_PAD_SIZE
    pbuf_remove_header(p, ETH_PAD_SIZE); /* drop the padding word */
#endif

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
#if !defined(CONFIG_LWIP_ENET)
      read data into(q->payload, q->len);
#else
      memcpy(q->payload, ptr, q->len);
      ptr = (char *)ptr + q->len;
#endif
    }
#if !defined(CONFIG_LWIP_ENET)
    acknowledge that packet has been read();
#endif

    MIB2_STATS_NETIF_ADD(netif, ifinoctets, p->tot_len);
    if (((u8_t *)p->payload)[0] & 1) {
      /* broadcast or multicast packet*/
      MIB2_STATS_NETIF_INC(netif, ifinnucastpkts);
    } else {
      /* unicast packet*/
      MIB2_STATS_NETIF_INC(netif, ifinucastpkts);
    }
#if ETH_PAD_SIZE
    pbuf_add_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

    LINK_STATS_INC(link.recv);
  } else {
#if !defined(CONFIG_LWIP_ENET)
    drop packet();
#endif
    LINK_STATS_INC(link.memerr);
    LINK_STATS_INC(link.drop);
    MIB2_STATS_NETIF_INC(netif, ifindiscards);
  }

  return p;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
#if !defined(CONFIG_LWIP_ENET)
static void
ethernetif_input(struct netif *netif)
#else
UINT32 ethernetif_input(const UINT32 Idx, const void *frame,  const UINT16 RxLen, const UINT16 type)
#endif
{
#if !defined(CONFIG_LWIP_ENET)
  struct ethernetif *ethernetif;
  struct eth_hdr *ethhdr;
  struct pbuf *p;

  ethernetif = netif->state;
#else
  struct ethernetif *ethernetif = priv_data[Idx];
  struct netif *netif;
  struct pbuf *p;
  (void)type;
  if(ethernetif == NULL) {
     return 0;
  }
  netif = ethernetif->netif;
#endif

  /* move received packet into a new pbuf */
#if !defined(CONFIG_LWIP_ENET)
  p = low_level_input(netif);
#else
  p = low_level_input(netif, frame, RxLen);
#endif
  /* if no packet could be read, silently ignore this */
  if (p != NULL) {
    u32_t err;

    LOCK_TCPIP_CORE();
    /* pass all packets to ethernet_input, which decides what packets it supports */
    err = netif->input(p, netif);
    UNLOCK_TCPIP_CORE();

    if (err != ERR_OK) {
      LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
      pbuf_free(p);
      p = NULL;
    }
  }
  return 0U;
}

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
#if defined(CONFIG_LWIP_ENET)
  if ((netif->name[0] - '0') == 1) {
    priv_data[1] = ethernetif;
  } else {
    priv_data[0] = ethernetif;
  }
#endif
  if (ethernetif == NULL) {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
    return ERR_MEM;
  }

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

  netif->state = ethernetif;
#if !defined(CONFIG_LWIP_ENET)
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
#endif
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
#if LWIP_IPV4
  netif->output = etharp_output;
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
  netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
  netif->linkoutput = low_level_output;

  ethernetif->ethaddr = (struct eth_addr *) & (netif->hwaddr[0]);

  /* initialize the hardware */
  low_level_init(netif);

  return ERR_OK;
}

#endif /* 0 */
