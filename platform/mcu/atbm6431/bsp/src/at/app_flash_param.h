/**************************************************************************************************************
 * altobeam iot at cmd file 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 * Permission is hereby granted for use on Altobeam IOT 6421 only,
 * in which case ,it is free to  redistribute it and/or modify
 *
*****************************************************************************************************************/

#ifndef _FLASH_CONFIG_PARAM_H_
#define _FLASH_CONFIG_PARAM_H_

#include "flash.h"

 
#define FLASH_1M  1
#define FLASH_2M  2
#define FLASH_2M_FS  3

#define FLASH_PAGE_SIZE 256
/*
! this file define how to map flash code
!FLASH_USE_1MBYTE_SIZE the max flash size is 1M
!FLASH_USE_2MBYTE_SIZE the max flash size is 2M
*/
#ifdef FLASH_USE_1MBYTE_SIZE
#define FLASH_SIZE FLASH_1M
#include "app_flash_param_1M.h"
#elif defined(FLASH_USE_2MBYTE_SIZE)
#define FLASH_SIZE FLASH_2M
#include "app_flash_param_2M.h"
#elif defined(FLASH_USE_FS_2MFBYTE_SIZE)
#define FLASH_SIZE FLASH_2M_FS
#include "app_flash_param_2M_fs.h"
#else
//#define FLASH_SIZE FLASH_1M
#include "app_flash_param_1M.h"
#endif //FLASH_USE_1MBYTE_SIZE

#define BOOT_VALID_FLAG 				0x32A7
#define BOOTLOAD_VALID_FLAG 				(BOOT_VALID_FLAG+1)
#define FLASH_SECTOR_SIZE		0x001000u


/******************************************************/
struct iot_flash_info{
	u32 flash_boot_code_section_addr;
	u32 flash_boot_param_section_addr;
	u32 flash_code_section_addr;
	u32 flash_code1_section_addr;
	u32 flash_code2_section_addr;
	u32 flash_code1_section_len;
	u32 flash_code2_section_len;
	u32 flash_code_flag_section_addr;
	u32 flash_code1_update_section_addr;
	u32 flash_code1_update_section_len;
	u32 flash_code2_update_section_addr;
	u32 flash_code2_update_section_len;
	u32 flash_code_update_flag_section_addr;
	u32 flash_usr_data_section_addr;
	u32 flash_usr_data_section_len;
	u32 flash_usr_data_section2_addr;
	u32 flash_usr_data_section2_len;
	u32 boot_valid_flag;
	u32 flash_offset_mask;
	u32 flash_usr_key_bak_addr;
	u32 flash_usr_key_bak_len;
	u32 flash_boot_param_section2_addr;
	u32 flash_boot_param_section2_len;
};

enum CACHE_TYPE{
	NO_CACHE=0,
	CACHE_ITCM_128K=1,
	CACHE_ITCM_64K=2,
	CACHE_AHB_128K=3,
	CACHE_AHB_64K=4,
};

//FLASH_CODE_CONFIG_SECTION_ADDR
struct config_bin{
	u32 valid_boot_param;
	u32 cache_type;
	u32 flash_type;
	u32 flash_enc_enable;
};

struct iot_updata_param {
	u32 iot_flag;
	u32 iot_version;
	u32 iot_checksum;
	u32 iot_updataCnt;
	u32 iot_CODE1_len;
	u32 iot_CODE2_len;
};

struct iot_boot_param {
	u8 iot_info[32];
	u32 iot_version;
	int msgwaittimer;//wait fw download msg timer
	
	u8 bootdelay;
	u8 smartconfig;/*if used smart config before connect ap*/
	u8 b_usedMacaddr;/*if used Macaddr in flash*/
	u8 flash_size;
	
	u8 wifiMacaddr[6];/*mac addr used for wifi*/
	u8 reserve[1];	
	u8 b_SetupCount;
};


struct iot_uart_param {
	u32	uart_Baudrate;
	u8	uart_DataBits;
	u8	uart_Parity;
	u8	uart_StopBits;
	u8  reserve;
};

struct iot_tcpip_param {
	u16 static_ipaddr[2];
	u16 static_ipmask[2];
	u16 static_gwaddr[2];
	u16 static_dns[2];
	u16 iot_connect_ip[2];
	u16 iot_listen_port;
	u16 iot_connect_rport;
	u16 iot_connect_lport;
	u8 static_ip;/*1:static_ip,0 dhcp */
	u8 reserve;
};

struct wifi_configure {
	char ssid[32];
	u8 ssid_len;
	u8 password_len;
	char bssid[6/*ETH_ALEN*/];
	char password[64];
	
	u8 privacy;
	u8 psk_set;
	u8 bssid_set;
	u8 wifi_auto_connect;
	
	u8 ap_mode;
	u8 hw_fix_rate_id;
	u16 key_mgmt;
	
	int pairwise_cipher;
	int group_cipher;
	int wpa;
	u8  psk[32];
	
	u8 key_id;
	u8 reserve2;
	u16 auth_alg;
	
	u8 country_id;
	u8 channel_no;  
	u8 reserve3[2];
#ifdef CONFIG_WPS
	u8 dev_password[12];
	int dev_password_len;
	int pbc;
	int pin;
#endif
	u8 wifimode;/*0:sta mode ,other ap mode*/
	u8 system_sleep_mode; //standby power save enable
	u8 reserve[2];

};

struct TCP_socket_configure {
	char connect_class[4];
	char connect_addr[128];
	u32_t connect_port;
};

struct iot_device_param {
	u8 key_data[512];
	u16 revert;
	u16 chksum;
};

#define FLASH_CODE_ENCODED_FLAG 0x23247788
struct iot_flashcode_enc_param {
	u32	flashCodeEncoded;
	u8  flashEncKey[16];
};

#define IOT_BOOT_INITREG_LENMAX 64
struct iot_bootInitRegister{
	u32 len;
	u32 addr[IOT_BOOT_INITREG_LENMAX];
	u32 value[IOT_BOOT_INITREG_LENMAX];
};

/*
FLASH_BOOT_SECTION_ADDR
*/
struct flash_boot_const_section{
	struct iot_uart_param  no_use_uart_param;	
	int no_use_valid_uart_param;
	struct iot_boot_param  no_use_boot_param;
	int no_use_valid_boot_param;
	struct iot_device_param device_param;	
	int valid_device_param;

};


struct flash_boot_volatile_section{
	struct iot_tcpip_param STA_tcpip_param; 
	int valid_STA_tcpip_param;
	struct iot_tcpip_param AP_tcpip_param;
	int valid_AP_tcpip_param;
	struct wifi_configure  wifi_param;	
	int valid_wifi_param;
	struct TCP_socket_configure  TCP_socket_param;	
	int valid_TCP_socket_param;
	struct iot_bootInitRegister iot_bootInitRegister_param; 
	int valid_iot_bootInitRegister;
	struct iot_uart_param  uart_param;	
	int valid_uart_param;
	struct iot_boot_param  boot_param;
	int valid_boot_param;
	struct iot_flashcode_enc_param flashcode_enc_param; 
	int valid_flashcode_enc_param;
};

extern struct iot_flash_info flash_info;
extern  struct flash_boot_volatile_section iot_boot_volatile_sect;
extern  struct flash_boot_const_section iot_boot_const_sect;
NO_FLASH_FUNC void flash_param_config_save();
NO_FLASH_FUNC int  atbmwifi_flash_param_config_restore(void);
NO_FLASH_FUNC void atbmwifi_flash_param_wificonfig_change();
NO_FLASH_FUNC void atbmwifi_flash_param_uartconfig_change();
NO_FLASH_FUNC void atbmwifi_flash_param_staipconfig_change();
NO_FLASH_FUNC void atbmwifi_flash_param_apipconfig_change();
NO_FLASH_FUNC void atbmwifi_flash_param_bootconfig_change();
NO_FLASH_FUNC void atbmwifi_flash_param_deviceinfo_change();
NO_FLASH_FUNC int flash_param_read(u32 addr,u8 *data,int len);

NO_FLASH_FUNC int flash_param_write(u32 addr,u8 *data,int len);
//UPDATE_CODE1 write
NO_FLASH_FUNC void flash_updatafw1_write(u32 offset,u8 *data,int len);
//UPDATE_CODE2 write
NO_FLASH_FUNC void flash_updatafw2_write(u32 offset,u8 *data,int len);
//update_param2 write
/*
如果更新的code 不对返回HAL_ERR_UNAVAILABLE
如果flash 操作错误，返回HAL_FAILURE
成功返回HAL_SUCCESS
*/
NO_FLASH_FUNC int flash_updatafwFlag_write();
/*
如果更新的code 不对返回HAL_ERR_UNAVAILABLE
如果flash 操作错误，返回HAL_FAILURE
参数reboot=1，更新完成之后reboot 系统
成功返回HAL_SUCCESS
*/
NO_FLASH_FUNC int flash_updatafwFlag_reboot(int reboot);

//UPDATE_CODE1 _erase
NO_FLASH_FUNC void flash_updatafw1_block_erase();
//UPDATE_CODE2 _erase
NO_FLASH_FUNC void flash_updatafw2_block_erase();
#if (CHIP_TYPE==CHIP_ARES_B)
FLASH_FUNC int atbmwifi_flash_chip_deepSleep();
#endif


extern int flash_page_program(u32 addr,u32 *wbuf,u32 wcnt);
extern int flash_sector_erase(u32 addr);
extern int flash_read(u32 addr,u32 *rbuf,u32 rcnt);

#endif /*_FLASH_CONFIG_PARAM_H*/
