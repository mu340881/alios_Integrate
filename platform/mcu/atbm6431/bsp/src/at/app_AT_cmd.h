/**************************************************************************************************************
 * altobeam iot at cmd file 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 * Permission is hereby granted for use on Altobeam IOT 6421 only,
 * in which case ,it is free to  redistribute it and/or modify
 *
*****************************************************************************************************************/

#ifndef _APP_AT_CMD_H_
#define _APP_AT_CMD_H_


#include "stdlib.h"
#include "hal.h"
#include "os_api.h"
#include "cli.h"
#include "app_flash_param.h"
#include "app_wifi.h"


#define MAX_GPIO_PIN_NUM 23
#define FLASH_SAVA_FLAG  1
#define FLASH_UNSAVA_FLAG  0
#define UN_USED_PARAMS (void)

#ifndef TSPT_RX_BUF_SIZE
#if CONFIG_ALI_OS 
#define TSPT_RX_BUF_SIZE      64
#else
#define TSPT_RX_BUF_SIZE      2048 
#endif 
#endif

struct iot_ip_param{
	u32 static_ipaddr;
	u32 static_ipmask;
	u32 static_gwaddr;
};


extern  struct flash_boot_volatile_section iot_boot_volatile_sect;
extern  struct flash_boot_const_section iot_boot_const_sect;
extern  struct iot_ip_param ip_params_user_set;

/*wifi config*/
extern struct wifi_configure* hmac_cfg;


/*********************************************************/
/*********************************************************/

FLASH_FUNC void AT_SmartConfigStart(char *Args);

#define UART_SendString(...) do{\
									u8 buffer[200]={0};\
									sprintf((char *)buffer,__VA_ARGS__);\
									UART_TSPT_SendStr(buffer);}while(0)

FLASH_FUNC void UART_Send_status(int ret);
FLASH_FUNC int hexstr2bin(u8 *buf, const char *hex, size_t len);
FLASH_FUNC void AT_WDisConnect(char *pLine);
char * get_country_str(int country_id);	

#endif /*_APP_AT_CMD_H_*/
