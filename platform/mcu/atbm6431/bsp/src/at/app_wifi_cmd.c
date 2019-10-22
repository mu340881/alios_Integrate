/**************************************************************************************************************
 * altobeam iot at cmd file 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 * Permission is hereby granted for use on Altobeam IOT 6421 only,
 * in which case ,it is free to  modify it
 *
*****************************************************************************************************************/

#include "hal.h"
#include "wifi_api.h"
#include "app_flash_param.h"
//#include "atbm_joylink_app.h"
#include "atbm_wifi_driver_api_export.h"


/* 
*smartconfig control switch
*smt_type should chose follow value:
*ATBM_SMARTCONF_CONFIGURE: use our own smartconfig to configurate ssid and password
*JOYLINK_CONFIGURE: use jingdong weilian smartconfig to configurate ssid and password
*/
enum smartconfig_type smt_type = ATBM_SMARTCONF_CONFIGURE;


/*smartconfig or joylink*/
extern struct wifi_configure* hmac_cfg; 


/*connect AP*/
FLASH_FUNC void wifi_ConnectAP(u8 * ssid,int ssidlen,u8 * password,int passwdlen,SECURITY_TYPE key_mgmt)
{
	if(ssidlen > 32){
		iot_printf("ssidlen max=32,please set again\n");
		return;
	}	
	if(passwdlen > 64){
		iot_printf("passwdlen max=64,please set again\n");
		return;
	}
	memset(hmac_cfg->ssid,0,sizeof(hmac_cfg->ssid));
	os_memcpy(hmac_cfg->ssid,ssid,ssidlen);
	hmac_cfg->ssid_len = ssidlen;
	if(passwdlen){
		memset(hmac_cfg->password,0,sizeof(hmac_cfg->password));
		os_memcpy(hmac_cfg->password,password,passwdlen);
	}
	if(KEY_WEP_SHARE != key_mgmt){
		hmac_cfg->auth_alg = WLAN_AUTH_OPEN;
	}
	else {
		hmac_cfg->auth_alg = WLAN_AUTH_SHARED_KEY;
	}
	hmac_cfg->password_len = passwdlen;
	hmac_cfg->privacy = passwdlen?1:0;
	hmac_cfg->key_mgmt = key_mgmt;
	hmac_cfg->key_id = 0;
	hmac_cfg->psk_set = 0;
	wifi_printk(WIFI_ATCMD,"%s ssid[%s]:%d \n",__func__,hmac_cfg->ssid,hmac_cfg->ssid_len);
	atbm_wifi_connect_ap();
}



/*
change wifi mode
mode : 0 stamode
mode : 1 APmode
//标识需要进行模式切换，如果之前是sta 就会切换成ap，
如果之前是ap 就会切换为sta，
切换完成之后的连接ap 或者创建ap 配置是根据hmac_cfg 中得到，
如有需要可以在切换核心代码之前添加hmac_cfg的配置
g_vmac->iftype_change = 1;
//启动切换task 开始切换wifi模式
atbmwifi_queue_atcmd_task(ATCMD_TASK_EVENT);	
*/
extern u8 mode_str[2][16];
FLASH_FUNC  int atbm_wifi_change_mode(u8 mode)
{
	//step 1		
	if(mode > 1)
		return -2;
	
	if((atbm_wifi_is_iftype_ap()) && (mode==1)){
		return -1;
	}
	if((atbm_wifi_is_iftype_station()) && (mode==0)){
		return -1;
	}
	#if 0
	g_vmac->iftype_change = 1;
	atbmwifi_queue_atcmd_task((void*)g_vmac,(void*)mode,AT_CMD_CHANGE_MODE);
	#else
	atbmwifi_stop();
	atbmwifi_start(atbm_wifi_is_iftype_station());
	#endif
	/*
	if(saveflash){
		if(mode == 0){
			hmac_cfg->ap_mode = 0;
			iot_boot_volatile_sect.wifi_param.wifimode = 0;
		}
		else {
			hmac_cfg->ap_mode = 1;
			iot_boot_volatile_sect.wifi_param.wifimode = 1;
		}
	}*/
	wifi_printk(WIFI_ATCMD,"change wifi to <%s> \n",mode_str[mode]);

	return 0;
}



/*
change wifi powersave mode
*/
FLASH_FUNC void wifi_ChangePsMode(u8 enable)
{
	atbm_wifi_set_ps_mode(enable);
}

/****************************************************************************
* Function:   	atbm_wifi_smart_config_start
*
* Purpose:   	This function is used to start smart configure
*
* Parameters: none
*
* Returns:	return 0 if success, others if failed.
****************************************************************************/
FLASH_FUNC int atbm_wifi_smart_config_start(enum smartconfig_type type)
{	
	int ret = -1;
	
	if((!atbm_wifi_is_iftype_station())&&(!atbm_wifi_is_iftype_p2p_client())) {	
		iot_printf( "not support scan in AP mode!\n");
		return	-1;		
	}
	if(atbm_wifi_is_smartconfig_enable()){
		iot_printf( "scan_smartconfig now!please try later!");
		return	-1;
    }	
	if(!atbm_wifi_is_enabled()){
		iot_printf( "not support not enabled!\n");
		return	-1;
	}
	smt_type = type;

	switch(type){
		case EWLINK_CONFIGURE:
#ifdef CONFIG_EWLINK_SMARTCONFIG
			ew_smartconfig_start();
#endif  //CONFIG_EWLINK_SMARTCONFIG
			//break;
		case ATBM_SMARTCONF_CONFIGURE:
		{
			struct smartconfig_config st_cfg = {
				.type = ATBM_SMARTCONF_CONFIGURE,
				.magic_cnt = 1,
				.magic_time = 70, 
				.payload_time = 12000,
			};
		    ret = smartconfig_start(&st_cfg);
		}
			break;
		case JOYLINK_CONFIGURE: 
			{
#ifdef CONFIG_JOY_LINK	 
				atbm_joylink_start();
#endif//CONFIG_JOY_LINK
			} 
			break;
		default:
			break;
	}
	return ret;
}
/****************************************************************************
* Function:   	atbm_wifi_smart_config_stop
*
* Purpose:   	This function is used to stop smart configure
*
* Parameters: none
*
* Returns:	return 0 if success, others if failed.
****************************************************************************/
FLASH_FUNC int atbm_wifi_smart_config_stop(void)
{	
	int ret= -1;
	if((!atbm_wifi_is_iftype_station())&&(!atbm_wifi_is_iftype_p2p_client())) {	
		iot_printf("not support scan in AP mode!\n");
		return ret;		
	}	
	if(!atbm_wifi_is_enabled()){
		iot_printf("not support not enabled!\n");
		return ret;
	}
	switch(smt_type){
		case EWLINK_CONFIGURE:
		case ATBM_SMARTCONF_CONFIGURE:
			if(atbm_wifi_is_smartconfig_enable()){
				smartconfig_stop();
		   	}	
			ret = 0;
			break;
		case JOYLINK_CONFIGURE:
#ifdef CONFIG_JOY_LINK	 
			ret = atbm_joylink_stop();
#endif//CONFIG_JOY_LINK
			break;
		default:
			break;
	}
	return ret;
}


FLASH_FUNC void atbm_wifi_smart_config_rx(void * hw_priv,struct sk_buff *skb,int channel)
{
	switch(smt_type){		
		case ATBM_SMARTCONF_CONFIGURE:
			smartconfig_start_rx(hw_priv,skb,channel);  
			break;
		case EWLINK_CONFIGURE:    //fifo mode, internal used ATBM smartconfig RX when enter monitor phase.
		
#ifdef CONFIG_EWL_LINK	 
			ew_smartconfig_start_rx(hw_priv,skb,channel);
			//smartconfig_start_rx(hw_priv,skb,channel);  
#endif //CONFIG_JOY_LINK
			break;
		case JOYLINK_CONFIGURE: 
#ifdef CONFIG_JOY_LINK	 
			joylink_smnt_datahandler(OS_SKB_DATA(skb), OS_SKB_LEN(skb));
#endif//CONFIG_JOY_LINK
			break;
		case ALIYUN_CONFIGURE:
#if defined(CONFIG_ALI_YUN)	|| defined(CONFIG_ALI_OS)
			HAL_Awss_Rx_hander(skb,channel);
#endif//CONFIG_ALI_YUN
			break;
		default:
			break;
	}

}

#if defined(CONFIG_ALI_OS)
FLASH_FUNC void atbm_wifi_mgmt_rx(u8 *buff, u16 len, char rssi)
{
	HAL_Awss_MGMT_Rx_hander(buff, len, rssi);
}
#endif


