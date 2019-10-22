/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hal/base.h>
#include <hal/wifi.h>
#include <aos/network.h>

#include "lwipopts.h"
#include "lwip/ip_addr.h"
#include "lwip/pbuf.h"

#include "aos/cli.h"
//#include "common.h"
#include "rtos-comm/driver/wifi/mod_hmac/atbm_wifi_driver_api_export.h"

#define PORTING_DEBUG
#define LOG_AOS_HAL     printf


#define ERROR_SUCCESS 0
#define ERROR_INVALID_PARAMETER -1
#define ERROR_NOT_IMPLEMENT -2


#define IEEE80211_FCTL_FTYPE        0x000c
#define IEEE80211_FTYPE_MGMT		0x0000


struct ieee80211_hdr_alios {
	 u16 frame_control;
	 u16 duration_id;
	 u8 addr1[6];
	 u8 addr2[6];
	 u8 addr3[6];
	 u16 seq_ctrl;
	 u8 addr4[6];
}__attribute__((packed));

static int ieee80211_is_mgmt_alios(u16 fc)
{
	return (fc & IEEE80211_FCTL_FTYPE) == IEEE80211_FTYPE_MGMT;
}
extern hal_wifi_module_t sim_aos_wifi_icomm;
monitor_data_cb_t gallpktfn = NULL;
monitor_data_cb_t gmgmtpktfn = NULL;
static hal_wifi_ip_stat_t _ip_stat;
static bool factory_mode = false;
extern int ht40_smartconfig_set_rx_cb(void *smt_rx_cb);
extern void ip4_set_default_multicast_netif(void* default_multicast_netif);

extern struct netif * g_wifinetif[MAX_VMAC_NUM];
extern char * CmdLine_SkipSpace(char * line);
extern char * CmdLine_GetToken(char ** pLine);
extern int CmdLine_GetHex(char ** pLine, uint32 *pDword);
extern int CmdLine_GetInteger(char ** pLine, uint32 *pDword);
extern char * CmdLine_GetToken_String(char ** pLine);
extern void UART_Send_status(int ret);
extern void atbmwifi_flash_param_wificonfig_change();
extern int hexstr2bin(u8 *buf, const char *hex, size_t len);
s8 atbm_wifi_get_skb_rssi(struct pbuf *skb);

void inet_ntoa_icomm(ip4_addr_t *ipaddr, char *ipstr)
{
    char *tmpstr;

    tmpstr = ip4addr_ntoa(ipaddr);
    memcpy(ipstr, tmpstr, strlen(tmpstr));
}

static char hextochar(u8 hex)
{
    if(0x0 <= hex && hex <= 0x9)
        return '0' + hex;
    else if(0xa <= hex && hex <= 0xf)
        return 'A' + hex - 0xa;
    else
        return 0;
}

static void machextostr(u8 *mac, char *str)
{
    int i;
    for(i = 0; i < 6; i++){
       str[2 * i] = hextochar(mac[i] >> 4);
       str[2 * i + 1] = hextochar(mac[i] & 0xF);
    }
}


void scan_cp(struct atbmwifi_scan_result *scan_result)
{
    int i;
    hal_wifi_scan_result_t aplist;
    
	if (sim_aos_wifi_icomm.ev_cb == NULL)	
        return; 
    if (sim_aos_wifi_icomm.ev_cb->scan_compeleted == NULL)      
        return;

    aplist.ap_num = scan_result->len;
    aplist.ap_list = malloc(aplist.ap_num * sizeof(ap_list_t));
    memset(aplist.ap_list, 0, aplist.ap_num * sizeof(ap_list_t));

    for(i = 0; i < aplist.ap_num; i++){
        aplist.ap_list[i].ap_power = scan_result->info[i].rssi;
        memcpy(aplist.ap_list[i].ssid, scan_result->info[i].ssid, scan_result->info[i].ssidlen);
    }
    
    sim_aos_wifi_icomm.ev_cb->scan_compeleted(&sim_aos_wifi_icomm, (hal_wifi_scan_result_t*)&aplist, NULL);
    
    free(aplist.ap_list);
}

u8 enctypeicommtoali(u8 wpa, u8 rsn, u8 enc)
{
    u8 enctype = SECURITY_TYPE_NONE;
    
    if(enc == 0)
        enctype = SECURITY_TYPE_NONE;
    else if(enc == 1)
    {
    	if (wpa == 0 && rsn == 0)
	        enctype = SECURITY_TYPE_WEP;
	    else if(wpa == 1 && rsn == 1){
			enctype = SECURITY_TYPE_WPA2_MIXED;
	    }else if (wpa == 0 && rsn == 1){
			 enctype = SECURITY_TYPE_WPA2_MIXED;
		}else if (wpa == 1 && rsn == 0){
			enctype = SECURITY_TYPE_WPA2_MIXED;
		}
    }
    return enctype;
}

u8 enctypealitoicomm(u8 type)
{
    u8 enctype = KEY_NONE;

    if(type == SECURITY_TYPE_NONE)
        enctype = KEY_NONE;
    else if(type == SECURITY_TYPE_WEP)
        enctype = KEY_WEP;
    else if(type == SECURITY_TYPE_WPA_TKIP || type ==SECURITY_TYPE_WPA_AES ){
        enctype = KEY_WPA;
    }
    else if(type == SECURITY_TYPE_WPA2_TKIP || type == SECURITY_TYPE_WPA2_AES){
        enctype = KEY_WPA2;
    }
    else if(type == SECURITY_TYPE_WPA2_MIXED){
        enctype = KEY_WPA2;
    }
    return enctype;
}
void scan_cpadv(struct atbmwifi_scan_result *scan_result)
{
	iot_printf("atbm_wifi_scan_name %d\n",scan_result->len);
	
    int i;
    hal_wifi_scan_result_adv_t aplist;
    
	if (sim_aos_wifi_icomm.ev_cb == NULL)		
        return; 
    if (sim_aos_wifi_icomm.ev_cb->scan_compeleted == NULL)      
        return;
#if 0
	for (i = 0; i < scan_result->len; i++)
	{
		//printf("***SSID***: %s\n", scan_result->info[i].ssid);
		if(strcmp(scan_result->info[i].ssid,"KZLEVA-TESET1")==0)
		{
			while(1)
			{
				light_LED_product_test();
			}
		}

	}
#endif 
	aplist.ap_num = scan_result->len;
    aplist.ap_list = malloc(aplist.ap_num * sizeof(ap_list_adv_t));
    memset(aplist.ap_list, 0, aplist.ap_num * sizeof(ap_list_adv_t));

    for(i = 0; i < aplist.ap_num; i++){
        aplist.ap_list[i].ap_power = scan_result->info[i].rssi;
        memcpy(aplist.ap_list[i].ssid, scan_result->info[i].ssid, scan_result->info[i].ssidlen);
        memcpy(aplist.ap_list[i].bssid, scan_result->info[i].bssid, 6);
        aplist.ap_list[i].channel = scan_result->info[i].channel;
        aplist.ap_list[i].security = enctypeicommtoali(scan_result->info[i].wpa, scan_result->info[i].encrypt, scan_result->info[i].rsn);
    }

	sim_aos_wifi_icomm.ev_cb->scan_adv_compeleted(&sim_aos_wifi_icomm, (hal_wifi_scan_result_adv_t*)&aplist, NULL);
    
    free(aplist.ap_list);
}
void  wifi_connected_cb()
{
    printf("Wifi connected.[%x]\n",g_wifinetif[0]);
	ip4_set_default_multicast_netif(g_wifinetif[0]);
}

void  wifi_disconnect_cb()
{
    hal_wifi_module_t *m = hal_wifi_get_default_module();

    //printf("Wifi disconnected (reason: %d)\n", reason);
    if (m->ev_cb && m->ev_cb->stat_chg) {
        m->ev_cb->stat_chg(m, NOTIFY_STATION_DOWN, NULL);
    }

}
static void  wifi_notify_got_ip_cb()
{
    hal_wifi_module_t *m = hal_wifi_get_default_module();
    ip4_addr_t ipaddr, submask, gateway;

    memset(&_ip_stat, 0, sizeof(_ip_stat));
	
	atbm_wifi_get_ip_info((u32*)&ipaddr,(u32*)&submask,(u32*)&gateway);
    inet_ntoa_icomm(&ipaddr, _ip_stat.ip);
    inet_ntoa_icomm(&submask, _ip_stat.mask);
    inet_ntoa_icomm(&gateway, _ip_stat.gate);
   // inet_ntoa_icomm(&dnsserver, _ip_stat.dns);
	
    if (m->ev_cb && m->ev_cb->ip_got) {
        m->ev_cb->ip_got(m, &_ip_stat, NULL);
    }
}
//void atbmwifi_start_app();

static int wifi_init(hal_wifi_module_t *m)
{
    //LOG_AOS_HAL("wifi_init vggg!!\n");
	u8 mac[6];
	
    static int inited;

    if (inited) {
        printf("Wifi already started.\r\n"); 
        return 0;
    }
	//atbmwifi_start_app();
	while(atbm_wifi_is_enabled()==0){
		hal_sleep(100);
	}

    atbmwifi_RegisterEventHandler(WIFI_CONNECT_SUCCESS,wifi_connected_cb);
    atbmwifi_RegisterEventHandler(WIFI_CONNECT_LOSS,wifi_disconnect_cb);
    atbmwifi_RegisterEventHandler(WIFI_CONNECT_GOT_IP,wifi_notify_got_ip_cb);

    atbm_wifi_get_mac_addr(mac);

    inited = 1;
    printf("Wifi init success!![%02x-%02x-%02x-%02x-%02x-%02x]\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	
 	return 0;
};

static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
    //LOG_AOS_HAL("wifi_get_mac_addr!!\n");
    atbm_wifi_get_mac_addr(mac);
};

static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    u32 ipaddr, submask, gateway;
    
    if(init_para == NULL)
        return -1;
    
    LOG_AOS_HAL("aos:wifi_start!!\n");
    if(init_para->wifi_mode == SOFT_AP){
       atbm_wifi_startap();
    }
    else if(init_para->wifi_mode == STATION){
		if(strlen(init_para->wifi_key) >=8)
        	atbm_wifi_set_config((u8 *)init_para->wifi_ssid, strlen(init_para->wifi_ssid),
				(u8 *)init_para->wifi_key, strlen(init_para->wifi_key),KEY_WPA2, 0);
		else if(strlen(init_para->wifi_key) == 5)
        	atbm_wifi_set_config((u8 *)init_para->wifi_ssid, strlen(init_para->wifi_ssid),
				(u8 *)init_para->wifi_key, strlen(init_para->wifi_key),KEY_WEP, 0);
		else
        	atbm_wifi_set_config((u8 *)init_para->wifi_ssid, strlen(init_para->wifi_ssid),
				(u8 *)init_para->wifi_key,0,KEY_NONE, 0);
			
		atbm_wifi_connect_ap_auto();
        atbm_wifi_start_station();
        if(init_para->dhcp_mode == DHCP_SERVER){
            return -1;
        }else if(init_para->dhcp_mode == DHCP_DISABLE){
            ipaddr = ipaddr_addr(init_para->local_ip_addr);
            submask = ipaddr_addr(init_para->net_mask);
            gateway = ipaddr_addr(init_para->gateway_ip_addr);
			atbm_wifi_set_ipaddr(ipaddr, submask, gateway);
        };
    }
    return 0;
}

int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    u32 ipaddr, submask, gateway;
    //int ret, i;

    
    LOG_AOS_HAL("wifi_start_adv!!\n");
    if(init_para_adv == NULL)
        return -1;
    
	atbm_wifi_start_station();
    
    if(init_para_adv->dhcp_mode == DHCP_SERVER){
        return -1;
    }else if(init_para_adv->dhcp_mode == DHCP_DISABLE){
        ipaddr = ipaddr_addr(init_para_adv->local_ip_addr);
        submask = ipaddr_addr(init_para_adv->net_mask);
        gateway = ipaddr_addr(init_para_adv->gateway_ip_addr);
        //dnsserver = ipaddr_addr(init_para_adv->dns_server_ip_addr);
		atbm_wifi_set_ipaddr(ipaddr, submask, gateway);
    }
	atbm_wifi_set_config((u8 *)init_para_adv->ap_info.ssid, strlen(init_para_adv->ap_info.ssid),
		(u8 *)init_para_adv->key, strlen(init_para_adv->key),KEY_WPA2, 0);
    	
    return 0;
}

static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
	u8 mac[6];
    ip4_addr_t ipaddr, submask, gateway, dnsserver;
    if (atbm_wifi_is_get_ipaddr_ok() == false) {
        printf("WiFi station is not connected yet.\r\n");
        return -1;		
    }
	atbm_wifi_get_mac_addr(mac);
	atbm_wifi_get_ip_info((u32* )&ipaddr,(u32* )&submask,(u32* )&gateway);

    machextostr(mac, out_net_para->mac);
    inet_ntoa_icomm(&ipaddr, out_net_para->ip);
    inet_ntoa_icomm(&submask, out_net_para->mask);
    inet_ntoa_icomm(&gateway, out_net_para->gate);
    inet_ntoa_icomm(&dnsserver, out_net_para->dns);
	
    return 0;
}




static int get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
    LOG_AOS_HAL("get_link_stat!!\n");
    if(out_stat == NULL)
        return -1;
    
    out_stat->is_connected = atbm_wifi_is_connect_ok();
    if(out_stat->is_connected == 1)
    {
	    atbm_wifi_get_connected_ssid(out_stat->ssid);
		atbm_wifi_get_connected_destAddr(out_stat->bssid);
		out_stat->channel = atbm_wifi_get_channel();
		out_stat->wifi_strength = atbm_wifi_get_rssi();
    }
	return 0;
}

static void start_scan(hal_wifi_module_t *m)
{
    if(factory_mode)
	return;
    LOG_AOS_HAL("start_scan!!\n");
	if(atbm_wifi_scan() == 0)
	{
		while(atbm_wifi_scan_is_done()==0){
			iot_printf("wait scan done1\n");
			hal_sleep(500);
		}
		hal_sleep(1000);
		//iot_printf("+++++whether to go into test mode+++++\n");
	    atbm_wifi_process_scan_result(scan_cp);
	}

}

void start_scan_adv(hal_wifi_module_t *m)
{
    if(factory_mode)
	return;
	if(atbm_wifi_scan() == 0)
	{
		while(atbm_wifi_scan_is_done()==0){
			iot_printf("wait scan done1\n");
			hal_sleep(500);
		}
		hal_sleep(1000);
		//iot_printf("+++++whether to go into test mode+++++\n");
	    atbm_wifi_process_scan_result(scan_cpadv);
	}

}

static int power_off(hal_wifi_module_t *m)
{
    if(factory_mode);
	return 0;
    LOG_AOS_HAL("power_off!!\n");
	atbmwifi_stop();
    return 0;
}

static int power_on(hal_wifi_module_t *m)
{
    //Enable DUT_STA mode to make sure scan function can work
    if(factory_mode)
	return 0;
    LOG_AOS_HAL("power_on!!\n");
    atbmwifi_start(0);
    return 0;
}

static int suspend(hal_wifi_module_t *m)
{
    if(factory_mode)
        return 0;
    LOG_AOS_HAL("suspend!!\n");
    atbmwifi_stop();
    return 0;
}

static int suspend_station(hal_wifi_module_t *m)
{
    if(factory_mode)
        return 0;
    LOG_AOS_HAL("suspend_station!!\n");
    if(atbm_wifi_is_iftype_station())
		atbmwifi_stop();
    return 0;
}

static int suspend_soft_ap(hal_wifi_module_t *m)
{
    LOG_AOS_HAL("suspend_soft_ap!!\n");
    if(atbm_wifi_is_iftype_ap())
		atbmwifi_stop();
    return 0;
}

static int set_channel(hal_wifi_module_t *m, int ch)
{
   // int ret;

    if(factory_mode)
        return 0;
    LOG_AOS_HAL("set_channel!ch %d!\n",ch);
    atbmwifi_monitor_channel(ch);
    return 0;
}

void HAL_Awss_Rx_hander(struct pbuf *skb,int channel)
{
	hal_wifi_link_info_t info;
	struct ieee80211_hdr_alios *hdr;

	info.rssi = atbm_wifi_get_skb_rssi(skb);
   // LOG_AOS_HAL("HAL_Awss_Rx_hander len %d!fc %x MIB_Flags %x!\n",skb->data_len,*(u16 *)skb->payload,MIB_Flags);

	if(gallpktfn)
		gallpktfn(skb->payload, skb->data_len, &info);
	
	hdr = (struct ieee80211_hdr_alios *)skb->payload;
	if(ieee80211_is_mgmt_alios(hdr->frame_control)) {
#if 0
		if(0 == memcmp(test_mac, hdr->addr2, 6)) {
			printf("mgmt rx1:%d\n", skb->data_len);
		}		
		printf("mgmt rx,len:%d, addr1:%02X:%02X:%02X:%02X:%02X:%02X,addr2:%02X:%02X:%02X:%02X:%02X:%02X,addr3:%02X:%02X:%02X:%02X:%02X:%02X\n", 
			skb->data_len, 
			hdr->addr1[0],hdr->addr1[1],hdr->addr1[2],hdr->addr1[3],hdr->addr1[4],hdr->addr1[5],
			hdr->addr2[0],hdr->addr2[1],hdr->addr2[2],hdr->addr2[3],hdr->addr2[4],hdr->addr2[5],
			hdr->addr3[0],hdr->addr3[1],hdr->addr3[2],hdr->addr3[3],hdr->addr3[4],hdr->addr3[5]);
#endif
		if(gmgmtpktfn)
			gmgmtpktfn(skb->payload, skb->data_len, &info);
	}
}

void HAL_Awss_MGMT_Rx_hander(u8 *buff, u16 len, char rssi)
{
	hal_wifi_link_info_t info;
	struct ieee80211_hdr_alios *hdr;
	
	info.rssi = rssi;
	
	hdr = (struct ieee80211_hdr_alios *)buff;
	if(ieee80211_is_mgmt_alios(hdr->frame_control)) {
#if 0
		if(0 == memcmp(test_mac, hdr->addr2, 6)) {
			printf("mgmt rx2:%d\n", len);
		}	
		printf("mgmt rx,len:%d, addr1:%02X:%02X:%02X:%02X:%02X:%02X,addr2:%02X:%02X:%02X:%02X:%02X:%02X,addr3:%02X:%02X:%02X:%02X:%02X:%02X\n", 
			len, 
			hdr->addr1[0],hdr->addr1[1],hdr->addr1[2],hdr->addr1[3],hdr->addr1[4],hdr->addr1[5],
			hdr->addr2[0],hdr->addr2[1],hdr->addr2[2],hdr->addr2[3],hdr->addr2[4],hdr->addr2[5],
			hdr->addr3[0],hdr->addr3[1],hdr->addr3[2],hdr->addr3[3],hdr->addr3[4],hdr->addr3[5]);
#endif
		if(gmgmtpktfn)
			gmgmtpktfn(buff, len, &info);
	}

}

static void start_monitor(hal_wifi_module_t *m) 
{
    if(factory_mode)
        return ;
    LOG_AOS_HAL("start_monitor!!\n");
	 
	atbm_wifi_smart_config_start(ALIYUN_CONFIGURE);
	atbmwifi_monitor_enable();
	atbmwifi_monitor_filtermgmt();
	atbmwifi_monitor_filterdata();
}

static void stop_monitor(hal_wifi_module_t *m)
{
    if(factory_mode)
        return ;
	
    LOG_AOS_HAL("stop_monitor!!\n");
	gallpktfn = NULL;
	//ht40_smartconfig_set_rx_cb(NULL); 
	atbmwifi_monitor_disable();
}

static void register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    gallpktfn = fn;
}

static void register_wlan_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
	gmgmtpktfn = fn;
}

static int wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len)
{
	if((gallpktfn)||atbm_wifi_is_get_ipaddr_ok()){
		atbm_wifi_tx_data_force(buf, len);
	}
    return 0; // len-4=exclude FCS
}



/*scan AP. the scan ap result will iot_printf auto*/
FLASH_FUNC void AT_WifiScan(char *pLine, int len, int argc, char **argv)
{	
	UN_USED_PARAMS(pLine);
	UN_USED_PARAMS(argc);
	UN_USED_PARAMS(len);
	UN_USED_PARAMS(argv);
	atbmwifi_scan_printf(1);
	atbm_wifi_scan();
}


FLASH_FUNC void AT_WifiStatus(char *pLine, int len, int argc, char **argv)
{
	u8 mac_addr[6] ={0};
	u8 ssid[32] ={0};

	
	UN_USED_PARAMS(pLine);
	atbm_wifi_get_mac_addr(mac_addr);
	if(atbm_wifi_is_iftype_ap()){
		printf("[AP mode] \n");
		printf("mac	:"MACSTR"\n",MAC2STR(mac_addr));
		printf("channel	:%d \n",atbm_wifi_get_channel());
	}
	else if(atbm_wifi_is_iftype_station()){
		printf("[STA mode] \n");
		printf("mac	:["MACSTR"] \n",MAC2STR(mac_addr));
		if(atbm_wifi_is_connect_ok()){
			printf("status	:[connect]\n");
		}
		else {
			printf("status	:[not connect]\n");
		}
		atbm_wifi_get_connected_ssid(ssid);
		printf("ssid	:[%s] \n",ssid);
		
		atbm_wifi_get_connected_destAddr(mac_addr);
		printf("bssid	:["MACSTR"]\n",MAC2STR(mac_addr));
		printf("channel :%d \n",atbm_wifi_get_channel());
	}
	else {
		printf("WIFI MODE[%d] ERROR!!\n",atbm_wifi_get_iftype());
	}
	
}
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
	printf("key_mgmt %s %d\n",pLine,key_mgmt);
	return key_mgmt;
} 

FLASH_FUNC void AT_WifiConfig(int argc, char **argv)
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
	int id =1;
	

	//printf("\nAT_WifiConfig %s\n",pLine);
	if (!memcmp("KEY_MGMT",argv[id],strlen("KEY_MGMT")))
	{
		id++;
		printf("KEY_MGMT %s\n",argv[id]);
		//
		key_mgmt = do_config_keymgmt(argv[id]);
		id++;
	}
	
	
	if(id > argc)
		return ;
	
	if (!memcmp("PWD",argv[id],strlen("PWD")))
	{
		id++;
		tmpdata = strlen(argv[id]);
		str = argv[id];
		//
		switch(key_mgmt){

			case KEY_WPA:
			case KEY_WPA2:
				if((tmpdata<8)||(tmpdata>=64)){

					printf("keylen err(%d)!\n",tmpdata);
					return;
				}
				break;

			case KEY_WEP:
			case KEY_WEP_SHARE:
				if((tmpdata!=5)&&(tmpdata!=13)&&(tmpdata!=10)&&(tmpdata!=26)){

					printf("keylen err! need len=5 or 13 ,but now[%d]\n",tmpdata);
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
				printf("key mgmt err!\n");
				return;
				//break;

		}
		
		password = (u8 *)str;
		password_len =(u8) tmpdata;
		id++;
	}

	
	if(id > argc)
		return ;

	if (!memcmp("KEYID",argv[id],strlen("KEYID")))
	{
		id++;
		str = argv[id];
		//
		CmdLine_GetInteger(&str, &tmpdata);
		printf("KEYID %d %s\n",tmpdata,str );
		//
		switch(key_mgmt){

			case KEY_WEP:
			case KEY_WEP_SHARE:
				key_id = tmpdata;
				if(/*(key_id<0)||*/(key_id>3)){
					printf("keyid err!\n");
					return;
				}
				break;
			case KEY_WPA:
			case KEY_WPA2:
			case KEY_NONE:
			default:
				printf("key mgmt err!\n");
				return;
				//break;

		}
		id++;
	}
	
	if(id > argc)
		return ;
	
	if (!memcmp("SSID",argv[id],strlen("SSID")))
	{
		id++;
		str = argv[id];
		//printf("PWD1str %s pLine %s\n",str,pLine);
		//printf("PWD2str %s\n",str);
		//
		tmpdata = strlen(str);
		if((tmpdata <1)||(tmpdata > 32)){

			printf("ssid err!\n");
			return;
		}
		ssid = (u8 *)str;
		ssid_len =(u8 ) tmpdata;
		printf("ssid_len %d %s\n",ssid_len,ssid );
		id++;
	}
	atbm_wifi_set_config(ssid, ssid_len, password, password_len, key_mgmt, key_id);
}


FLASH_FUNC void AT_WConnect(char *pLine, int len, int argc, char **argv)
{	
	if(!atbm_wifi_is_enabled()){
		printf("not support not enabled!\n");
		UART_Send_status(1);
		return;
	}
	
	if(atbm_wifi_is_iftype_station()){
		atbm_wifi_disconnect();
		AT_WifiConfig(argc,argv);
		atbm_wifi_connect_ap();
		//save to flash
	}	
	atbmwifi_flash_param_wificonfig_change();
	
	UART_Send_status(0);
}

extern const char SYS_SwTitle[];
extern void CmdLine_GetPlatformVar(char *buf, int len);

void AT_GetVer(char *pLine, int len, int argc, char **argv)
{	
	char strTmp[64] = {0};	

	CmdLine_GetPlatformVar(strTmp, sizeof(strTmp));
	printf("%s\n", strTmp);
}



void AT_GetSysStatus(char *pLine, int len, int argc, char **argv)
{	
	
	char strTmp[64] = {0};	
	printf("\n\n%s\n",SYS_SwTitle);
	CmdLine_GetPlatformVar(strTmp, sizeof(strTmp));
	printf("%s\n", strTmp);
	printf("OS	: AOS\n");
	printf("wifi	: 11n 1X1 20M\n");
    AT_WifiStatus(NULL,0,0,0);
	printf("\ncpu	: 160Mhz\n");
	printf("memory total: 168Kbyte\n");
	printf("memory free: %d byte\n",hal_sys_mem_free_size_get());
	printf("TCPIP	: LWIP\n");
	printf("lwip config: lwipopts.h\n");
}



char *AT_Cmd_Build_CmdLine(int argc, char **argv)
{
	char * cmdline = zalloc(512);
	int i=0;
	int len=0;
	if(cmdline == NULL)
		return NULL;

	for(i=1;i<=argc;i++){
		memcpy(cmdline+len,argv[i],strlen(argv[i]));
		len += strlen(argv[i]);
		cmdline[len] = ' ';
		len++;
	}
	cmdline[len]=0;
	return  cmdline;
}
//AT+WIFI_ETF_START_TX  CH 7  RATE 65   40M 0 GREENFIELD 0
extern int atbmwifi_etf_start_tx(int channel,int rate_value,int is_40M, int greenfield);
extern  int atbmwifi_etf_stop_tx();
extern int atbmwifi_etf_start_rx(int channel ,int is_40M);
extern  int atbmwifi_etf_stop_rx();
extern int parse_EtfStartTxCmd(char *pLine,	u8 *Txchannel, int *Txrate,	u8 *Txis40M,	u8 *TxisGreenfield);
extern void parse_etf_rx_cmd(char *pLine,	u8 *Txchannel, u8 *Txis40M);
FLASH_FUNC void AT_WifiETF_StartTx(char *pLine, int len, int argc, char **argv)
{	
	
	int ret = 0;
	u8 channel =0;
	int rate = 0;
	u8 is40M = 0;
	u8 isGreenfield = 0;

	pLine = AT_Cmd_Build_CmdLine(argc,argv);
	
	if(pLine == NULL)
		return ;
	atbm_wifi_disconnect();
	ret  = parse_EtfStartTxCmd(pLine, &channel, &rate, &is40M, &isGreenfield);
    if(!ret)
    {
		atbmwifi_etf_start_tx(channel,rate,is40M,isGreenfield);
    }
	free(pLine);
}

FLASH_FUNC void AT_WifiETF_StopTx(char *pLine, int len, int argc, char **argv)
{	
	atbmwifi_etf_stop_tx();
}

FLASH_FUNC void AT_WifiETF_StartRx(char *pLine, int len, int argc, char **argv)
{	
	u8 channel =0;
	u8 is40M = 0;	
	
	pLine = AT_Cmd_Build_CmdLine(argc,argv);
	
	if(pLine == NULL)
		return ;
	parse_etf_rx_cmd(pLine, &channel, &is40M);

	atbmwifi_etf_start_rx(channel, is40M);
	free(pLine);

}
FLASH_FUNC void AT_WifiETF_StopRx(char *pLine, int len, int argc, char **argv)
{	
	atbmwifi_etf_stop_rx();
}




struct cli_command  wifi_debug_cli_cmd[] = {
    { "AT+WIFI_SCAN", 	 "wifi scan AP", AT_WifiScan },
    { "AT+WIFI_STATUS",  "get wifi status", AT_WifiStatus },
	{ "AT+WIFI_JOIN_AP", "wifi join AP", AT_WConnect },	
	{ "AT+WIFI_ETF_START_TX", "wifi etf start tx test", AT_WifiETF_StartTx }, 
	{ "AT+WIFI_ETF_STOP_TX", "wifi etf stop tx test", AT_WifiETF_StopTx }, 
	{ "AT+WIFI_ETF_START_RX", "wifi etf start rx test", AT_WifiETF_StartRx }, 
	{ "AT+WIFI_ETF_STOP_RX", "wifi etf stop rx test", AT_WifiETF_StopRx }, 
	{ "AT+GET_VER", 		 "iot get system version", AT_GetVer }, 
	{ "AT+GET_SYS_STATUS",	 "iot get system status", AT_GetSysStatus }, 
	
};

void start_debug_mode(hal_wifi_module_t *m)
{
    aos_cli_register_commands(&wifi_debug_cli_cmd[0], sizeof(wifi_debug_cli_cmd) / sizeof(struct cli_command));

}

void stop_debug_mode(hal_wifi_module_t *m)
{
}


hal_wifi_module_t sim_aos_wifi_icomm = {
    .base.name           = "sim_aos_wifi_icomm",
    .init                =  wifi_init,
    .get_mac_addr        =  wifi_get_mac_addr,
    .start               =  wifi_start,
    .start_adv           =  wifi_start_adv,
    .get_ip_stat         =  get_ip_stat,
    .get_link_stat       =  get_link_stat,
    .start_scan          =  start_scan,
    .start_scan_adv      =  start_scan_adv,
    .power_off           =  power_off,
    .power_on            =  power_on,
    .suspend             =  suspend,
    .suspend_station     =  suspend_station,
    .suspend_soft_ap     =  suspend_soft_ap,
    .set_channel         =  set_channel,
    .start_monitor       =  start_monitor,
    .stop_monitor        =  stop_monitor,
    .register_monitor_cb =  register_monitor_cb,
    .register_wlan_mgnt_monitor_cb = register_wlan_mgnt_monitor_cb,
    .wlan_send_80211_raw_frame = wlan_send_80211_raw_frame,
    .start_debug_mode = start_debug_mode,
    .stop_debug_mode = stop_debug_mode
};
