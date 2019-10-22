/**************************************************************************************************************
 * altobeam iot at cmd file 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 * Permission is hereby granted for use on Altobeam IOT 6421 only,
 * in which case ,it is free to  modify it
 *
*****************************************************************************************************************/


#ifndef WIFI_API_H
#define WIFI_API_H
#include "app_flash_param.h"
#include "atbm_wifi_driver_api_export.h"
#include "os_net_type.h"
/*
enum WIFI_EVENT_ID {
	WIFI_CONNECT_SUCCESS,
	WIFI_CONNECT_FAIL,
	WIFI_CONNECT_LOSS,
	WIFI_STA_ADD_EVENT,//just in ap mode
	WIFI_STA_LOSS_EVENT,//just in ap mode
	WIFI_SCAN_COMPLETE_EVENT,//just in sta mode
	WIFI_CHANGE_MODE_SUCCESS,
	WIFI_CREATE_AP_SUCCESS,
	WIFI_SMARTCONFIG_SUCCESS,
	WIFI_SMARTCONFIG_FAIL,
	WIFI_EVENT_MAX,
};
*/


typedef void (*ATBM_WIFI_EVENT_HANDLER)(void);
/*
change wifi mode
mode :0 stamode
mode : 1 APmode
*/
int atbm_wifi_change_mode(u8 mode);
int atbmwifi_event_indicate(int vid, enum WIFI_EVENT_ID event);
/*
因为EventHandler ，是在hmac task中被调用的所以，不能在EventHandler 函数调用socket，wifi等含有等待的函数，
原因是wifi等待函数，或者socket 的函数，通常是等待hmac task 处理结束，但是因为EventHandler 本身就是在hmac task中
很容易变成自己等自己,正确的做法是在EventHandler 中创建task 
*/
void atbmwifi_RegisterEventHandler(enum WIFI_EVENT_ID event, ATBM_WIFI_EVENT_HANDLER EventHandler);
int atbmwifi_wait_connect(/*struct netif * netdev*/);
int atbm_wifi_wait_get_ipaddr_ok(/*struct netif * netdev*/);
u32_t atbm_wifi_get_ipaddr(unsigned char *ipdata);
int atbmwifi_init();
int hmac_main();
void flash_param_config_init();
void flash_init_check_user_key(void);
int get_host_by_name(char *hostname, uint32_t *addr);
void flash_param_config_reset();

#endif //WIFI_API_H
