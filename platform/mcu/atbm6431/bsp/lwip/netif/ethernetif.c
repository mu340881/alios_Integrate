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

#include <string.h>
#include "lwip/opt.h"
#include "netif.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "lwip/ethip6.h"
#include "netif/etharp.h"
#include "rtos-comm/include/debug.h"
#include "rtos-freertos/api/skbuf.h"
#include "rtos-comm/driver/wifi/mod_hmac/atbm_wifi_driver_api_export.h"
#include "app_flash_param.h"

#define iot_printf printf

//#include "cfg80211.h"
//#include "wpa.h"
/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 't'

//#define OS_SKB_LEN(skb)   (skb)->data_len
//#define OS_SKB_DATA(skb)   (((skb)->payload))

//#include <osal.h>
//#include "netstack.h"
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

/* Forward declarations. */
void ethernetif_input(struct netif *netif,struct pbuf *p);

//add by wp 
hal_mutex_t lwip_mutex;
int lwip_queue_enable = 0;
int lwip_enable = 0;

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */

//extern OS_EVENT  *pool_alloc_sem;

static void
low_level_init(struct netif *netif)
{
   
  /* set MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;

  
  /* maximum transfer unit */
  netif->mtu = 1500;
  
  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP |NETIF_FLAG_LINK_UP;
 
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
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

extern u16_t htons(u16_t n);

NOCACHE_NOFLASH_FUNC static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
  struct eth_hdr *ethhdr;
  struct pbuf *skb = p;

#if ETH_PAD_SIZE
  pbuf_header(skb, -ETH_PAD_SIZE); /* drop the padding word */
#endif

  if((skb->tot_len< 14)||(skb->tot_len> 1538)){
  	iot_printf("<lwip>:low_tx short %d\n",skb->tot_len);
	return ERR_BUF;
  }

  if(!atbm_wifi_is_connect_ok()){		
		return ERR_CONN;
  }
  
  //if the low wifi stop the queue, wait mutex 2 second
  if(!lwip_queue_enable && lwip_enable){
	  iot_printf("<lwip>:drop tx protocol %d\n",skb->protocol);
	  hal_sleep(10);
	  return ERR_BUF;
  }  

  /*if have more than one skb link, copy it to one skb*/
 // if((skb->next != NULL)||(skb_headroom(skb)<PBUF_WIFI_HEADROOM)||(p->ref > 1))
  {
  	 int len = 0;
	 struct pbuf *tmpskb = p;
  	 skb = dev_alloc_skb(p->tot_len);
	 if(skb == NULL){
	 	iot_printf("<lwip>low_tx no mem ERROR!! %d\n",p->tot_len);
		return ERR_MEM;
	 }
	 do {
	 	memcpy((u8 *)OS_SKB_DATA(skb)+len,OS_SKB_DATA(tmpskb) ,tmpskb->len);
		len += tmpskb->len;
		tmpskb = tmpskb->next;
	 }	 while(tmpskb != NULL);
	 
  }
 //  else {  
 //	pbuf_ref(skb);  
 // }

  ethhdr = (struct eth_hdr *)OS_SKB_DATA(skb);
  skb->protocol = htons(ethhdr->type);
  
  KASSERT(skb->next == NULL);
  KASSERT(skb_headroom(skb)>=PBUF_WIFI_HEADROOM);
  


  
  skb->tail = OS_SKB_DATA(skb);
  OS_SKB_LEN(skb) = 0;
  skb_put(skb,skb->len);
  
  //wifi_printk_time(WIFI_TX, "low_level_output\n");
  
  //hal_wait_for_mutex(&lwip_mutex,100);  
  netif->netdev_ops->ndo_start_xmit(netif, skb); 
 // hal_release_mutex(&lwip_mutex);


#if ETH_PAD_SIZE
  pbuf_header(skb, ETH_PAD_SIZE); /* reclaim the padding word */
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
static inline void
low_level_input(struct netif *netif)
{
  LINK_STATS_INC(link.recv);
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
#define ETH_FRAME_LEN	1514


NO_FLASH_FUNC1 void ethernetif_input(struct netif *netif,struct pbuf *p)
{
  struct eth_hdr *ethhdr;
  
  //wifi_printk_time(WIFI_RX, "ethernetif_input\n");

  if(OS_SKB_LEN(p) > ETH_FRAME_LEN)
  {  	
	pbuf_free(p);
  	iot_printf("ethernetif_input to long:%d\n",OS_SKB_LEN(p));
	return;
  }
  low_level_input(netif);
  //
  //set pbuf len to real data len
  p->tot_len = OS_SKB_LEN(p);
  p->len = p->tot_len;   
  /* points to packet payload, which starts with an Ethernet header */
  ethhdr = (struct eth_hdr *)OS_SKB_DATA(p);
 // if(!is_multicast_ether_addr(ethhdr->dest.addr)){
  //	iot_printf("ethinput %x len%d\n",htons(ethhdr->type),p->tot_len);
  //}
  switch (htons(ethhdr->type)) {
  /* IP or ARP packet? */
  case ETHTYPE_IP:
  case ETHTYPE_ARP:
  case ETHTYPE_EAPOL:
#if LWIP_IPV6
  case ETHTYPE_IPV6:	
#endif	//LWIP_IPV6
#if PPPOE_SUPPORT
  /* PPPoE packet? */
  case ETHTYPE_PPPOEDISC:
  case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */
    /* full packet send to tcpip_thread to process */
    //INC_MONITOR_ITEM_VALUE(recv_packets_cnt);
    if (netif->input(p, netif)!=ERR_OK)
     { LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
       pbuf_free(p);
       p = NULL;
     }
    break;

  default:
    pbuf_free(p);
    p = NULL;
    break;
  }
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
FLASH_FUNC err_t
ethernetif_init(struct netif *netif)
{
  struct ethernetif *ethernetif;

  LWIP_ASSERT("netif != NULL", (netif != NULL));
    
  ethernetif = (struct ethernetif *)mem_malloc(sizeof(struct ethernetif));
  
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
  NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

  netif->state = ethernetif;
  //netif->name[0] = IFNAME0;
  //netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
#if LWIP_IPV6
  netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
  netif->linkoutput = low_level_output;
  
  ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);
  
  /* initialize the hardware */
  low_level_init(netif);

  return ERR_OK;
}

FLASH_FUNC void atbm_lwip_init(struct net_device *dev)
{
	hal_create_mutex(&lwip_mutex);
}
extern  struct flash_boot_volatile_section iot_boot_volatile_sect;
extern err_t dhcp_start(struct netif *netif);
extern int atbmwifi_event_indicate(int vid, enum WIFI_EVENT_ID event);

FLASH_FUNC void atbm_lwip_enable(struct net_device *dev)
{
	int b_dhcp_start = 0;
	
	lwip_queue_enable = 1;
	lwip_enable = 1;
	
	if(atbm_wifi_is_iftype_ap()/*&& 
			!iot_boot_volatile_sect.AP_tcpip_param.static_ip*/){
			//APmode not start dhcp client
			b_dhcp_start = 0;		
	}
	else if(atbm_wifi_is_iftype_station()&& 
			!iot_boot_volatile_sect.STA_tcpip_param.static_ip){
			int static_ipaddr = 0;
			
			b_dhcp_start = 1;	
			//clear ip addr
			netif_set_addr(dev, ( const ip4_addr_t *)&static_ipaddr,( const ip4_addr_t *)&static_ipaddr,( const ip4_addr_t *)&static_ipaddr);
	}
	netif_set_up(dev);

	if(b_dhcp_start){
		if(dhcp_start(dev) != ERR_OK)
		{
			iot_printf("dhcp start error\n");
		}
		else
		{
			iot_printf("dhcp start success\n");
		}
	}
	else {		
		//let lmac can sleep
		atbm_clear_wifi_powerbusy_event();
		//
		atbmwifi_event_indicate(0,WIFI_CONNECT_GOT_IP);
	}
}


void atbm_lwip_disable(struct net_device *dev)
{
	netif_set_down(dev);
	//lwip_queue_enable = 0;
	lwip_enable = 0;
	lwip_queue_enable = 0;
}

NO_FLASH_FUNC void atbm_lwip_txdone(struct net_device *dev)
{
}

void atbm_lwip_wake_queue(struct net_device *dev,int num)
{
	if(!lwip_queue_enable && lwip_enable){
		//hal_release_mutex(&lwip_mutex);
		lwip_queue_enable = 1;
		LWIP_DEBUGF(NETIF_DEBUG, ("<lwip>:wake queue \n"));
	}
}

void atbm_lwip_stop_queue(struct net_device *dev,int num)
{
	if(lwip_queue_enable && lwip_enable){
		//hal_wait_for_mutex(&lwip_mutex,2000);
		lwip_queue_enable = 0;
		LWIP_DEBUGF(NETIF_DEBUG, ("<lwip>:STOP queue \n"));
	}
}

void atbm_lwip_task_event(struct net_device *dev)
{
}


NOCACHE_NOFLASH_FUNC int is_zero_ether_addr(const u8 *addr)
{
	return !(addr[0] | addr[1] | addr[2] | addr[3] | addr[4] | addr[5]);
}
NOCACHE_NOFLASH_FUNC int is_valid_ether_addr(const u8 *addr)
{
	/* FF:FF:FF:FF:FF:FF is a multicast address so we don't need to
	 * explicitly check for it here. */
	return !is_multicast_ether_addr(addr) && !is_zero_ether_addr(addr);
}
void netif_tx_stop_all_queues(struct netif *netif)
{
}
void netif_tx_wake_all_queues(struct netif *netif)
{
}
void netif_tx_start_all_queues(struct netif *netif)
{
}
extern struct net_device_ops wifi_net_ops;
struct netif * alloc_netdev(int size)
{
	struct netif *  netdev =malloc(size + sizeof(struct netif));
	
	KASSERT((netdev != NULL));
	if(netdev)
		memset(netdev,0,(size + sizeof(struct netif)));
	
	memcpy(netdev->name,"AB",2);	
	
	netdev->netdev_ops =(struct net_device_ops *) &wifi_net_ops;
	return  netdev;
}

void free_netdev(struct netif * netdev)
{
	if(netdev != NULL)
		os_free(netdev);
}


void netdev_attach_ops (struct netif * netdev, struct net_device_ops * net_ops)
{
	netdev->netdev_ops = net_ops;
}


NOCACHE_NOFLASH_FUNC void *netdev_drv_priv(struct netif * netdev)
{
	return netdev->drv_priv;
}

void netdev_set_macaddr (struct netif * netdev,u8 *mac)
{
	memcpy(netdev->hwaddr,mac,6);
}
u8 *netdev_get_macaddr (struct netif * netdev)
{
	
	return(netdev->hwaddr);
}

struct tcpip_opt lwip_tcp_opt ={
	.net_init = atbm_lwip_init,
	.net_enable = atbm_lwip_enable,//
	.net_disable = atbm_lwip_disable,//
	.net_rx = ethernetif_input,
	.net_tx_done = 	atbm_lwip_txdone,
	.net_start_queue =	atbm_lwip_wake_queue,
	.net_stop_queue =	atbm_lwip_stop_queue,
	.net_task_event =	atbm_lwip_task_event,//
};

