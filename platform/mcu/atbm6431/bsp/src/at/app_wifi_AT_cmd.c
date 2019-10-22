/**************************************************************************************************************
 * altobeam iot at cmd file 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 * Permission is hereby granted for use on Altobeam IOT 6421 only,
 * in which case ,it is free to  modify it
 *
*****************************************************************************************************************/

#include "app_AT_cmd.h"
#include "app_flash_param.h"
#include "wifi_api.h"
#include "atbm_etf.h"
//#include "atbm_joylink_app.h"
#include "atbm_wifi_driver_api_export.h"
extern FLASH_FUNC int atbm_wifi_softAP_step1(void);
#define NOT_USED_POINTER(X) (*(volatile typeof(X)*)&(X) = (X))
FLASH_FUNC u8 do_config_keymgmt(char *pLine)
{
	u8 key_mgmt =0;

	if (!memcmp("NONE",pLine,strlen("NONE")))
	{
		key_mgmt = KEY_NONE;
	}
	else if(!memcmp("WEP",pLine,strlen("WEP")))
	{
		key_mgmt = KEY_WEP;
	}
	else if(!memcmp("WEP_SHARE",pLine,strlen("WEP_SHARE")))
	{
		key_mgmt = KEY_WEP_SHARE;
	}
	else if(!memcmp("WPA_PSK",pLine,strlen("WPA_PSK")))
	{
		key_mgmt = KEY_WPA;
	}
	else if(!memcmp("WPA2_PSK",pLine,strlen("WPA2_PSK")))
	{
		key_mgmt = KEY_WPA2;
	}
	else if(!memcmp("WPA_WPA2_PSK",pLine,strlen("WPA_WPA2_PSK")))
	{
		key_mgmt = KEY_WPA2;
	}
	wifi_printk(WIFI_ATCMD,"key_mgmt %s %d\n",pLine,key_mgmt);
	return key_mgmt;
}

FLASH_FUNC void AT_WifiConfig(char *pLine)
{
	uint32 tmpdata = 0;
	char * str;
	u32 password_ascii[33];	
	u8 key_mgmt =0;
	u8 password_len = 0;
	u8 *password = 0;
	u8 key_id = 0;
	u8 *ssid = 0;
	u8 ssid_len = 0;
	
	pLine = CmdLine_SkipSpace(pLine);

	wifi_printk(WIFI_ATCMD,"\nAT_WifiConfig %s\n",pLine);
	if (!memcmp("KEY_MGMT",pLine,strlen("KEY_MGMT")))
	{
		str = CmdLine_GetToken(&pLine);
		str = CmdLine_GetToken(&pLine);
		wifi_printk(WIFI_ATCMD,"KEY_MGMT %s\n",str);
		//
		key_mgmt = do_config_keymgmt(str);
	}
	
	pLine = CmdLine_SkipSpace(pLine);
	if (!memcmp("PWD",pLine,strlen("PWD")))
	{
		str = CmdLine_GetToken(&pLine);
		str = CmdLine_GetToken(&pLine);
		tmpdata = strlen(str);
		//
		switch(key_mgmt){

			case KEY_WPA:
			case KEY_WPA2:
				if((tmpdata<8)||(tmpdata>=64)){

					p_err("keylen err(%d)!\n",tmpdata);
					return;
				}
				break;

			case KEY_WEP:
			case KEY_WEP_SHARE:
				if((tmpdata!=5)&&(tmpdata!=13)&&(tmpdata!=10)&&(tmpdata!=26)){

					p_err("keylen err! need len=5 or 13 ,but now[%d]\n",tmpdata);
					return;
				}
				//todo add hex mode
				if((tmpdata==10)||(tmpdata==26)){
					//hex to ascii
					hexstr2bin( (u8 *)password_ascii,str,tmpdata);
					str = (char*)password_ascii;
					tmpdata = tmpdata/2;
				}
				break;
			case KEY_NONE:
			default:
				p_err("key mgmt err!\n");
				return;
				//break;

		}
		
		password = str;
		password_len = tmpdata;
	}
	pLine = CmdLine_SkipSpace(pLine);
	if (!memcmp("KEYID",pLine,strlen("KEYID")))
	{
		str = CmdLine_GetToken(&pLine);
		str = CmdLine_GetToken(&pLine);
		//
		CmdLine_GetInteger(&str, &tmpdata);
		wifi_printk(WIFI_ATCMD,"KEYID %d %s\n",tmpdata,str );
		//
		switch(key_mgmt){

			case KEY_WEP:
			case KEY_WEP_SHARE:
				key_id = tmpdata;
				if(/*(key_id<0)||*/(key_id>3)){
					p_err("keyid err!\n");
					return;
				}
				break;
			case KEY_WPA:
			case KEY_WPA2:
			case KEY_NONE:
			default:
				p_err("key mgmt err!\n");
				return;
				//break;

		}
	}
	pLine = CmdLine_SkipSpace(pLine);
	if (!memcmp("SSID",pLine,strlen("SSID")))
	{
		str = CmdLine_GetToken(&pLine);
#if 0
		int index = 0;
		int first_flag=0;
		//str = CmdLine_GetToken(&pLine);
		while(pLine[index] != 0)
		{
			if (pLine[index] == '"')
			{
				first_flag++;
				if (first_flag == 1)
				{
					str = &pLine[index + 1];
				}else
				{
					pLine[index] = 0;
					index++;
					break;
				}
			}
			index++;
		}
		if (first_flag != 0)
			pLine += index;
		else{
			str = CmdLine_GetToken(&pLine);
		}
#else
		str = CmdLine_GetToken_String(&pLine);
#endif 
		//wifi_printk(WIFI_ATCMD,"PWD1str %s pLine %s\n",str,pLine);
		//wifi_printk(WIFI_ATCMD,"PWD2str %s\n",str);
		//
		tmpdata = strlen(str);
		if((tmpdata <1)||(tmpdata > 32)){

			p_err("ssid err!\n");
			return;
		}
		ssid = str;
		ssid_len = tmpdata;
		wifi_printk(WIFI_ATCMD,"ssid_len %d %s\n",ssid_len,ssid );
	}
	atbm_wifi_set_config(ssid, ssid_len, password, password_len, key_mgmt, key_id);
}

FLASH_FUNC void AT_WifiModeChange(char *pLine)
{
	wifi_printk(WIFI_ATCMD,"AT_WifiModeChange\n");
	if (!memcmp("AP_MODE",pLine,strlen("AP_MODE")))	{
			atbm_wifi_change_mode(1);
	}
	else if (!memcmp("STA_MODE",pLine,strlen("STA_MODE")))	{
			atbm_wifi_change_mode(0);		
	}
	else {
		wifi_printk(WIFI_ATCMD,"\nparam error! must be AP_MODE or STA_MODE\n");
	}
}

FLASH_FUNC void AT_WifiStatus(char *Args)
{
	u8 mac_addr[6] ={0};
	u8 ssid[32] ={0};
	UN_USED_PARAMS(*Args);
	atbm_wifi_get_mac_addr(mac_addr);
	if(atbm_wifi_is_iftype_ap()){
		UART_SendString("[AP mode] \n");
		UART_SendString("mac	:"MACSTR"\n",MAC2STR(mac_addr));
		UART_SendString("channel	:%d \n",atbm_wifi_get_channel());
	}
	else if(atbm_wifi_is_iftype_station()){
		UART_SendString("[STA mode] \n");
		UART_SendString("mac	:["MACSTR"] \n",MAC2STR(mac_addr));
		if(atbm_wifi_is_connect_ok()){
			UART_SendString("status	:[connect]\n");
		}
		else {
			UART_SendString("status	:[not connect]\n");
		}
		atbm_wifi_get_connected_ssid(ssid);
		UART_SendString("ssid	:[%s] \n",ssid);
		
		atbm_wifi_get_connected_destAddr(mac_addr);
		UART_SendString("bssid	:["MACSTR"]\n",MAC2STR(mac_addr));
		UART_SendString("channel :%d \n",atbm_wifi_get_channel());
		//iot_printf("key_mgmt:[%s]"MACSTR"\n",MAC2STR(g_vmac->c));

	}
	else {
		UART_SendString("WIFI MODE[%d] ERROR!!\n",atbm_wifi_get_iftype());
	}
}

/*
connect AP AT cmd
*************************************************************************************************************************
<type1>: "KEY_MGMT" 
			<values>: "NONE" "WEP" "WEP_SHARE" "WPA_PSK" "WPA2_PSK" "WPA_WPA2_PSK"
<type2>: "PWD" 
			<values>: "xxxxxxxxxxxx"
<type3>: "KEYID" 
			<values>: "0" "1" "2" "3" //just support for WEP or WEP_SHARE
<type4>: "SSID" 
			<values>: "xxxAP"
*************************************************************************************************************************
example:
1. connect open AP:
	AT+WIFI_JOIN_AP KEY_MGMT NONE SSID wifi_test_ap15
2. connect wpa psk AP:
	AT+WIFI_JOIN_AP KEY_MGMT WPA_WPA2_PSK PWD 1234567890 SSID wifi_test_ap15

	AT+WIFI_JOIN_AP KEY_MGMT  WEP   PWD 12345 SSID wifi_test_ap29_2
*************************************************************************************************************************
*/
FLASH_FUNC void AT_WConnectTmp(char *pLine)
{
	if(!atbm_wifi_is_enabled()){
		wifi_printk(WIFI_ATCMD,"not support not enabled!\n");
		UART_Send_status(1);
		return;
	}
	//int tmpdata = 0;
	//char * str;
	if(atbm_wifi_is_iftype_station()){
		atbm_wifi_disconnect();
		AT_WifiConfig(pLine);
		atbm_wifi_connect_ap();
		//save to flash
	}
	
	UART_Send_status(0);
}


FLASH_FUNC void AT_WConnect(char *pLine)
{	
	if(!atbm_wifi_is_enabled()){
		wifi_printk(WIFI_ATCMD,"not support not enabled!\n");
		UART_Send_status(1);
		return;
	}
	//int tmpdata = 0;
	//char * str;
	if(atbm_wifi_is_iftype_station()){
		atbm_wifi_disconnect();
		AT_WifiConfig(pLine);
		atbm_wifi_connect_ap();
		//save to flash
	}	
	atbmwifi_flash_param_wificonfig_change();
	
	UART_Send_status(0);
}

FLASH_FUNC void AT_WDisConnect(char *pLine)
{		
	UN_USED_PARAMS(*pLine);
	atbm_wifi_disconnect();
	UART_Send_status(0);
}

/*scan AP. the scan ap result will iot_printf auto*/
FLASH_FUNC void AT_WifiScan(char *Args)
{	
	UN_USED_PARAMS(*Args);
	atbmwifi_scan_printf(1);
	atbm_wifi_scan();
	UART_Send_status(0);
}
void atbm_wifi_scanRet_cb(struct atbmwifi_scan_result *scan_result)
{
	int i;
	for (i =0; i < scan_result->len; i++)
	{
		UART_SendString("SSID: %s\n", scan_result->info[i].ssid);	
		UART_SendString("     channel %d\n", scan_result->info[i].channel);	
//		UART_SendString("MAC: %02X %02X %02X %02X %02X %02X\n",scan_result->info[i].bssid[0],
//			                                                   scan_result->info[i].bssid[1],
//			                                                   scan_result->info[i].bssid[2],
//			                                                   scan_result->info[i].bssid[3],
//		                                                       scan_result->info[i].bssid[4],
//		                                                       scan_result->info[i].bssid[5]);
		UART_SendString("MAC "MACSTR"\n",MAC2STR(scan_result->info[i].bssid));
		UART_SendString("     ht[%d] wpa[%d] rsn[%d] enc[%d]\n", scan_result->info[i].ht, scan_result->info[i].wpa, scan_result->info[i].rsn,scan_result->info[i].encrypt);
	}
	atbmwifi_scan_printf(0);
}
FLASH_FUNC void AT_WifiScanResult(char *Args)
{	
	UN_USED_PARAMS(*Args);
	atbm_wifi_process_scan_result(atbm_wifi_scanRet_cb);
}

#define CONFIG_SMART_CFG_STK_SIZE 1024
#define CONFIG_SMART_CFG_THREAD_PRIORITY CONFIG_TCPAPP_THREAD_PRIORITY
static int AT_smart_config_start = 0;
extern int smartConfigLinkedNotify_sample(void);

FLASH_FUNC void  smart_cfg_thread(void *param)
{
	int i = 0;
	unsigned int time_ms, timestamp_ms;
	NOT_USED_POINTER(param);
	time_ms = hal_time_get();
	for(i = 0; i<6; i++)  //atbm_wifi_wait_get_ipaddr_ok is 10s blocking function.
	{
		if((atbm_wifi_wait_get_ipaddr_ok())&&(AT_smart_config_start == 1))  //atbm_wifi_wait_get_ipaddr_ok is blocking function.
			{
				iot_printf("smartconfig_notify...\n");
				smartConfigLinkedNotify_sample();
				AT_smart_config_start = 0;
				return;
			}
		mdelay(1000);
		timestamp_ms = hal_time_get();
		//iot_printf("smartconfig_timeout i:%d time:%d...\n", i, timestamp_ms);
		if(timestamp_ms > (time_ms + 45000))
		{
			break;
		}
	}
	AT_smart_config_start = 0;
	iot_printf("smartconfig_timeout...\n");
}

FLASH_FUNC void smart_cfg_monitor_thread(char *pLine)
{
   	sys_thread_new("smart_monitor_thread", smart_cfg_thread, pLine,CONFIG_SMART_CFG_STK_SIZE,CONFIG_SMART_CFG_THREAD_PRIORITY);
}

FLASH_FUNC void AT_SmartConfigStart(char *Args)
{	
	UN_USED_PARAMS(*Args);
	if(AT_smart_config_start == 1)
	{
		iot_printf("smart_config started already, wait done or 45s timeout...\n");
	}else
	{
		smart_cfg_monitor_thread(Args);
		atbm_wifi_smart_config_start(ATBM_SMARTCONF_CONFIGURE);
		AT_smart_config_start = 1;
		UART_Send_status(0);
	}
}
FLASH_FUNC void AT_SmartConfigStop(char *Args)
{	
	UN_USED_PARAMS(*Args);
	atbm_wifi_smart_config_stop();
	AT_smart_config_start = 0;  /*for AT command thread */
	UART_Send_status(0);
}

/*添加了设置fix发送rate */
FLASH_FUNC void AT_WifiFixTxRate(char *Args)
{
/*
	uint32 hw_fix_rate_id =0;
    CmdLine_GetInteger(&Args, &hw_fix_rate_id);
	if((hw_fix_rate_id >=0) &&(hw_fix_rate_id < NUM_RATE_INDICES)){
		//test_iot_gpio();
		hmac_cfg->hw_fix_rate_id = hw_fix_rate_id;
		iot_printf("FixTxRate %d\n",hw_fix_rate_id);
		
	}
	else if((hw_fix_rate_id == 0xff)){		
		hmac_cfg->hw_fix_rate_id = hw_fix_rate_id;
		iot_printf("freeTxRate %d\n",hw_fix_rate_id);
	}
	else {		
		iot_printf("not support rate!\n");
	}*/
}


FLASH_FUNC void wifi_set_ip_params(unsigned char mode,  unsigned char savaflag)	
{  

	//set  IP address     
	netif_set_addr(atbm_wifi_get_netdev(),
		(ip4_addr_t *)&ip_params_user_set.static_ipaddr,
		(ip4_addr_t *)&ip_params_user_set.static_ipmask,
		(ip4_addr_t *)&ip_params_user_set.static_gwaddr);

	//AP DHCPD restart if opened
	if(mode == NL80211_IFTYPE_AP)
	{
		atbmwifi_disable_dhcp_server();
		atbmwifi_enable_dhcp_server();
	}	

	if(savaflag)
	{	
		if(mode == NL80211_IFTYPE_AP)
		{	
			memcpy((unsigned char *)&iot_boot_volatile_sect.AP_tcpip_param,   (unsigned char *)&ip_params_user_set, sizeof(ip_params_user_set));
			/* iot_printf("Ip params: IP:%x %x GW:%x %x\n",iot_boot_volatile_sect.AP_tcpip_param.iot_connect_ip[0], iot_boot_volatile_sect.AP_tcpip_param.iot_connect_ip[1], \
				iot_boot_volatile_sect.AP_tcpip_param.static_gwaddr[0], iot_boot_volatile_sect.AP_tcpip_param.static_gwaddr[1]);
				*/
			atbmwifi_flash_param_apipconfig_change();
		}
		else       	
		{
			memcpy((unsigned char *)&iot_boot_volatile_sect.STA_tcpip_param,   (unsigned char *)&ip_params_user_set, sizeof(ip_params_user_set));
			/*iot_printf("Ip params: IP:%x %x GW:%x %x\n",iot_boot_volatile_sect.STA_tcpip_param.iot_connect_ip[0], iot_boot_volatile_sect.STA_tcpip_param.iot_connect_ip[1], \
				iot_boot_volatile_sect.STA_tcpip_param.static_gwaddr[0], iot_boot_volatile_sect.STA_tcpip_param.static_gwaddr[1]);
				*/
			atbmwifi_flash_param_staipconfig_change();
		}	
	}

}


FLASH_FUNC int wifi_set_mac(unsigned char *mac_buf, unsigned char savaflag)	
{	 

	atbm_wifi_set_mac_addr(mac_buf);	 //Here using the same interface function, will update when AP+station supported.

	if(savaflag)
	{
		//update MAC address to flash etc  
		iot_boot_volatile_sect.boot_param.b_usedMacaddr = 1;
		memcpy(iot_boot_volatile_sect.boot_param.wifiMacaddr, mac_buf, 6);	
		atbmwifi_flash_param_bootconfig_change();	
	}
	return 0;
}




FLASH_FUNC void AT_WifiSoftAPCfg(char *pLine)
{	
	UN_USED_PARAMS(*pLine);
}




FLASH_FUNC void AT_ChannelSetTmp(char *pLine)
{	
	
	int ret = 0;
	int channel_no = 0; 
	
	ret  = parse_channel_no_cmd(pLine, &channel_no);
	if(!ret)
	{
		atbm_wifi_set_channel((u8)channel_no);
	  	wifi_printk(WIFI_ATCMD, "channel_no :%d\n", channel_no);
		//iot_boot_volatile_sect.wifi_param.channel_no = (u8)channel_no;
	 	//atbmwifi_flash_param_wificonfig_change();
	}	
	UART_Send_status(ret);
}	

FLASH_FUNC void AT_ChannelSet(char *pLine)
{	

	int ret = 0;
	int channel_no = 0;	
	
	ret  = parse_channel_no_cmd(pLine, &channel_no);
    if(!ret)
    {
    	atbm_wifi_set_channel((u8)channel_no);
  	  iot_boot_volatile_sect.wifi_param.channel_no = (u8)channel_no;
     wifi_printk(WIFI_ATCMD, "iot_boot_volatile_sect.wifi_param.channel_no :%d\n", iot_boot_volatile_sect.wifi_param.channel_no);
	
      atbmwifi_flash_param_wificonfig_change();
    }	
	UART_Send_status(ret);
}	
FLASH_FUNC void AT_CountryIdSetTmp(char *pLine)
{	
  	int ret = 0;	
	int country_id = 0;
	
	ret  = parse_country_code_cmd(pLine, &country_id);
	 
	UART_Send_status(ret);
}
FLASH_FUNC void AT_CountryIdSet(char *pLine)
{	
	
	int ret = 0;
	int country_id = 0;	
	
	ret  = parse_country_code_cmd(pLine, &country_id);
    if(!ret)
    {
    	iot_boot_volatile_sect.wifi_param.country_id = (u8)country_id;
		 wifi_printk(WIFI_ATCMD, "iot_boot_volatile_sect.wifi_param.country_id :%d\n", iot_boot_volatile_sect.wifi_param.country_id );
     	atbmwifi_flash_param_wificonfig_change();
    }	
	UART_Send_status(ret);
}

FLASH_FUNC void AT_CountryIdGet(char *pLine)
{	
	int country_id  = atbm_wifi_get_country();
	if(country_id < country_MAX){
		UART_SendString("+OK=%s\n",get_country_str(country_id));
	}
	else {
		UART_Send_status(HAL_ERR_UNAVAILABLE);
	}
}



FLASH_FUNC void AT_RestoreMacSet(char *pLine)
{		
	UN_USED_PARAMS(*pLine);
	int ret = 0;
	
	ret = atbm_wifi_use_efuse_mac();
	if (ret == 0)
	{
		iot_boot_volatile_sect.boot_param.b_usedMacaddr = 0;	
		atbmwifi_flash_param_bootconfig_change();	
	}
	UART_Send_status(ret);
}


FLASH_FUNC void AT_ApMacSet(char *pLine)
{		
	unsigned char mac_addr[6] = {0};
	int ret = 0;
	ret = parse_mac_cmd(pLine, mac_addr);

	if(!ret)
	{	 	
		wifi_set_mac(mac_addr,FLASH_SAVA_FLAG);			
	}
	UART_Send_status(ret);
}

 

FLASH_FUNC void AT_ApMacSetTmp(char *pLine)
{	
	unsigned char mac_addr[6] = {0};
	int ret = 0;
	ret = parse_mac_cmd(pLine, mac_addr);
	if(!ret)
	{
	  wifi_set_mac(mac_addr,  FLASH_UNSAVA_FLAG);	
	}
	UART_Send_status(ret);
}




FLASH_FUNC void AT_StaMacSet(char *pLine)
{	
	unsigned char mac_addr[6] = {0};
	int ret = 0;
	ret = parse_mac_cmd(pLine, mac_addr);

	if(!ret)
	{
		wifi_set_mac(mac_addr, FLASH_SAVA_FLAG);			
	}
	UART_Send_status(ret);
}


FLASH_FUNC void AT_StaMacSetTmp(char *pLine)
{	
	unsigned char mac_addr[6] = {0};
	int ret = 0;
	
	ret = parse_mac_cmd(pLine, mac_addr);

	if(!ret)
	{
		wifi_set_mac(mac_addr, FLASH_UNSAVA_FLAG);			
	}
	UART_Send_status(ret);
}




FLASH_FUNC void AT_ApIpSet(char *pLine)
{	
	int ret = 0;
 	ret =parse_ip_cmd(pLine);
	if(!ret)
	{
		wifi_set_ip_params(NL80211_IFTYPE_AP,  FLASH_SAVA_FLAG);
	}
	UART_Send_status(ret);
}


FLASH_FUNC void AT_ApIpSetTmp(char *pLine)
{		
	int ret = 0;
	ret =parse_ip_cmd(pLine);

	if(!ret)
	{
		wifi_set_ip_params(NL80211_IFTYPE_AP,  FLASH_UNSAVA_FLAG);
	}
	UART_Send_status(ret);
}

FLASH_FUNC void AT_StaIpSet(char *pLine)
{	

	int ret = 0;	
	ret = parse_ip_cmd(pLine);

	if(!ret)
	{
		wifi_set_ip_params(NL80211_IFTYPE_STATION,  FLASH_SAVA_FLAG);
	}
	UART_Send_status(ret);
}


FLASH_FUNC void AT_StaIpSetTmp(char *pLine)
{	
	int ret = 0;
	ret =parse_ip_cmd(pLine);
	
	if(!ret)
	{
		wifi_set_ip_params(NL80211_IFTYPE_STATION,  FLASH_UNSAVA_FLAG);
	}
	UART_Send_status(ret);
}



FLASH_FUNC void AT_WifiSetModeTmp(char *pLine)
{	
	int ret = 0;

	AT_WifiModeChange(pLine);

	UART_Send_status(ret);

}
FLASH_FUNC void AT_WifiSetMode(char *pLine)
{	
	//unsigned char wifi_mode = 0;

	int ret = 0;

	if (!memcmp("AP_MODE",pLine,strlen("AP_MODE")))	{	
		iot_printf("set Ap mode\n");	
		iot_boot_volatile_sect.wifi_param.wifimode= 1;
		atbmwifi_flash_param_wificonfig_change();	
	}
	else if(!memcmp("STA_MODE",pLine,strlen("STA_MODE"))){
		iot_printf("set sta mode\n");
		iot_boot_volatile_sect.wifi_param.wifimode = 0;
		atbmwifi_flash_param_wificonfig_change();	
	}
	else	{

	}		
	AT_WifiModeChange(pLine);	
	/*
	if(!ret)
	{
	atbm_wifi_change_mode(wifi_mode);
	}*/
	UART_Send_status(ret);
}

FLASH_FUNC void AT_WifiApfg(char *pLine)
{	
	int ret = 0;
	
	AT_WifiConfig(pLine);

	if(!ret)
	{
		if(atbm_wifi_is_iftype_station()){
			atbm_wifi_stop_station();
		}
		else {
			atbm_wifi_stopap();
		}

		atbm_wifi_startap();
		
		//iot_printf("set Ap mode to flash\n",iot_boot_volatile_sect.valid_wifi_param);
		//must save bootconfig and wifi config
		iot_boot_volatile_sect.valid_wifi_param= BOOT_VALID_FLAG;
		iot_boot_volatile_sect.wifi_param.wifimode = 1;	
		atbmwifi_flash_param_wificonfig_change();	
	}
	UART_Send_status(ret);
}
FLASH_FUNC void AT_WifiApfgTmp(char *pLine)
{	
	int ret = 0;
	
	AT_WifiConfig(pLine);
	if(!ret)
	{
		if(atbm_wifi_is_iftype_station()){
			atbm_wifi_stop_station();
		}
		else {
			atbm_wifi_stopap();
		}

		atbm_wifi_startap();
	}
	UART_Send_status(ret);
}		

FLASH_FUNC void AT_WifiStaList(char *pLine)
{	
	struct atbmwifi_stainfo info[5]={0};  
	int i;
	int realsta_num = 0;
	UN_USED_PARAMS(*pLine);
	//support num of station is 4
	realsta_num = atbm_wifi_get_sta_info(info, 5);
	for (i = 0; i < realsta_num; i++)
	{
		UART_SendString("No[%d]. station mac %02x:%02x:%02x:%02x:%02x:%02x connected\n", i,
			info[i].macAddr[0],info[i].macAddr[1],info[i].macAddr[2],info[i].macAddr[3],
			info[i].macAddr[4],info[i].macAddr[5]);
	}	
}	


FLASH_FUNC void AT_wmode()
{
   if(atbm_wifi_is_iftype_ap())
   {
	   UART_TSPT_SendStr("+ok=AP\n");
   }
   else  if(atbm_wifi_is_iftype_station())
   {
	   UART_TSPT_SendStr("+ok=STA\n");
   }
}



//AT+WIFI_ETF_START_TX  CH 7  RATE 65   40M 0 GREENFIELD 0
FLASH_FUNC void AT_WifiETF_StartTx(char *pLine)
{	
	
	int ret = 0;
	
	u8 channel =0;
	int rate = 0;
	u8 is40M = 0;
	u8 isGreenfield = 0;

	
	atbm_wifi_disconnect();
	ret  = parse_EtfStartTxCmd(pLine, &channel, &rate, &is40M, &isGreenfield);
    if(!ret)
    {
		atbmwifi_etf_start_tx(channel,rate,is40M,isGreenfield);
    }	
}

FLASH_FUNC void AT_WifiETF_StopTx(char *pLine)
{	
	atbmwifi_etf_stop_tx();
}
extern int parse_etf_rx_cmd(char *pLine,	u8 *Txchannel, u8 *Txis40M);

FLASH_FUNC void AT_WifiETF_StartRx(char *pLine)
{	
	u8 channel =0;
	u8 is40M = 0;	
	
	parse_etf_rx_cmd(pLine, &channel, &is40M);

	atbmwifi_etf_start_rx(channel, is40M);

}
FLASH_FUNC void AT_WifiETF_StopRx(char *pLine)
{	
	atbmwifi_etf_stop_rx();
}


#ifdef CONFIG_WPS

FLASH_FUNC void AT_Wifi_WPS(char *pLine)
{	
	int ret = 0;
	struct atbmwifi_vif *priv;
	Console_SetPolling(1);
	priv = atbmwifi_Get_global_variable();
	if(priv == NULL){
		wifi_printk(WIFI_DBG_ERROR, "wps: error, priv is NULL\n");
		goto _error;
	}

	if (!memcmp("PBC",pLine,strlen("PBC")) || !memcmp("pbc",pLine,strlen("pbc")))	{	
		iot_printf("WPS: start PBC\n");
		ret = atbmwps_start_pbc(priv, NULL);
	}
	else if(!memcmp("PIN",pLine,strlen("PIN")) || !memcmp("pin",pLine,strlen("pin"))){
		iot_printf("WPS: start PIN %s\n", pLine);
		CmdLine_GetToken(&pLine);
		ret = atbmwps_start_pin(priv, pLine, NULL, 0);
	}
	else if(!memcmp("STOP",pLine,strlen("STOP"))){
		iot_printf("WPS: stop\n");
		ret = atbmwps_cancel(priv, pLine, NULL, 0);
	}
	else{
		iot_printf("WPS: commond parameter invalid\n");
	}	

_error:
	UART_Send_status(ret);
	return;

} 
#endif

FLASH_FUNC void AT_wsmac()
{
	u8 buffer[64]={0};
	u8 macAddr[6] = {0};
	atbm_wifi_get_mac_addr(macAddr);
	sprintf(buffer,"+ok="MACSTR_HAILIN"\n",MAC2STR(macAddr));
	UART_TSPT_SendStr(buffer);
}



struct cli_cmd_struct ATCommands_wifi[] = 
{
	{.cmd ="AT+WIFI_STATUS",		 .fn = AT_WifiStatus, 			.next = (void*)0 },
	{.cmd ="AT+WMODE",				 .fn = AT_wmode,				.next = (void*)0 },	
	{.cmd ="AT+WIFI_SET_MODE_TMP",	 .fn = AT_WifiSetModeTmp,		.next = (void*)0 }, 
	{.cmd ="AT+WIFI_SET_MODE",		 .fn = AT_WifiSetMode,			.next = (void*)0 },	
	{.cmd ="AT+WIFI_JOIN_AP_TMP",	 .fn = AT_WConnectTmp, 			.next = (void*)0 },
	{.cmd ="AT+WIFI_JOIN_AP",		 .fn = AT_WConnect, 			.next = (void*)0 },
	{.cmd ="AT+WIFI_DISCONNECT",	 .fn = AT_WDisConnect,			.next = (void*)0 },	
	{.cmd ="AT+WIFI_SCAN",			 .fn = AT_WifiScan, 			.next = (void*)0 },
	{.cmd ="AT+WIFI_GET_SCANED",	 .fn = AT_WifiScanResult,		.next = (void*)0 },
	{.cmd ="AT+WIFI_AP_CFG_TMP",	 .fn = AT_WifiApfgTmp,			.next = (void*)0 },
	{.cmd ="AT+WIFI_AP_CFG",		 .fn = AT_WifiApfg, 			.next = (void*)0 },
	{.cmd ="AT+WIFI_AP_STA_LIST",	 .fn = AT_WifiStaList,      	.next = (void*)0 },	
	{.cmd ="AT+SMART_CFG_START",	 .fn = AT_SmartConfigStart, 	.next = (void*)0 },
	{.cmd ="AT+SMART_CFG_STOP", 	 .fn = AT_SmartConfigStop,		.next = (void*)0 },
	{.cmd ="AT+WIFI_STA_MAC_TMP",	 .fn = AT_StaMacSetTmp, 		.next = (void*)0 },
	{.cmd ="AT+WIFI_STA_MAC",	 	 .fn = AT_StaMacSet,			.next = (void*)0 },		
	{.cmd ="AT+WIFI_AP_MAC_TMP",	 .fn = AT_ApMacSetTmp, 			.next = (void*)0 },
	{.cmd ="AT+WIFI_AP_MAC",	     .fn = AT_ApMacSet,				.next = (void*)0 },
	{.cmd ="AT+WIFI_RESET_MAC",	 	 .fn = AT_RestoreMacSet,		.next = (void*)0 },	
	{.cmd ="AT+WIFI_STA_IP_TMP",	 .fn = AT_StaIpSetTmp, 			.next = (void*)0 },
	{.cmd ="AT+WIFI_STA_IP",	 	 .fn = AT_StaIpSet,				.next = (void*)0 },		
	{.cmd ="AT+WIFI_AP_IP_TMP",  	 .fn = AT_ApIpSetTmp, 			.next = (void*)0 },
	{.cmd ="AT+WIFI_AP_IP",  		 .fn = AT_ApIpSet,				.next = (void*)0 },
	{.cmd ="AT+WIFI_CHANNEL_TMP",	 .fn = AT_ChannelSetTmp, 		.next = (void*)0 },
	{.cmd ="AT+WIFI_CHANNEL",		 .fn = AT_ChannelSet, 			.next = (void*)0 },
	{.cmd ="AT+WIFI_COUNTRY_TMP",	 .fn = AT_CountryIdSetTmp,		.next = (void*)0 },
	{.cmd ="AT+WIFI_COUNTRY",	 	 .fn = AT_CountryIdSet,			.next = (void*)0 },
	{.cmd ="AT+WIFI_GET_COUNTRY",	 .fn = AT_CountryIdGet, 		.next = (void*)0 },	 	
	{.cmd ="AT+WIFI_ETF_START_TX",	 .fn = AT_WifiETF_StartTx,		.next = (void*)0 },
	{.cmd ="AT+WIFI_ETF_STOP_TX",	 .fn = AT_WifiETF_StopTx, 		.next = (void*)0 },	
	{.cmd ="AT+WIFI_ETF_START_RX",	 .fn = AT_WifiETF_StartRx,		.next = (void*)0 },
	{.cmd ="AT+WIFI_ETF_STOP_RX",	 .fn = AT_WifiETF_StopRx, 		.next = (void*)0 },	
#if CONFIG_ATBM_PRODUCT_TEST	
	{.cmd ="AT+WIFI_ETF",			 .fn = AT_Mp_ETF,				.next = (void*)0 },
	{.cmd ="AT+WIFI_ETF_CFG_FILTER", .fn = AT_WifiETF_Cfg_Filter,	.next = (void*)0 },
	{.cmd ="AT+WIFI_ETF_RMEM", 		 .fn = AT_WifiETF_ReadMemory,	.next = (void*)0 },
	{.cmd ="AT+WIFI_ETF_SAVE_EFUSE", .fn = AT_WifiETF_Save_Efuse,	.next = (void*)0 },
	{.cmd ="AT+WIFI_ETF_GET_EFUSE",  .fn = AT_WifiETF_Get_Efuse,	.next = (void*)0 },
	{.cmd ="AT+SEND",  				 .fn = AT_WifiETF_Write_User_Key,	.next = (void*)0 },
	{.cmd ="AT+WIFI_ETF_KEY_RECHECK",  	.fn = AT_WifiETF_Key_Recheck,	.next = (void*)0 },
	{.cmd ="AT+WIFI_ETF_GETMAC",  	.fn = AT_WifiETF_Get_Mac,	.next = (void*)0 },
#endif	
#ifdef CONFIG_WPS
	{.cmd ="AT+WIFI_WPS",			 .fn = AT_Wifi_WPS,				.next = (void*)0 },
#endif

	{.cmd ="AT+WSMAC",				     .fn = AT_wsmac,				.next = (void*)0 },
			
};


	

	
	
FLASH_FUNC void ATCmd_init_wifi(void)
{
	 cli_add_cmds(ATCommands_wifi,sizeof(ATCommands_wifi)/sizeof(ATCommands_wifi[0]));
 
}

