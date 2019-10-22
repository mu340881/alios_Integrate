/**************************************************************************************************************
 * altobeam IOT API
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/

#ifndef ATBM_WIFI_DRIVER_API_H
#define ATBM_WIFI_DRIVER_API_H
#ifdef __cplusplus
extern "C" {
#endif
	

//typedef int int32_t;
//typedef unsigned int uint32_t;

enum nl80211_iftype {
	NL80211_IFTYPE_STATION,
	NL80211_IFTYPE_AP,
	NL80211_IFTYPE_ADHOC,
	NL80211_IFTYPE_MONITOR,
	NL80211_IFTYPE_P2P_CLIENT,
	NL80211_IFTYPE_P2P_GO,

	/* keep last */
	NUM_NL80211_IFTYPES,
	NL80211_IFTYPE_MAX = NUM_NL80211_IFTYPES - 1
};

typedef enum _SECURITY_TYPE
{
	KEY_NONE = 0,
	KEY_WEP,
	KEY_WEP_SHARE,
	KEY_WPA,
	KEY_WPA2,
	KEY_MAX,
}SECURITY_TYPE;

/* Authentication algorithms */
#define WLAN_AUTH_OPEN 0
#define WLAN_AUTH_SHARED_KEY 1



#define MAX_SCAN_INFO_NUM 40
struct atbmwifi_scan_result_info{
	u8 ssid[33];
	u8 bssid[6];
	u8 ssidlen;
	u8 channel;
	u8 ht:1,
	   wpa:1,
	   rsn:1,
	   wps:1,
	   p2p:1,
	   b40M:1,
	   encrypt:1;
	s8 rssi;
};

/*
	len: it is num of info list
*/
struct atbmwifi_scan_result{
	int len;
	struct atbmwifi_scan_result_info *info;
};
typedef void (*atbm_wifi_scanRet_cb_t)(struct atbmwifi_scan_result *scan_result);


typedef void (*ATBM_WIFI_EVENT_HANDLER)(void);
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
	WIFI_CONNECT_GOT_IP,
	
	WIFI_EVENT_MAX
};
/*user get current wifi configure struct*/
struct atbmwifi_configure {
	u8 ssid[32];
	u8 ssid_len;
	u8 bssid[6/*ETH_ALEN*/];
	u8 password_len;
	u8 password[64];
	u8 key_mgmt;
	u8 key_id;
	u16 auth_alg;
};

enum wifi_country{
	country_chinese =0,
	country_usa = 1,
	country_europe =2,
	country_japan =3,
	country_canada = 4,
	country_australia = 5,
	country_Israel =6,//G channel 3-9
	country_Mexico =7,//G channel 10-.11
	country_France = 8,//12~13
	country_MAX =9,//G channel 1-.13
};


enum smartconfig_type{
		ATBM_SMARTCONF_CONFIGURE,		//一键配置，smartconfig 配置ssid和pwd
		AIRKISS_CONFIGURE ,
		JOYLINK_CONFIGURE,		//京东微联配置ssid 和pwd
		EWLINK_CONFIGURE,
		ALIYUN_CONFIGURE,		//aliyun 微联配置ssid 和pwd
};

struct smartconfig_config {
	enum smartconfig_type type;
	/*cnt : 1 ~ 10*/
	int magic_cnt;
	/*ms : 20ms ~ 200ms*/
	int magic_time;
	/*ms : 500ms ~ 10000ms*/
	int payload_time;
};

struct atbmwifi_stainfo{
	u8 macAddr[6];
};

/****************************************************************************
* Function:   	atbm_wifi_startap
*
* Purpose:   	This function is used to start ap
*
* Parameters: none
*
* Returns:	none
****************************************************************************/
void atbm_wifi_startap(void);
/****************************************************************************
* Function:   	atbm_wifi_stopap
*
* Purpose:   	This function is used to stop ap
*
* Parameters: none
*
* Returns:	none
****************************************************************************/
void atbm_wifi_stopap(void);
/****************************************************************************
* Function:   	atbm_wifi_start_dhcp_server
*
* Purpose:   	This function is used to start dhcp server for ap mode
*
* Parameters: none
*
* Returns:	Returns 0 if succeed, otherwise a negative error code.
****************************************************************************/
int atbm_wifi_start_dhcp_server(void);
/****************************************************************************
* Function:   	atbm_wifi_stop_dhcp_server
*
* Purpose:   	This function is used to stop dhcp server for ap mode
*
* Parameters: none
*
* Returns:	Returns 0 if succeed, otherwise a negative error code.
****************************************************************************/
int atbm_wifi_stop_dhcp_server(void);
/****************************************************************************
* Function: 	atbm_wifi_get_sta_num
*
* Purpose:	This function is used to get connected station num of ap
*
* Parameters: none
*
* Returns:	station num
****************************************************************************/
u8 atbm_wifi_get_sta_num(void);
/****************************************************************************
* Function: 	atbm_wifi_get_config
*
* Purpose:	This function is used to get current configure
*
* Parameters: point to config buffer
*
* Returns:	none
****************************************************************************/
void atbm_wifi_get_config(struct atbmwifi_configure *config);
/****************************************************************************
* Function: 	atbm_wifi_set_config
*
* Purpose:	This function is used to set  configure
*
* Parameters: essid, essid_len,key,key_len,key_mgmt,keyid
*
* Returns:	none
****************************************************************************/
void atbm_wifi_set_config(u8 *essid,int essid_len,u8 *key,int key_len,int key_mgmt,int keyid);
/****************************************************************************
* Function: 	atbm_wifi_get_iftype
*
* Purpose:	This function is used to get iftype
*
* Parameters: none
*
* Returns:	return iftype.
****************************************************************************/
 u8 atbm_wifi_get_iftype(void);
/****************************************************************************
* Function: 	atbm_wifi_is_iftype_ap
*
* Purpose:	This function is used to judge whether it is ap
*
* Parameters: none
*
* Returns:	return 1 if it is ap, return 0 if it is not ap.
****************************************************************************/
u8 atbm_wifi_is_iftype_ap(void);
/****************************************************************************
* Function: 	atbm_wifi_is_iftype_ap
*
* Purpose:	This function is used to judge whether it is station
*
* Parameters: none
*
* Returns:	return 1 if it is station, return 0 if it is not station.
****************************************************************************/
u8 atbm_wifi_is_iftype_station(void);
/****************************************************************************
* Function: 	atbm_wifi_is_iftype_ap
*
* Purpose:	This function is used to judge whether it is p2p_client
*
* Parameters: none
*
* Returns:	return 1 if it is p2p_client, return 0 if it is not p2p_client.
****************************************************************************/
u8 atbm_wifi_is_iftype_p2p_client(void);
/****************************************************************************
* Function: 	atbm_wifi_get_mac_addr
*
* Purpose:	This function is used to get mac address
*
* Parameters: array  of  mac address
*
* Returns:	none
****************************************************************************/
void atbm_wifi_get_mac_addr(u8 *mac_addr);
/****************************************************************************
* Function: 	atbm_wifi_is_enabled
*
* Purpose:	This function is used to judge whether atbm wifi is enabled
*
* Parameters: none
*
* Returns:	return 1 if it is enabled, return 0 if it is not enabled
****************************************************************************/
 u8 atbm_wifi_is_enabled(void);
/****************************************************************************
* Function: 	atbm_wifi_connect_ap
*
* Purpose:	This function is used to start connect to ap using user's configure
*
* Parameters: none
*
* Returns:	return 0 if success, other if failed
****************************************************************************/
int atbm_wifi_connect_ap(void);
/****************************************************************************
* Function: 	atbm_wifi_is_connect_ok
*
* Purpose:	This function is used to judge whether station is connect ok
*
* Parameters: none
*
* Returns:	return 1 if connect ok, return 0 if not connected
****************************************************************************/
 u8 atbm_wifi_is_connect_ok(void);
/****************************************************************************
* Function: 	atbm_wifi_get_connected_ssid
*
* Purpose:	This function is used to get station connected ssid of ap
*
* Parameters: point to ssid buffer, length >=32
*
* Returns:	none
****************************************************************************/
 void atbm_wifi_get_connected_ssid(u8 *ssid);
/****************************************************************************
* Function: 	atbm_wifi_get_connected_destAddr
*
* Purpose:	This function is used to get station connected address of destination
*
* Parameters: point to destMacAddr buffer, length = 6
*
* Returns:	none
****************************************************************************/
 void atbm_wifi_get_connected_destAddr(u8 *destMacAddr);
/****************************************************************************
* Function: 	atbm_wifi_get_channel
*
* Purpose:	This function is used to get station connected channel
*
* Parameters: none
*
* Returns:	return channel num
****************************************************************************/
 u8 atbm_wifi_get_channel(void);
/****************************************************************************
* Function: 	atbm_wifi_get_country
*
* Purpose:	This function is used to get wifi channel country
*
* Parameters: country id (see: enum wifi_country)
*
* Returns:	none
****************************************************************************/

int atbm_wifi_get_country();

/****************************************************************************
* Function: 	atbm_wifi_disconnect
*
* Purpose:	This function is used to disconnect
*
* Parameters: none
*
* Returns:	none
****************************************************************************/
void atbm_wifi_disconnect(void);
/****************************************************************************
* Function: 	atbm_wifi_scan
*
* Purpose:	This function is used to start scan
*
* Parameters: none
*
*
* Returns:	success return 0,else fail
****************************************************************************/
int atbm_wifi_scan(void);
/****************************************************************************
* Function: 	atbm_wifi_scan_is_done
*
* Purpose:	This function is used to get  scan is done
*
* Parameters: none
*
* Returns:	if scan done return scan num, else 0
****************************************************************************/
int atbm_wifi_scan_is_done(void);

/****************************************************************************
* Function: 	atbmwifi_scan_printf
*
* Purpose:	This function is used to printf  "scan AP info" on uart
*
SSID: JI
        channel 1  rssi -43
        ht[1] wpa[0] rsn[0] enc[1]
SSID: NV
        channel 1  rssi -49
        ht[1] wpa[0] rsn[0] enc[1]
        .......
* Parameters: enable: 1 printf, 0 not printf
*
* Returns:	if success 0,else -1
****************************************************************************/

int atbmwifi_scan_printf(bool enable);

/****************************************************************************
* Function: 	atbm_wifi_get_scan_result
*
* Purpose:	This function is used to get scan result
*
* Parameters: callback function for processing scan result
*
* Returns:	none
****************************************************************************/
void atbm_wifi_process_scan_result(atbm_wifi_scanRet_cb_t proc_scanRet_cb);
/****************************************************************************
* Function: 	atbm_wifi_get_netdev
*
* Purpose:	This function is used to get net device struct
*
* Parameters: none
*
* Returns:	pointer to net_device
****************************************************************************/
struct net_device *atbm_wifi_get_netdev(void);

/****************************************************************************
* Function:   	atbm_wifi_register_event_handler
*
* Purpose:   	This function is used to register event handler
*
* Parameters: Event_ID and Event_ID callback funtion
*
* Returns:	none
****************************************************************************/
void atbm_wifi_register_event_handler(enum WIFI_EVENT_ID event, ATBM_WIFI_EVENT_HANDLER EventHandler);
/****************************************************************************
* Function:   	atbm_wifi_wait_connect
*
* Purpose:   	This function is used to wait sta connect to ap
*
* Parameters: none
*
* Returns:	Returns TRUE if succeed, FALSE if failed.
****************************************************************************/
int atbm_wifi_wait_connect(void);
/****************************************************************************
* Function:   	atbm_wifi_wait_get_ipaddr_ok
*
* Purpose:   	This function is used to wait sta get dhcp ipaddr from ap
*
* Parameters: none
*
* Returns:	Returns TRUE if succeed, FALSE if failed.
****************************************************************************/

int atbm_wifi_wait_get_ipaddr_ok(void);

/****************************************************************************
* Function:   	atbm_wifi_set_mac_addr
*
* Purpose:   	This function is used to set mac addr
*
* Parameters: point to mac address
*
* Returns:	none
****************************************************************************/
void atbm_wifi_set_mac_addr(u8 *mac);
/****************************************************************************
* Function:   	atbm_wifi_start_station
*
* Purpose:   	This function is used to start station
*
* Parameters: none
*
* Returns:	none
****************************************************************************/
void atbm_wifi_start_station(void);

/****************************************************************************
* Function:   	atbm_wifi_stop_station
*
* Purpose:   	This function is used to stop station
*
* Parameters: none
*
* Returns:	none
****************************************************************************/
void atbm_wifi_stop_station(void);
/****************************************************************************
* Function:   	atbm_wifi_set_country_code
*
* Purpose:   	This function is used to set country code
*
* Parameters: country code
*
* Returns:	none
****************************************************************************/
void atbm_wifi_set_country_code(int country);
/****************************************************************************
* Function:   	atbm_wifi_smart_config_start
*
* Purpose:   	This function is used to start smart configure
*
* Parameters: none
*
* Returns:	return 0 if success, others if failed.
****************************************************************************/
int atbm_wifi_smart_config_start(enum smartconfig_type type);
/****************************************************************************
* Function:   	atbm_wifi_smart_config_stop
*
* Purpose:   	This function is used to stop smart configure
*
* Parameters: none
*
* Returns:	return 0 if success, others if failed.
****************************************************************************/
int atbm_wifi_smart_config_stop(void);
int atbm_wifi_is_smartconfig_enable();
/****************************************************************************
* Function: 	atbm_wifi_get_sta_info
*
* Purpose:	This function is used to get station info in ap mode
*
* Parameters: info, is a point to array for station infomation; count, array len
*
* Returns:	return real count of station infomation
****************************************************************************/
int atbm_wifi_get_sta_info(struct atbmwifi_stainfo *info, int count);
/****************************************************************************
* Function: 	atbm_wifi_set_channel
*
* Purpose:	This function is used to set ap channel
*
* Parameters: channel number
*
* Returns:	none
****************************************************************************/
 void atbm_wifi_set_channel(u8 channel);

/*
change wifi mode
mode : 0 stamode
mode : 1 APmode
//标识需要进行模式切换，如果之前是sta 就会切换成ap，
如果之前是ap 就会切换为sta，
*/
int atbm_wifi_change_mode(u8 mode);

/****************************************************************************
* Function:   	atbm_wifi_set_ps_mode
*
* Purpose:   	This function is used to enable or stop  wifi sta power save mode 
*
* Parameters: enable 
*
* Returns:	return 0 if success, others if failed.
****************************************************************************/

int atbm_wifi_set_ps_mode(u8 enable);
/****************************************************************************
* Function:   	atbmwifi_stop
*
* Purpose:   	This function is used to stop wifi
*
* Parameters: none
*
* Returns:	none
****************************************************************************/
int atbmwifi_stop();
/****************************************************************************
* Function:   	atbm_wifi_start
*
* Purpose:   	This function is used to start wifi
*
* Parameters: none
*
* Returns:	none
****************************************************************************/
int atbmwifi_start(bool is_ap);
/****************************************************************************
* Function:   	atbm_wifi_use_efuse_mac
*
* Purpose:   	This function is used to set mac addr from efuse
*
* Parameters: none
*
* Returns:	Returns 0 if succeed, other if failed.
****************************************************************************/
int atbm_wifi_use_efuse_mac();

/****************************************************************************
* Function:   	atbmwifi_monitor_enable
*
* Purpose:   	This function is used to  enable monitor mode,
			this is just used for smartconfig,not all data will rxhandle, just  rxhandle A1/A3  multicase data frame
*
* Parameters: none
*
* Returns:	none
****************************************************************************/

void atbmwifi_monitor_enable();
void atbmwifi_monitor_disable();
/****************************************************************************
* Function:   	atbmwifi_monitor_channel
*
* Purpose:   	This function is used to set monior channel
*
* Parameters: channel id
*
* Returns:	Returns 0 if succeed, other if failed.
****************************************************************************/
int atbmwifi_monitor_channel(uint8 channel);

void atbm_wifi_tx_data_force(u8 *data, u16 len);

void atbmwifi_monitor_filtermgmt();
void atbmwifi_monitor_filterdata();

void atbm_wifi_get_ip_info(u32* ipaddr,u32 * netmask,u32 * gw);
void atbm_wifi_set_ipaddr(u32 ipaddr,u32 netmask,u32 gw);
int atbm_wifi_get_rssi();

void atbm_wifi_not_connect_ap_auto();
void atbm_wifi_connect_ap_auto();

void atbmwifi_RegisterEventHandler(enum WIFI_EVENT_ID event, ATBM_WIFI_EVENT_HANDLER EventHandler);

//s8 atbm_wifi_get_skb_rssi(struct pbuf *skb);
void atbm_clear_wifi_powerbusy_event();
int atbm_wifi_is_get_ipaddr_ok(/*struct netif * netdev*/);


#ifdef __cplusplus
	}
#endif

#endif	/* ATBM_WIFI_DRIVER_API_H */


