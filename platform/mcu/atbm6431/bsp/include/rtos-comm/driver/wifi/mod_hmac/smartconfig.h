#ifndef __SMART_CONFIG_H__
#define  __SMART_CONFIG_H__

#define SMART_MAX_CHANNEL 14

/*
enum smartconfig_status{
	CONFIG_ST_IDLE = 0,
	CONFIG_ST_START = 1,
	CONFIG_ST_GET_MAGIC = 2,
	CONFIG_ST_SWITCH_PAYLOAD_CH = 3,
	CONFIG_ST_GET_PAYLOAD = 4,
	CONFIG_ST_DONE_SUCCESS = 5,
	CONFIG_ST_DONE_FAIL =6,
	CONFIG_ST_DUPLICATE_FAIL =7,
	CONFIG_ST_STARTCONFIG_ERR =8,
};*/
enum smartconfig_status{
	CONFIG_ST_IDLE = 0,
	CONFIG_ST_REIDLE = 8,
	CONFIG_ST_START = 1,
	CONFIG_ST_GET_MAGIC = 2,
	CONFIG_ST_PAYLOAD = 3,
	CONFIG_ST_RE_PAYLOAD = 4,
	CONFIG_ST_GET_TOTALLEN = 5,
	CONFIG_ST_SWITCH_PAYLOAD_CH = 6,
	CONFIG_ST_GET_PAYLOAD = 7,
	CONFIG_ST_GET_PAYLOAD_CSUM = 8,
	CONFIG_ST_DONE_SUCCESS = 9,
	CONFIG_ST_DONE_FAIL =10,
	CONFIG_ST_DUPLICATE_FAIL =11,
	CONFIG_ST_STARTCONFIG_ERR =12,
	CONFIG_ST_RETRY_PAYLOAD=13,
};
#define SMARTCONFIG_DATA_BUF_LEN	 (256*2)
enum smartconfig_type{
		ATBM_SMARTCONF_CONFIGURE,		//一键配置，smartconfig 配置ssid和pwd
		AIRKISS_CONFIGURE ,
		JOYLINK_CONFIGURE,		//京东微联配置ssid 和pwd
		EWLINK_CONFIGURE,
		ALIYUN_CONFIGURE,		//aliyun 微联配置ssid 和pwd
};

struct smartconfig_config{
	enum smartconfig_type type;
	/*cnt : 1 ~ 10*/
	int magic_cnt;
	/*ms : 20ms ~ 200ms*/
	int magic_time;
	/*ms : 500ms ~ 10000ms*/
	int payload_time;
};

int smartconfig_start(struct smartconfig_config *cfg);
int smartconfig_stop(void);
int smartconfig_status(void);

int smartconfig_may_magic_channel(int channel);
int smartconfig_have_may_magic_channel();
void smartconfig_clear_magic_channel();
void smartconfig_set_magic_channel(int channel);

int smartconfig_fix_channel(int channel);  //not stop, smart config, and set fixed channel.

/*ewlink smartconfig api*/
void ew_smt_get_app_ip(unsigned char *u8app_ip, unsigned char	*smt_total_len);
int  ew_smartconfig_start_rx_fifo(unsigned short length,int channel,int8 rssi,u32 rx_type,u8 *mac_hdr );



typedef int (*ht40_smartconfig_rx_process_func)(unsigned short length,int channel,int8 rssi,u32 rx_type,u8 *mac_hdr );
int ht40_smartconfig_set_rx_cb(ht40_smartconfig_rx_process_func smt_rx_cb);


/*cnt : 2 ~ 10*/
int smartconfig_magic_channel_cnt(int cnt);
/*ms : 20ms ~ 200ms*/
int smartconfig_magic_channel_timeout(int ms);
/*ms : 500ms ~ 10000ms*/
int smartconfig_payload_timeout(int ms);

#endif  /*__SMART_CONFIG_H__*/