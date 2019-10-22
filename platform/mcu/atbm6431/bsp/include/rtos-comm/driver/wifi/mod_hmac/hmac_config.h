#ifndef _HMAC_CFG_H_
#define _HMAC_CFG_H_

#undef ARP_OFFLOAD
#undef CONFIG_INET
#undef CONFIG_ATBMWIFI__TESTMODE
#undef CONFIG_ATBMWIFI__ITP
#undef CONFIG_ATBMWIFI__DEBUGFS
#undef CONFIG_BT_COEX
#undef ROAM_OFFLOAD
#undef CONFIG_WAPI
#undef CONFIG_WIFI_IBSS
#undef CONFIG_ACTTION_RETURN
#undef CONFIG_P2P_PS
#undef CONFIG_SMPS
#undef CONFIG_NET_NS
/*may need define */
#undef CONFIG_WEP_SHARE_KEY
#undef DOWNLOAD_FW
#undef CONFIG_80211D
#undef CONFIG_COMBS_IFACE
/*5G support*/
#undef CONFIG_ATBMWIFI__5GHZ_SUPPORT
#undef CONFIG_5G_SUPPORT
#undef CONFIG_MONITOR
/*hostapd*/
#define NEED_AP_MLME
#define CONFIG_IEEE80211N
//#define AP_AGGREGATE_FW_FIX
//#define ATBMWIFI__SUSPEND_RESUME_FILTER_ENABLE
//#define MCAST_FWDING
//#define P2P_MULTIVIF
//#define CW1260_DETECTION_LOGIC
//#define AP_HT_CAP_UPDATE
//#define PROBE_RESP_EXTRA_IE
#define CONFIG_PM
#define CONFIG_ATBMWIFI__USE_STE_EXTENSIONS
#define CONFIG_HT_MCS_STREAM_MAX_STREAMS	1

/********firmware*************/ 
#undef USE_CR429501_ROAMING_OFFLOAD

//#ifndef WSM_MAX_NUM_LNK_AP
//#define WSM_MAX_NUM_LNK_AP              (1) //number of peer stations
//#endif //WSM_MAX_NUM_LNK_AP
#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))
#undef PRE_MUCT
#undef ARP_ADDR
#undef SDD_FILE 


#define HWBUF_EXTERN_HEADROM_LEN 	64  
#define HWBUF_EXTERN_TAILROM_LEN 	16  

#define ATBM_FAST_SLEEP_TIME        50//ms

#endif /*_HMAC_CFG_H_*/

