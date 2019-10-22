/**************************************************************************************************************
* altobeam iot at cmd file 
*
* Copyright (c) 2018, altobeam.inc   All rights reserved.
* Permission is hereby granted for use on Altobeam IOT 6421 only,
* in which case ,it is free to  redistribute it and/or modify
*
*****************************************************************************************************************/

#ifndef APP_MQTT_AT_CMD_H_
#define APP_MQTT_AT_CMD_H_

#include "hmac_timer.h"

#include "iot_import.h"
#include "iot_export.h"
#include "aliyun_config.h"


extern char * CmdLine_SkipSpace(char * line);
extern char UART_TSPT_RxStr[TSPT_RX_BUF_SIZE];
extern void UART_TSPT_Sendbuff(char *str,int num);
extern void UART_TSPT_SendStr(const char *str);
extern char * UART_TSPT_NetData_GetLine(void);
extern u32_t Uart_TSPT_Echo_flag;



typedef enum atbm_mqtt_server_id {
	ATBM_MQTT_TEST = 0,    /* MQTT server server*/
	ATBM_MQTT_ALIYUN        /*Ali YUN specific mqtt server*/
}ATBM_MQTT_SERVER_ID;


typedef enum atbm_mqtt_link_type {
	ATBM_MQTT_INVALID = 0,
	ATBM_MQTT_DIRECT,          /*MQTT direct, no TLS used*/
	ATBM_MQTT_TLS_DIRECT,	   /*MQTT direct,  TLS used*/
	ATBM_MQTT_TLS_GUIDER       /*MQTT message protected by TLS*/
}ATBM_MQTT_LINK_TYPE;

typedef enum atbm_mqtt_status_type {
	ATBM_MQTT_STATUS_UNINIT = 0,
	ATBM_MQTT_CONNECTED,
	ATBM_MQTT_DISCONNECTING,
	ATBM_MQTT_DISCONNECTED
}ATBM_MQTT_STATUS_TYPE;


#define ATBM_MQTT_CLIENT_ID_LEN 32
#define ATBM_MQTT_USER_NAME_LEN 32
#define ATBM_MQTT_PASSWORD_LEN 32


#define ATBM_MQTT_PRODUCT_KEY_LEN 32
#define ATBM_MQTT_DEVICE_NAME_LEN 32
#define ATBM_MQTT_DEVICE_SECRET_LEN 64

#define ATBM_MAX_MQTT_TOPIC_LEN 64

typedef void (* atbm_mqtt_sent_callback)(int status);
typedef struct{

	ATBM_MQTT_SERVER_ID mqtt_sever_id;
	ATBM_MQTT_LINK_TYPE linkType;
	char  host[128];       /*MQTT server Host name*/
	u16_t port;            /*MQTT server host port*/
	u32_t ip;
	union
	{	
		struct
		{
			char client_id[ATBM_MQTT_CLIENT_ID_LEN];
			char user_name[ATBM_MQTT_USER_NAME_LEN];
			char password[ATBM_MQTT_PASSWORD_LEN];		
		} mqtt_user;         /* mqtt client, client id, user name and password is usually used. */
		struct
		{
			char product_key[ATBM_MQTT_PRODUCT_KEY_LEN];
			char device_name[ATBM_MQTT_DEVICE_NAME_LEN];
			char device_secret[ATBM_MQTT_DEVICE_SECRET_LEN];		
		} mqtt_aliyun_user;    /*Aliyun IOT specific  key input information of MQTT client*/
	}u;
}MQTT_CLIENT_INFO;



/*MQTT client context for AT command*/
typedef struct atbm_mqtt_context{

	MQTT_CLIENT_INFO       mqtt_client_info;
	ATBM_MQTT_STATUS_TYPE  mqtt_status;
	char *msg_buf;
	char *msg_readbuf;
	void *at_mqtt_client;
	atbm_mqtt_sent_callback send_callback;
}ATBM_MQTT_CONTEXT;



/********************************************************************
* Function: AT_MqttClientInfo
* Description: Set MQTT client user information, for MQTT test the user information is not required, depends on server config.
*
* Input: AT command line string, will parsing the follwoing field.
*		CLIENT_ID -- ID of MQTT client
*		USER_NAME - MQTT user name
*		PWD ------- MQTT user password
* Sample:
* AT+MQTT_CLIENT_INFO  CLIENT_ID=abcdxxxx, USER_NAME=abcdxxx, PWD=12345678xx
*
* Retrun: None
********************************************************************/
FLASH_FUNC void  AT_MqttClientInfo(char* pLine);





/********************************************************************
* Function: AT_MqttClientInfoAli
* Description: Set MQTT client user information, here use ali yun registered device information.
*
* Input: AT command line string, will parsing the follwoing field.
*		PRODUCT_KEY ---- Aliyun Product key
*		DEVICE_NAME ---- device name, each device has a unique name
*		DEVICE_SECRET -- the device's secret password.
*       When test should register Aliyun iot account and create your product , then use it as test device information.
* Sample:
* AT+MQTT_CLIENT_INFO_ALI  PRODUCT_KEY=a16INy8YJxw, DEVICE_NAME=atbm6421_test0_1, DEVICE_SECRET=RWhL1AUgXe18QyN07EdcmGIgcUaIfAqE
*
* Retrun: None
********************************************************************/
FLASH_FUNC void  AT_MqttClientInfoAli(char* pLine); 



/*********************************************************************
* Function: AT_MqttConnect
* Description: 
* Connect with MQTT sever with TLS or without TLS.
*
* Input: 	AT command line string, will parsing the follwoing field:
* AT+MQTT_CONNECT=  <type>,<remote host>,<remote port>
* Parameters£º
*       <type>   : connect type "TCP", "TLS"
*       <host>   : mqtt host name or IP address
*       <remote port> mqtt port
* Response:OK
*      	   ERROR
*       	   ALREADY CONNECTED
Sample:
1, Aliyun no TLS
AT+MQTT_CONNECT=TCP, iot-as-mqtt.cn-shanghai.aliyuncs.com,1883
2, Aliyun with TLS
AT+MQTT_CONNECT=TLS, iot-as-mqtt.cn-shanghai.aliyuncs.com,1883

3, MQTT test no TLS
AT+MQTT_CONNECT=TCP, iot.eclipse.org,1883
4, MQTT test with TLS
AT+MQTT_CONNECT=TLS, iot.eclipse.org,8883
*********************************************************************/
FLASH_FUNC void  AT_MqttConnect(char* pLine);




FLASH_FUNC void  AT_MqttPublish(char* pLine);

FLASH_FUNC void  AT_MqttSubscribe(char* pLine);

FLASH_FUNC void  AT_MqttUnSubscribe(char* pLine);


/********************************************************************
* Function: AT_MqttClose
* Description: Set MQTT client user information, for MQTT test the user information is not required, depends on server config.
*
* Input: 	AT command line string, will parsing the follwoing field.
*		CLIENT_ID -- ID of MQTT client
*		USER_NAME - MQTT user name
*		PWD ------- MQTT user password
* Sample:
* AT+MQTT_CLIENT_INFO  CLIENT_ID=abcdxxxx, USER_NAME=abcdxxx, PWD=12345678xx
*
* Retrun: None
********************************************************************/
FLASH_FUNC void  AT_MqttClose(char* pLine);


/********************************************************************
* Function: AT_MqttStatus
* Description: get mqtt network status, get MQTT_CONNECTED or else not connected.
*
* Input: 	none

* Response: MQTT_CONNECTED
*			MQTT_DISCONNECTED
*			MQTT_STATUS_UNKNOWN
*
* Sample:
* AT+MQTT_STATUS
*
* Retrun: None
********************************************************************/
FLASH_FUNC void  AT_MqttStatus(char* pLine);

#endif /* APP_MQTT_AT_CMD_H_ */
