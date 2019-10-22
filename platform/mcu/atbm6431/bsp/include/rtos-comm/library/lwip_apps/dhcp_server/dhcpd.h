/**************************************************************************************************************
 * altobeam iot user app file 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 * Permission is hereby granted for use on Altobeam IOT 6421 only,
 * in which case ,it is free to  modify it
 *
*****************************************************************************************************************/
#ifndef __DHCP_SERVER_H__
#define __DHCP_SERVER_H__

#include "hal.h"


#define DHCP_SERVER_SUPPORT_IP_TABLE_CNT	16
#define DHCP_SERVER_LEASETIME  3600 


#define DHCP_MAGIC			0x63825363
#define DHCP_REPLY          2




#define DHCPD_HDR_LEN offsetof(struct dhcp_msg,options)



int atbmwifi_init_dhcp_server(struct netif *netif);
int atbmwifi_free_dhcp_server(struct netif *netif);
void atbmwifi_enable_dhcp_server(void);
void atbmwifi_disable_dhcp_server(void);


		
#endif //__DHCP_SERVER_H__
