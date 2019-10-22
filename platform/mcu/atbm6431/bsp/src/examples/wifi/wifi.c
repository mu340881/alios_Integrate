/**************************************************************************************************************
 * altobeam iot at cmd file 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 * Permission is hereby granted for use on Altobeam IOT 6421 only,
 * in which case ,it is free to  modify it
 *
*****************************************************************************************************************/


#include <netif.h>
#include "hal.h"
#include "cli.h"
#include "lwip/init.h"
#include "lwip/dhcp.h"
#include "lwip/tcpip.h"
#include "lwip/inet.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "lwip/inet_chksum.h"
#include "wifi_api.h"
#include "os_api.h"
#include "app_AT_cmd.h"





extern err_t ethernetif_init(struct netif *netif);

DTCM_ZI  struct netif * g_wifinetif[MAX_VMAC_NUM];

#define if_netdev2index(_s)	(0)


FLASH_FUNC int  register_netdevice(struct netif * netdev)
{
	int ifindex = if_netdev2index(netdev);
	ip4_addr_t ip_addr;

	wifi_printk(WIFI_DBG_MSG,"register_netdevice %x\n",netdev);
	ip_addr.addr =0;

	g_wifinetif[ifindex] = netdev;	

	if(netif_add(netdev, &ip_addr, &ip_addr, &ip_addr, 0, ethernetif_init, tcpip_input) == NULL){
		DEBUG(1,1,"netif_add failed\n");
		return -1;
	}
	netif_set_default(netdev);
#ifdef ARDUINO
  	netdev->flags |= NETIF_FLAG_IGMP;
#endif //#ifndef ARDUINO
	//netif_set_up(netdev);
	return 0;	
}

FLASH_FUNC void unregister_netdevice(struct netif * netdev)
{
	int ifindex =  if_netdev2index(netdev);
	netif_set_down(netdev);
	netif_set_default(NULL);	
	g_wifinetif[ifindex] = NULL;
	return ;
}

FLASH_FUNC int atbmwifi_wait_connect(/*struct netif * netdev*/)
{
	while(/*loop-->0*/1)
	{
		if(!atbm_wifi_is_iftype_station()){
			return TRUE;
		}
		if(atbm_wifi_is_connect_ok())
		{
			return TRUE;
		}
		hal_sleep(100);
	}
	return FALSE;
}

FLASH_FUNC int atbm_wifi_wait_get_ipaddr_ok(/*struct netif * netdev*/)
{
	int loop = 300;
	while(loop-->0)
	{
		if(!atbm_wifi_is_iftype_station()){
			return TRUE;
		}
		if(iot_boot_volatile_sect.STA_tcpip_param.static_ip)
		{
			return TRUE;
		}
		else {
			if(dhcp_supplied_address(g_wifinetif[0]) ){
				return TRUE;
			}
		}
		hal_sleep(100);
	}
	return FALSE;
}

FLASH_FUNC u32_t atbm_wifi_get_ipaddr(unsigned char *ipdata)
{
	  // struct ip_addr ipaddr;
	
		if(!atbm_wifi_is_iftype_station()){
				iot_printf(" atbm_wifi_get_ipaddr....return not sta.!\n");
			return 0;
		}
		if(iot_boot_volatile_sect.STA_tcpip_param.static_ip)
		{
			iot_printf(" atbm_wifi_get_ipaddr....return static address.!\n");
			//memcpy((unsigned char *)ipdata, (unsigned char *)iot_boot_volatile_sect.STA_tcpip_param.static_ipaddr, sizeof(u32_t));
			return TRUE;
		}
		else {
			if(dhcp_supplied_address(g_wifinetif[0]) ){
				memcpy((unsigned char *)ipdata,(char *)&g_wifinetif[0]->ip_addr.addr, sizeof(u32_t));
				iot_printf("input IP %x\r\n",g_wifinetif[0]->ip_addr.addr);
				return TRUE;
			}
		}
	
	return FALSE;
}

FLASH_FUNC int atbm_wifi_is_get_ipaddr_ok(/*struct netif * netdev*/)
{
	//int loop = 1;
	//while(loop-->0)
	{
		if(!atbm_wifi_is_iftype_station()){
			return TRUE;
		}
		if(iot_boot_volatile_sect.STA_tcpip_param.static_ip)
		{
			return TRUE;
		}
		else {
			if(dhcp_supplied_address(g_wifinetif[0]) ){
				return TRUE;
			}
		}
		hal_sleep(100);
	}
	return FALSE;
}



static ATBM_WIFI_EVENT_HANDLER atbmwifi_EventDispatchTable[WIFI_EVENT_MAX] = {0};
FLASH_FUNC void atbmwifi_RegisterEventHandler(enum WIFI_EVENT_ID event, ATBM_WIFI_EVENT_HANDLER EventHandler)
{
    atbmwifi_EventDispatchTable[event] = EventHandler;
}
int g_wifi_status;

FLASH_FUNC int atbmwifi_get_status()
{
	if(atbm_wifi_is_connect_ok()){
		if(dhcp_supplied_address(g_wifinetif[0]) )
			return WIFI_CONNECT_SUCCESS;
	}
	if(g_wifi_status != WIFI_CONNECT_SUCCESS)
		return g_wifi_status;
	else
		return WIFI_CONNECT_LOSS;
}

FLASH_FUNC int atbmwifi_event_indicate(int vid, enum WIFI_EVENT_ID event)
{
	ATBM_WIFI_EVENT_HANDLER event_handler;
	static bool connect_flag=0;

	g_wifi_status = event;

	if(event == WIFI_CONNECT_SUCCESS){
		connect_flag= 1;
	}
	if(event == WIFI_CONNECT_GOT_IP){
		if(connect_flag ==0){
			//this is may a redhcp ,just drop it
			//iot_printf("retry WIFI_CONNECT_GOT_IP,drop \n");
			return 0;
		}
		//just the first dhcp event after connect need to callback
		connect_flag = 0;
		//iot_printf("WIFI_CONNECT_GOT_IP event\n");
	}
	
	event_handler = atbmwifi_EventDispatchTable[event];
	
	if(event_handler != NULL)
	{	
		(*event_handler)();
	}
	
	/*
	switch(event){
	case WIFI_CONNECT_SUCCESS:
		break;
		
	case WIFI_CONNECT_FAIL:
		break;
		
	case WIFI_CONNECT_LOSS:
		break;
		
	case WIFI_CHANGE_MODE_SUCCESS:
		break;
		
	case WIFI_CREATE_AP_SUCCESS:
		break;
		
	case WIFI_SMARTCONFIG_SUCCESS:
		break;
		
	
	case WIFI_SMARTCONFIG_FAIL:
		break;
	case WIFI_SCAN_COMPLETE_EVENT:
		break;
	default:
		break;
	}*/
	return 0;
}


FLASH_FUNC int atbmwifi_init()
{	
	int ret;
	int i =0;
	
	for(i =0 ;i<MAX_VMAC_NUM;i++){
		g_wifinetif[i] = NULL; 
	}
	wifi_printk(WIFI_DBG_INIT,"atbmwifi_init \n");

	ret = hmac_main();
	if(ret<0){
		KASSERT(0);
		return -1;
	}

	return 0;
}


