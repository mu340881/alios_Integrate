/**************************************************************************************************************
 * altobeam iot at cmd file 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 * Permission is hereby granted for use on Altobeam IOT 6421 only,
 * in which case ,it is free to  redistribute it and/or modify
 *
*****************************************************************************************************************/

#include "hal.h"
#include "os_net_type.h"
#include "app_flash_param.h"
#include "atbm_wifi_driver_api_export.h"
#include "app_wifi_config.h"
#include "atbm_etf.h"

#define BLOCK_ERASE_EXTERN   0
/*所有的flash code block都erase，在函数中等待的时间太长有时候会导致，wifi 断网，所有把等待函数分散到每次写函数中*/
//just erase updata flag SECTOR

struct iot_updata_param_flag {
	struct iot_updata_param iot_updata;
	struct iot_updata_param updata_rx;
};

extern struct wifi_configure *hmac_cfg;
extern  struct flash_boot_volatile_section iot_boot_volatile_sect;
extern  struct flash_boot_const_section iot_boot_const_sect;

struct iot_device_param flash_user_key_bak;

void atbm_wifi_not_connect_ap_auto()
{
	iot_boot_volatile_sect.wifi_param.wifi_auto_connect =0;
}

void atbm_wifi_connect_ap_auto()
{
	iot_boot_volatile_sect.wifi_param.wifi_auto_connect =1;
}

FLASH_FUNC void flash_param_config_init()
{
	struct flash_boot_volatile_section *flash_volatile_data = (struct flash_boot_volatile_section *)flash_info.flash_boot_param_section2_addr;
	struct flash_boot_const_section *flash_const_data = (struct flash_boot_const_section *)flash_info.flash_boot_param_section_addr;

	os_memcpy(&iot_boot_volatile_sect,flash_volatile_data,sizeof(struct flash_boot_volatile_section));
	os_memcpy(&iot_boot_const_sect,flash_const_data,sizeof(struct flash_boot_const_section));
	
	hmac_cfg = &iot_boot_volatile_sect.wifi_param;
	//iot_printf("flash_param_config_init1 valid %x wifimode %x\n",iot_boot_volatile_sect.valid_boot_param ,iot_boot_volatile_sect.wifi_param.wifimode);
	iot_boot_volatile_sect.wifi_param.wifi_auto_connect=1;

	if(iot_boot_volatile_sect.valid_boot_param == BOOTLOAD_VALID_FLAG){		
		if(iot_boot_volatile_sect.boot_param.flash_size!= FLASH_SIZE){
			iot_printf("\n\n<WARNING> bootloader may err!! flash_size %d %d\n\n",iot_boot_volatile_sect.boot_param.flash_size,FLASH_SIZE);
		}
	}

	if(iot_boot_volatile_sect.valid_boot_param != BOOT_VALID_FLAG){		
		iot_boot_volatile_sect.boot_param.iot_version= 0;
		iot_boot_volatile_sect.boot_param.bootdelay = 3;
		//iot_boot_volatile_sect.boot_param.static_ip = 0;
		iot_boot_volatile_sect.boot_param.smartconfig= 0;
		iot_boot_volatile_sect.boot_param.b_usedMacaddr = 0;
		//iot_boot_volatile_sect.boot_param.msgwaittimer= 60;
	}

	
	if(iot_boot_volatile_sect.valid_wifi_param != BOOT_VALID_FLAG){
		//DBG_Printf("iot wifi param not set,use default\n");
		os_memcpy(hmac_cfg->ssid,DEFAULT_AP_SSID,strlen(DEFAULT_AP_SSID));
		hmac_cfg->ssid_len = strlen(DEFAULT_AP_SSID);
		os_memcpy(hmac_cfg->password,DEFAULT_AP_PWD,strlen(DEFAULT_AP_PWD));
		hmac_cfg->password_len = strlen(DEFAULT_AP_PWD);
		hmac_cfg->key_mgmt= KEY_WPA2;
		hmac_cfg->privacy = 1;
#ifdef CONFIG_PS_ENABLE
		iot_boot_volatile_sect.wifi_param.system_sleep_mode = HAL_MODEM_SLEEP;
#else //CONFIG_PS_ENABLE
		iot_boot_volatile_sect.wifi_param.system_sleep_mode = HAL_NO_SLEEP;
#endif //CONFIG_PS_ENABLE
		iot_boot_volatile_sect.wifi_param.wifimode= 0;
		if(iot_boot_volatile_sect.wifi_param.wifimode){ //default mode is AP or station
			//	iot_boot_volatile_sect.boot_param.static_ip = 1;
			hmac_cfg->ap_mode = 1;
		}
		else {
			//	iot_boot_volatile_sect.boot_param.static_ip = 0;
			hmac_cfg->ap_mode = 0;
		}	
		hmac_cfg->country_id = DEFAULT_COUNTRY;
		hmac_cfg->channel_no = DEFAULT_CHANNEL_VALUE;			
	}else{
		if(hmac_cfg->channel_no == 0){
			hmac_cfg->channel_no = DEFAULT_CHANNEL_VALUE;	
		}		
		if(hmac_cfg->country_id >= country_MAX){
			hmac_cfg->country_id = DEFAULT_COUNTRY;
		}
	}
	
	hmac_cfg->hw_fix_rate_id = 0xff;
	
	if(iot_boot_volatile_sect.valid_uart_param != BOOT_VALID_FLAG){		
		iot_boot_volatile_sect.uart_param.uart_Baudrate= 115200;
		iot_boot_volatile_sect.uart_param.uart_DataBits = 8;
		iot_boot_volatile_sect.uart_param.uart_Parity = 0;
		iot_boot_volatile_sect.uart_param.uart_StopBits = 1;
	}


	/***************AP mode iot_boot_sect*********************/
	if(iot_boot_volatile_sect.valid_AP_tcpip_param != BOOT_VALID_FLAG){
		iot_boot_volatile_sect.AP_tcpip_param.iot_listen_port = 1234;
		iot_boot_volatile_sect.AP_tcpip_param.iot_connect_rport = 1234;
		iot_boot_volatile_sect.AP_tcpip_param.iot_connect_lport = 1234;
		iot_boot_volatile_sect.AP_tcpip_param.static_ip = 1;
		set_ip_ipaddr(iot_boot_volatile_sect.AP_tcpip_param.static_ipaddr, 192, 168, 43,  1);
		set_ip_ipaddr(iot_boot_volatile_sect.AP_tcpip_param.static_gwaddr, 192, 168, 43,  1);
		set_ip_ipaddr(iot_boot_volatile_sect.AP_tcpip_param.static_ipmask, 255, 255, 255, 0);
		set_ip_ipaddr(iot_boot_volatile_sect.AP_tcpip_param.iot_connect_ip, 192, 168, 43,  2 );
	}
	/***************STA mode i iot_boot_sect*********************/
	if(iot_boot_volatile_sect.valid_STA_tcpip_param != BOOT_VALID_FLAG){
		iot_boot_volatile_sect.STA_tcpip_param.iot_listen_port = 1234;
		iot_boot_volatile_sect.STA_tcpip_param.iot_connect_rport = 1234;
		iot_boot_volatile_sect.STA_tcpip_param.iot_connect_lport = 1234;
		iot_boot_volatile_sect.STA_tcpip_param.static_ip = 0;
		set_ip_ipaddr(iot_boot_volatile_sect.STA_tcpip_param.static_ipaddr, 192, 168, 1,  20 );
		set_ip_ipaddr(iot_boot_volatile_sect.STA_tcpip_param.static_gwaddr, 192, 168, 1,  1);
		set_ip_ipaddr(iot_boot_volatile_sect.STA_tcpip_param.static_ipmask, 255, 255, 255, 0);
		set_ip_ipaddr(iot_boot_volatile_sect.STA_tcpip_param.iot_connect_ip, 192, 168, 1,  1 );
	}

}
FLASH_FUNC void flash_param_config_reset()
{
	os_memset(&iot_boot_volatile_sect, 0,sizeof(struct flash_boot_volatile_section));
	
	flash_param_write(flash_info.flash_boot_param_section2_addr,&iot_boot_volatile_sect,sizeof(iot_boot_volatile_sect));
}
NO_FLASH_FUNC void flash_param_config_save()
{
	//iot_printf("flash_param_config_saveiot_boot_sect.valid_uart_param: %d \n",iot_boot_volatile_sect.valid_uart_param);
	struct flash_boot_volatile_section *flash_volatile_data = (struct flash_boot_volatile_section *)flash_info.flash_boot_param_section2_addr;
	if((iot_boot_volatile_sect.valid_wifi_param == BOOT_VALID_FLAG)||
		(iot_boot_volatile_sect.valid_AP_tcpip_param == BOOT_VALID_FLAG)|| 
		(iot_boot_volatile_sect.valid_STA_tcpip_param == BOOT_VALID_FLAG)||
		(iot_boot_volatile_sect.valid_TCP_socket_param == BOOT_VALID_FLAG)||
		(iot_boot_volatile_sect.valid_iot_bootInitRegister == BOOT_VALID_FLAG ||
		(iot_boot_volatile_sect.valid_boot_param == BOOT_VALID_FLAG)||
		(iot_boot_volatile_sect.valid_uart_param == BOOT_VALID_FLAG))){
		if(memcmp(flash_volatile_data,&iot_boot_volatile_sect,sizeof(iot_boot_volatile_sect))){
			//iot_printf("flash_param_config_save2 len=%d addr %x\n",sizeof(iot_boot_volatile_sect),flash_info.flash_boot_param_section2_addr);
			os_memcpy(&iot_boot_volatile_sect.flashcode_enc_param, &flash_volatile_data->flashcode_enc_param, sizeof(iot_boot_volatile_sect.flashcode_enc_param));
			iot_boot_volatile_sect.valid_flashcode_enc_param = flash_volatile_data->valid_flashcode_enc_param;
			
			flash_param_write(flash_info.flash_boot_param_section2_addr,&iot_boot_volatile_sect,sizeof(iot_boot_volatile_sect));
		}
	}
	
}
NO_FLASH_FUNC void flash_const_param_config_save()
{
	struct flash_boot_const_section *flash_const_data = (struct flash_boot_const_section *)flash_info.flash_boot_param_section_addr;
	if((iot_boot_const_sect.valid_device_param == BOOT_VALID_FLAG)){
		if(memcmp(flash_const_data,&iot_boot_const_sect,sizeof(iot_boot_const_sect))){
			//iot_printf("flash_param_config_save len=%d addr %x\n",sizeof(iot_boot_const_sect),flash_info.flash_boot_param_section_addr);
#if CONFIG_ATBM_PRODUCT_TEST
			if (atbmwifi_is_etf_testmode() == 0)
#endif//#if CONFIG_ATBM_PRODUCT_TEST
			{
				os_memcpy(&iot_boot_const_sect.device_param, &flash_const_data->device_param, sizeof(iot_boot_const_sect.device_param));
				iot_boot_const_sect.valid_device_param = flash_const_data->valid_device_param;
			}
#if CONFIG_ATBM_PRODUCT_TEST
			if (atbmwifi_is_etf_testmode())
				flash_param_write(flash_info.flash_boot_param_section_addr,&iot_boot_const_sect,sizeof(iot_boot_const_sect));
#endif//#if CONFIG_ATBM_PRODUCT_TEST
	
		}
	}
}

/*Copy factory backed boot params to boot params section */ 
NO_FLASH_FUNC int atbmwifi_flash_param_config_restore(void)
{
#ifdef IOT_RESTORE_FACTORY_SUPPORT
	iot_printf("flash_param_config_save\n");
	struct flash_boot_volatile_section *flash_volatile_data = (struct flash_boot_volatile_section *)flash_info.flash_boot_param_section2_addr; 
	struct flash_boot_volatile_section *flash_volatile_data_back = (struct flash_boot_volatile_section *)flash_info.flash_boot_param_section2_addr + flash_info.flash_boot_param_section2_len;

	os_memcpy(&iot_boot_volatile_sect,flash_volatile_data_back,sizeof(struct flash_boot_section));
	if(os_memcmp(flash_volatile_data,&iot_boot_volatile_sect,sizeof(iot_boot_volatile_sect))){
		iot_printf("flash_param_config_save len=%d\n",sizeof(iot_boot_volatile_sect));
			flash_param_write(flash_info.flash_boot_param_section2_addr,&iot_boot_volatile_sect,sizeof(iot_boot_volatile_sect));
	}	
#endif
	return 0;
}
NO_FLASH_FUNC void atbmwifi_flash_param_bootconfig_change()
{	
	iot_boot_volatile_sect.valid_boot_param = BOOT_VALID_FLAG;
	flash_param_config_save();
}

NO_FLASH_FUNC void atbmwifi_flash_param_wificonfig_change()
{	
	iot_boot_volatile_sect.valid_wifi_param = BOOT_VALID_FLAG;
	flash_param_config_save();
}

NO_FLASH_FUNC void atbmwifi_flash_param_TCP_socket_change()
{
	iot_boot_volatile_sect.valid_TCP_socket_param = BOOT_VALID_FLAG;
	flash_param_config_save();
}

NO_FLASH_FUNC void atbmwifi_flash_param_uartconfig_change()
{
	iot_boot_volatile_sect.valid_uart_param = BOOT_VALID_FLAG;
	flash_param_config_save();
}

NO_FLASH_FUNC void atbmwifi_flash_param_staipconfig_change()
{
	iot_boot_volatile_sect.valid_STA_tcpip_param = BOOT_VALID_FLAG;
	//iot_printf("flash_param_config_saveiot_boot_sect.valid_STA_tcpip_param: %d \n",iot_boot_volatile_sect.valid_STA_tcpip_param);
	flash_param_config_save();
}
NO_FLASH_FUNC void atbmwifi_flash_param_apipconfig_change()
{
	iot_boot_volatile_sect.valid_AP_tcpip_param = BOOT_VALID_FLAG;
	flash_param_config_save();
}

NO_FLASH_FUNC void atbmwifi_flash_param_deviceinfo_change()
{
	iot_boot_const_sect.valid_device_param = BOOT_VALID_FLAG;
	flash_param_write(flash_info.flash_boot_param_section_addr,&iot_boot_const_sect,sizeof(iot_boot_const_sect));
	//flash_param_config_save();
}

NOCACHE_NOFLASH_FUNC int flash_param_write(u32 addr,u8 *data,int len)
{
	int i =0;
	//iot_printf("flash_param_write1 %x %x\n",addr,len);

	for(i=addr; i<addr + len;i+=DEVICE_SECTOR_SIZE)
	{
		flash_sector_erase(i&flash_info.flash_offset_mask);
		atcwdt200_restart_wdt();
	} 	
	//iot_printf("flash_param_write2 %x\n",addr);
	//device program time	
	for(i=0; i< len;i+=DEVICE_PAGE_SIZE)
	{
		if(flash_page_program((i+addr)&flash_info.flash_offset_mask,(UINT32 *)(data+i),DEVICE_PAGE_SIZE)!= FLASH_SUCCESS)
		{
			iot_printf("flash__program error! page addr=0x%x\t",i);
			return HAL_FAILURE;
			//break;
		}
		atcwdt200_restart_wdt();
	}
	//iot_printf("flash_param_write3 %x\n",addr);
	return HAL_SUCCESS;
}

NO_FLASH_FUNC int __flash_param_write(u32 addr,u8 *data,int len)
{
	int i =0;

	//iot_printf("flash_param_write1 %x %x\n",addr,len);

	if((addr & (DEVICE_SECTOR_SIZE-1))==0)
	{
		for(i=addr; i<addr + len;i+=DEVICE_SECTOR_SIZE)
		{
			flash_sector_erase(i&flash_info.flash_offset_mask);
		} 
	}
	//iot_printf("flash_param_write2 %x\n",addr);
	//device program time	
	for(i=0; i< len;i+=DEVICE_PAGE_SIZE)
	{
		if(flash_page_program((i+addr)&flash_info.flash_offset_mask,(UINT32 *)(data+i),DEVICE_PAGE_SIZE)!= FLASH_SUCCESS)
		{
			iot_printf("flash__program error! page addr=0x%x\t",i);
			return HAL_FAILURE;
		}
	}
	//iot_printf("flash_param_write3 %x\n",addr);
	return HAL_SUCCESS;
}
#if (CHIP_TYPE==CHIP_ARES_B)
FLASH_FUNC int atbmwifi_flash_chip_deepSleep() 
{
	return flash_chip_deepSleep();	
}
#endif
NO_FLASH_FUNC int flash_param_read(u32 addr,u8 *data,int len)
{
	int i =0;
	int  rlen = len;
//device program time	
	for(i=0; i< len;i+=DEVICE_PAGE_SIZE)
	{
		if((len-i)>=DEVICE_PAGE_SIZE)
			rlen = DEVICE_PAGE_SIZE;
		else
			rlen = (len-i);
		
		//iot_printf("flash_param_read %x\n",(i+addr)&flash_info.flash_offset_mask);
		if(flash_fast_read((i+addr)&flash_info.flash_offset_mask,(UINT32 *)(data+i),rlen)!= FLASH_SUCCESS)
		{
			iot_printf("flash__program error! page addr=0x%x\t",i);
			return HAL_FAILURE;
			//break;
		}
	}
	//iot_printf("flash_param_read --\n");
	return HAL_SUCCESS;


}
static struct iot_updata_param_flag iot_updata_flag;

//static struct iot_updata_param  iot_updata;
//UPDATE_CODE1 _erase
NO_FLASH_FUNC void flash_updatafw1_block_erase()
{
	u32 i =0;
	u32 addr = flash_info.flash_code1_update_section_addr;
	

	iot_printf("flashfw1_erase\n");

	//just erase updata flag SECTOR,
	//防止更新一半时更新出错导致的误更新错误
	flash_sector_erase((flash_info.flash_code_update_flag_section_addr)&(~(DEVICE_SECTOR_SIZE-1)));	
	// erase codeblock1
	for(i=addr; i<(addr+flash_info.flash_code1_update_section_len);i+=DEVICE_BLOCK_SIZE)
	{
		flash_block_erase(i&flash_info.flash_offset_mask);			
	} 
}

//UPDATE_CODE2 _erase
NO_FLASH_FUNC void flash_updatafw2_block_erase()
{
	u32 i =0;
	u32 addr = flash_info.flash_code2_update_section_addr;
	
	iot_printf("flashfw2_erase %x\n",flash_info.flash_code_update_flag_section_addr);
#if BLOCK_ERASE_EXTERN
	for(i=addr; i<(addr+flash_info.flash_code2_update_section_len);i+=DEVICE_BLOCK_SIZE)
	{
		flash_block_erase(i&flash_info.flash_offset_mask);		
	} 
#else //BLOCK_ERASE_EXTERN
	/*所有的flash 都erase，在函数中等待的时间太长有时候会导致，wifi 断网，所有把等待函数分散到每次写函数中*/
	//just erase updata flag SECTOR in flash_updatafw1_block_erase	
	//flash_sector_erase((flash_info.flash_code_update_flag_section_addr)&(~(DEVICE_SECTOR_SIZE-1)));	
#endif //BLOCK_ERASE_EXTERN
}


//UPDATE_CODE1  sector _erase
NO_FLASH_FUNC void flash_updatafw_sector_erase()
{
	u32 i =0;
	u32 addr = flash_info.flash_code1_update_section_addr;
	//u32 flags =0;
	
	for(i=addr; i<(addr+flash_info.flash_code1_update_section_len);i+=DEVICE_SECTOR_SIZE)
	{
		flash_sector_erase(i&flash_info.flash_offset_mask);
	} 
}

NO_FLASH_FUNC u16_t fw_chksum(void *dataptr, u16_t len)
{
  u16_t acc;
  u16_t src;
  u8_t *octetptr;

  acc = 0;
  /* dataptr may be at odd or even addresses */
  octetptr = (u8_t*)dataptr;
  while (len > 1) {
    /* declare first octet as most significant
       thus assume network order, ignoring host order */
    src = (*octetptr);
    octetptr++;
    /* declare second octet as least significant */
    src |= (*octetptr)<<8;
    octetptr++;
    acc += src;
    len -= 2;
  }
  if (len > 0) {
    /* accumulate remaining octet */
    src = (*octetptr);
    acc += src;
  }
  /* This maybe a little confusing: reorder sum using htons()
     instead of ntohs() since it has a little less call overhead.
     The caller must invert bits for Internet sum ! */
  return ((u16_t)acc);
}

//UPDATE_CODE1 write
NO_FLASH_FUNC void flash_updatafw1_write(u32 offset,u8 *data,int len)
{
	int i =0;
	//u32 flags =0;
	u32 baseaddr  = flash_info.flash_code1_update_section_addr;

	if(offset == 0){		
		os_memset(&iot_updata_flag,0,sizeof(struct iot_updata_param	));
	}
	
	if(flash_info.flash_code1_update_section_len <= offset){		
		iot_printf("%s offset error! page addr=0x%x\t",__func__,offset);
		return ;
	}
	for(i=0; i< len;i+=DEVICE_PAGE_SIZE)
	{
		if(flash_page_program((i+baseaddr+offset)&flash_info.flash_offset_mask,(UINT32 *)(data+i),DEVICE_PAGE_SIZE)!= FLASH_SUCCESS)
		{
			iot_printf("%s error! page addr=0x%x\t",__func__,i);
			return;
		}
	}
	iot_updata_flag.iot_updata.iot_CODE1_len = offset+len;
	iot_updata_flag.iot_updata.iot_checksum += fw_chksum(data, len);
	
}
//UPDATE_CODE2 write
NO_FLASH_FUNC void flash_updatafw2_write(u32 offset,u8 *data,int len)
{
	int i =0;
	//u32 flags =0;
	u32 baseaddr  = flash_info.flash_code2_update_section_addr;
	struct iot_updata_param * piot_data;

	if(flash_info.flash_code2_update_section_len <= offset){		
		iot_printf("%s offset error! page addr=0x%x\t",__func__,offset);
		return ;
	}
	if((len < sizeof(struct iot_updata_param))||(len & 3)){
		iot_printf("%s len error! 0x%x\t",__func__,len);
		return ;
	}
	
#if BLOCK_ERASE_EXTERN
	for(i=0; i< len;i+=DEVICE_PAGE_SIZE)
	{
		if(flash_page_program((i+baseaddr+offset)&flash_info.flash_offset_mask,(UINT32 *)(data+i),DEVICE_PAGE_SIZE)!= FLASH_SUCCESS)
		{
			iot_printf("%s error! page addr=0x%x\t",__func__,i);
			return;
		}
	}
#else
	if(__flash_param_write(baseaddr+offset,data,len)!= FLASH_SUCCESS){
		iot_printf("%s error! page addr=0x%x\t",__func__,i);
		return;
	}
#endif 
	piot_data= (struct iot_updata_param * )&data[len-sizeof(struct iot_updata_param)];
	memcpy(&iot_updata_flag.updata_rx,piot_data,sizeof(iot_updata_flag.updata_rx));
	//os_memcmp(&flags,(struct iot_updata_param *)&iot_data->iot_flag,4);
	if((len%DEVICE_PAGE_SIZE) && (iot_updata_flag.updata_rx.iot_flag== BOOT_VALID_FLAG)){		
		u16 checksum = iot_updata_flag.updata_rx.iot_checksum;
		os_memset(&piot_data->iot_checksum,0,sizeof(piot_data->iot_checksum));		
		iot_updata_flag.iot_updata.iot_CODE2_len = offset+len-sizeof(struct iot_updata_param);
		iot_updata_flag.iot_updata.iot_checksum += fw_chksum(data, len);
		iot_updata_flag.iot_updata.iot_version  = iot_updata_flag.updata_rx.iot_version;
		iot_updata_flag.iot_updata.iot_updataCnt = iot_updata_flag.updata_rx.iot_updataCnt;
		if(checksum == (iot_updata_flag.iot_updata.iot_checksum&0xffff)){
			iot_updata_flag.iot_updata.iot_flag 		= BOOT_VALID_FLAG;
			iot_printf("iot_updata.csum OK\n");
		}
		else {
			iot_updata_flag.iot_updata.iot_flag = 0;
			iot_printf("iot_updata.csum FAIL %x %x\n",(iot_updata_flag.iot_updata.iot_checksum&0xffff),checksum);
			goto __err;
			
		}
	}
	else {
		if(len&(DEVICE_PAGE_SIZE-1)){
			//dump_mem(iot_data,sizeof(struct iot_updata_param));
			goto __err;
		}
		iot_updata_flag.iot_updata.iot_CODE2_len = offset+len;
		iot_updata_flag.iot_updata.iot_checksum += fw_chksum(data, len);
	}
	return;

__err:	
	iot_printf("%s ERR2 len 0x%x %x\t",__func__,len,iot_updata_flag.updata_rx.iot_flag);
	//iot_printf("CODE1_len %d\n",iot_updata.iot_CODE1_len);
	//iot_printf("CODE2_len %d\n",iot_updata.iot_CODE2_len);
	iot_printf("csum 0x%x\n",iot_updata_flag.iot_updata.iot_checksum);
	iot_printf("ver %d\n",iot_updata_flag.iot_updata.iot_version);
	//dump_mem(data,len);			
	//iot_printf(".CODE1_len %d\n",iot_data->iot_CODE1_len);
	//iot_printf(".CODE2_len %d\n",iot_data->iot_CODE2_len);
	//iot_printf(".csum 0x%x\n",iot_data.iot_checksum);
	//iot_printf(".ver %d\n",iot_data.iot_version);
	return;
}
//update_param2 write
/*
如果更新的code 不对返回HAL_ERR_UNAVAILABLE
如果flash 操作错误，返回HAL_FAILURE
成功返回HAL_SUCCESS
*/
NO_FLASH_FUNC int flash_updatafwFlag_write()
{
	return flash_updatafwFlag_reboot(1);
}
/*
如果更新的code 不对返回HAL_ERR_UNAVAILABLE
如果flash 操作错误，返回HAL_FAILURE
参数reboot=1，更新完成之后reboot 系统
成功返回HAL_SUCCESS
*/
NO_FLASH_FUNC int flash_updatafwFlag_reboot(int reboot)
{
	struct iot_updata_param * iot_data1;

	if(iot_updata_flag.iot_updata.iot_flag != BOOT_VALID_FLAG){
		iot_printf("%s checksum fail\n",__func__);
		return HAL_ERR_UNAVAILABLE;
	}
	iot_data1 = (struct iot_updata_param * )(flash_info.flash_code_flag_section_addr);
	if(iot_data1->iot_flag == BOOT_VALID_FLAG){
		iot_updata_flag.iot_updata.iot_updataCnt	= iot_data1->iot_updataCnt +1;
	}
	else {
		iot_updata_flag.iot_updata.iot_updataCnt	= 1;
	}
	
	if(flash_page_program(flash_info.flash_code_update_flag_section_addr,(UINT32 *)&iot_updata_flag,DEVICE_PAGE_SIZE)!= FLASH_SUCCESS)
	{
		iot_printf("%s error! page addr=0x%x\n",__func__,flash_info.flash_code_update_flag_section_addr);
		return HAL_FAILURE;
	}
	else {
		iot_printf("%s OK\n",__func__);
	}
	iot_updata_flag.iot_updata.iot_flag = 0;
	if(reboot){
		system_reboot();
	}
	return HAL_SUCCESS;
}

NO_FLASH_FUNC void flash_cmd(u8 * data,int len){
	char *str = data;
	//int page =0;
	//char *flashdata =data;
	iot_printf("flash_cmd %s\n",data);
	if(memcmp("AT+flashRd",str,strlen("AT+flashRd"))==0){
		Console_SetPolling(1);
		dump_mem((u8 *)flash_info.flash_code1_update_section_addr,1024);
		Console_SetPolling(0);
	}
	else if(memcmp("AT+flashEraseB",str,strlen("AT+flashEraseB"))==0){
		flash_updatafw1_block_erase();
	}	
	else if(memcmp("AT+flashErase",str,strlen("AT+flashErase"))==0){
		flash_updatafw_sector_erase();
	}
}

FLASH_FUNC void flash_init_check_user_key(void)
{
	int origiBlock_valid = 1;
	int bakBlock_valid = 1;
	int i=0;
	os_memcpy(&flash_user_key_bak, flash_info.flash_usr_key_bak_addr, sizeof(flash_user_key_bak));
	//can't all zero
	if(iot_boot_const_sect.device_param.chksum ==0){
		for(i=0;i<512;i++){
			if(iot_boot_const_sect.device_param.key_data[i])
				break;
		}
		if(i==512){
			origiBlock_valid = 0;
		}
	}
	if(origiBlock_valid){
		if (fw_chksum(&iot_boot_const_sect.device_param.key_data[0], 512) != iot_boot_const_sect.device_param.chksum)
		{
			origiBlock_valid = 0;
		}
	}
	//dump_mem(&iot_boot_const_sect.device_param.key_data[0],512);
	//iot_printf("key_data i=%d origiBlock_valid%d %s\n",i,origiBlock_valid, &iot_boot_const_sect.device_param.key_data[0]);


	//can't all zero
	if(flash_user_key_bak.chksum ==0){
		for(i=0;i<512;i++){
			if(flash_user_key_bak.key_data[i])
				break;
		}
		if(i==512){
			bakBlock_valid = 0;
		}
	}
	if(bakBlock_valid){
		if (fw_chksum(&flash_user_key_bak.key_data[0], 512) != flash_user_key_bak.chksum)
		{
			bakBlock_valid = 0;
		}
	}
	
	if ((origiBlock_valid == 0) && (bakBlock_valid == 0))
	{
		iot_printf("original and backups, user key both invalid \n");
	}else if ((origiBlock_valid == 1) && (bakBlock_valid == 0))
	{
		iot_printf("back user key valid, backups user key invalid\n");
		flash_param_write(flash_info.flash_usr_key_bak_addr,&iot_boot_const_sect.device_param,sizeof(iot_boot_const_sect.device_param));	
		os_memcpy(&flash_user_key_bak, flash_info.flash_usr_key_bak_addr, sizeof(flash_user_key_bak));
	}else if ((origiBlock_valid == 0) && (bakBlock_valid == 1))
	{
		iot_printf("original user key invalid, backups user key valid\n");
		os_memcpy(&iot_boot_const_sect.device_param, &flash_user_key_bak, sizeof(iot_boot_const_sect.device_param));
		atbmwifi_flash_param_deviceinfo_change();	
	}else{
		iot_printf("original and backups, user key both valid\n");
	}
#define DEVICEID ((flash_user_key_bak.key_data[12] = 0), &(flash_user_key_bak.key_data[2]))//"100003a26d"//"100003a0d1"
#define APIKEY 	 ((flash_user_key_bak.key_data[49] = 0), &(flash_user_key_bak.key_data[13]))//"2ca02be8-b16e-4dfe-a2a1-1be5f096ddde"
#define MODEL 	 ((flash_user_key_bak.key_data[96] = 0), &(flash_user_key_bak.key_data[86]))//"C05-X01-GL"
#define VERDION "0.2.8"
	iot_printf("%s\n", DEVICEID);
	iot_printf("%s\n", APIKEY);
	iot_printf("%s\n", MODEL);
	iot_printf("%s\n", VERDION);

}






struct iot_flash_info flash_info = {
	.flash_boot_code_section_addr		=FLASH_BOOT_CODE_SECTION_ADDR,
	.flash_boot_param_section_addr		=FLASH_BOOT_PARAM_SECTION_ADDR,
	.flash_code_section_addr			=FLASH_CODE_SECTION_ADDR,
	.flash_code1_section_addr			=FLASH_CODE1_SECTION_ADDR,
	.flash_code2_section_addr			=FLASH_CODE2_SECTION_ADDR,
	.flash_code1_section_len			=FLASH_CODE1_SECTION_LEN,
	.flash_code2_section_len			=FLASH_CODE2_SECTION_LEN,
	.flash_code_flag_section_addr		=FLASH_CODE_FLAG_SECTION_ADDR,
	.flash_code1_update_section_addr	=FLASH_CODE1_UPDATA_SECTION_ADDR,
	.flash_code1_update_section_len		=FLASH_CODE1_UPDATA_SECTION_LEN,
	.flash_code2_update_section_addr	=FLASH_CODE2_UPDATA_SECTION_ADDR,
	.flash_code2_update_section_len		=FLASH_CODE2_UPDATA_SECTION_LEN,
	.flash_code_update_flag_section_addr=FLASH_CODE_UPDATA_FLAG_SECTION_ADDR,
	.flash_usr_data_section_addr		=FLASH_USR_DATA_SECTION_ADDR,
	.flash_usr_data_section_len			=FLASH_USR_DATA_SECTION_LEN,
	.flash_usr_data_section2_addr		=FLASH_USR_DATA_SECTION2_ADDR,
	.flash_usr_data_section2_len 		=FLASH_USR_DATA_SECTION2_LEN,
	.boot_valid_flag					=BOOT_VALID_FLAG,
	.flash_offset_mask					=FLASH_OFFSET_MASK,
	.flash_usr_key_bak_addr				=FLASH_USR_KEY_SECTION_ADDR,
	.flash_usr_key_bak_len				=FLASH_USR_KEY_SECTION_LEN, 
	.flash_boot_param_section2_addr   	=FLASH_BOOT_PARAM_SECTION2_ADDR,
	.flash_boot_param_section2_len		=BLOCK_BOOT_PARAM_LEN,
};

