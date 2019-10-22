/**************************************************************************************************************
* altobeam iot at cmd file 
*
* Copyright (c) 2018, altobeam.inc   All rights reserved.
* Permission is hereby granted for use on Altobeam IOT 6421 only,
* in which case ,it is free to  redistribute it and/or modify
*
*****************************************************************************************************************/

#ifdef CONFIG_MQTT_AT_CMD
#include "hal.h"
#include "lwip/init.h"
#include "lwip/dhcp.h"
#include "lwip/tcpip.h"
#include "lwip/inet.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "lwip_apps/sntp/sntp.h"

#include "app_at_cmd.h"

 
#define NOT_USED(X) (*(volatile typeof(X)*)&(X) = (X))

#include "app_mqtt_at_cmd.h"





static iotx_mqtt_param_t mqtt_params;
ATBM_MQTT_CONTEXT  at_mqtt_client_cxt;

#define MQTT_YIELD_TIME 500

#ifdef ATBM_MQTT_WITH_TLS
#define CONFIG_MQTT_APP_STK_SIZE 1024*8 //MQTT+TLS
#else
#define CONFIG_MQTT_APP_STK_SIZE 1024*4   
#endif

/*iot.eclipse.org,8883 mqtt test server certificate */
FLASH_RDATA const char *iotx_ca_crt_iot_eclipse = \
{
	\

		"-----BEGIN CERTIFICATE-----\r\n"
		"MIIFjTCCA3WgAwIBAgIRANOxciY0IzLc9AUoUSrsnGowDQYJKoZIhvcNAQELBQAw\r\n"  \
		"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\r\n"  \
		"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTYxMDA2MTU0MzU1\r\n"  \
		"WhcNMjExMDA2MTU0MzU1WjBKMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNTGV0J3Mg\r\n"  \
		"RW5jcnlwdDEjMCEGA1UEAxMaTGV0J3MgRW5jcnlwdCBBdXRob3JpdHkgWDMwggEi\r\n"  \
		"MA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCc0wzwWuUuR7dyXTeDs2hjMOrX\r\n"  \
		"NSYZJeG9vjXxcJIvt7hLQQWrqZ41CFjssSrEaIcLo+N15Obzp2JxunmBYB/XkZqf\r\n"  \
		"89B4Z3HIaQ6Vkc/+5pnpYDxIzH7KTXcSJJ1HG1rrueweNwAcnKx7pwXqzkrrvUHl\r\n"  \
		"Npi5y/1tPJZo3yMqQpAMhnRnyH+lmrhSYRQTP2XpgofL2/oOVvaGifOFP5eGr7Dc\r\n"  \
		"Gu9rDZUWfcQroGWymQQ2dYBrrErzG5BJeC+ilk8qICUpBMZ0wNAxzY8xOJUWuqgz\r\n"  \
		"uEPxsR/DMH+ieTETPS02+OP88jNquTkxxa/EjQ0dZBYzqvqEKbbUC8DYfcOTAgMB\r\n"  \
		"AAGjggFnMIIBYzAOBgNVHQ8BAf8EBAMCAYYwEgYDVR0TAQH/BAgwBgEB/wIBADBU\r\n"  \
		"BgNVHSAETTBLMAgGBmeBDAECATA/BgsrBgEEAYLfEwEBATAwMC4GCCsGAQUFBwIB\r\n"  \
		"FiJodHRwOi8vY3BzLnJvb3QteDEubGV0c2VuY3J5cHQub3JnMB0GA1UdDgQWBBSo\r\n"  \
		"SmpjBH3duubRObemRWXv86jsoTAzBgNVHR8ELDAqMCigJqAkhiJodHRwOi8vY3Js\r\n"  \
		"LnJvb3QteDEubGV0c2VuY3J5cHQub3JnMHIGCCsGAQUFBwEBBGYwZDAwBggrBgEF\r\n"  \
		"BQcwAYYkaHR0cDovL29jc3Aucm9vdC14MS5sZXRzZW5jcnlwdC5vcmcvMDAGCCsG\r\n"  \
		"AQUFBzAChiRodHRwOi8vY2VydC5yb290LXgxLmxldHNlbmNyeXB0Lm9yZy8wHwYD\r\n"  \
		"VR0jBBgwFoAUebRZ5nu25eQBc4AIiMgaWPbpm24wDQYJKoZIhvcNAQELBQADggIB\r\n"  \
		"ABnPdSA0LTqmRf/Q1eaM2jLonG4bQdEnqOJQ8nCqxOeTRrToEKtwT++36gTSlBGx\r\n"  \
		"A/5dut82jJQ2jxN8RI8L9QFXrWi4xXnA2EqA10yjHiR6H9cj6MFiOnb5In1eWsRM\r\n"  \
		"UM2v3e9tNsCAgBukPHAg1lQh07rvFKm/Bz9BCjaxorALINUfZ9DD64j2igLIxle2\r\n"  \
		"DPxW8dI/F2loHMjXZjqG8RkqZUdoxtID5+90FgsGIfkMpqgRS05f4zPbCEHqCXl1\r\n"  \
		"eO5HyELTgcVlLXXQDgAWnRzut1hFJeczY1tjQQno6f6s+nMydLN26WuU4s3UYvOu\r\n"  \
		"OsUxRlJu7TSRHqDC3lSE5XggVkzdaPkuKGQbGpny+01/47hfXXNB7HntWNZ6N2Vw\r\n"  \
		"p7G6OfY+YQrZwIaQmhrIqJZuigsrbe3W+gdn5ykE9+Ky0VgVUsfxo52mwFYs1JKY\r\n"  \
		"2PGDuWx8M6DlS6qQkvHaRUo0FMd8TsSlbF0/v965qGFKhSDeQoMpYnwcmQilRh/0\r\n"  \
		"ayLThlHLN81gSkJjVrPI0Y8xCVPB4twb1PFUd2fPM3sA1tJ83sZ5v8vgFv2yofKR\r\n"  \
		"PB0t6JzUA81mSqM3kxl5e+IZwhYAyO0OTg3/fs8HqGTNKd9BqoUwSRBzp06JMg5b\r\n"  \
		"rUCGwbCUDI0mxadJ3Bz4WxR6fyNpBK2yAinWEsikxqEt\r\n"  \
		"-----END CERTIFICATE-----"
};

/*get iot.eclipse.org server cerficate */
const char *iotx_ca_get_iot_eclipse(void)
{
	return iotx_ca_crt_iot_eclipse;

}

FLASH_FUNC void mqtt_event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
	uintptr_t packet_id = (uintptr_t)msg->msg;
	iotx_mqtt_topic_info_pt topic_info = (iotx_mqtt_topic_info_pt)msg->msg;
	NOT_USED(pclient);
	NOT_USED(pcontext);


	switch (msg->event_type) {
	case IOTX_MQTT_EVENT_UNDEF:
		EXAMPLE_TRACE("undefined event occur.");
		break;

	case IOTX_MQTT_EVENT_DISCONNECT:
		EXAMPLE_TRACE("MQTT disconnect.");
		break;

	case IOTX_MQTT_EVENT_RECONNECT:
		EXAMPLE_TRACE("MQTT reconnect.");
		break;

	case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
		EXAMPLE_TRACE("subscribe success, packet-id=%u", (unsigned int)packet_id);
		break;

	case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
		EXAMPLE_TRACE("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
		break;

	case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
		EXAMPLE_TRACE("subscribe nack, packet-id=%u", (unsigned int)packet_id);
		break;

	case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:
		EXAMPLE_TRACE("unsubscribe success, packet-id=%u", (unsigned int)packet_id);
		break;

	case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
		EXAMPLE_TRACE("unsubscribe timeout, packet-id=%u", (unsigned int)packet_id);
		break;

	case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
		EXAMPLE_TRACE("unsubscribe nack, packet-id=%u", (unsigned int)packet_id);
		break;

	case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:
		EXAMPLE_TRACE("publish success, packet-id=%u", (unsigned int)packet_id);
		break;

	case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:
		EXAMPLE_TRACE("publish timeout, packet-id=%u", (unsigned int)packet_id);
		break;

	case IOTX_MQTT_EVENT_PUBLISH_NACK:
		EXAMPLE_TRACE("publish nack, packet-id=%u", (unsigned int)packet_id);
		break;

	case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
		EXAMPLE_TRACE("topic message arrived but without any related handle: topic=%.*s, topic_msg=%.*s",
			topic_info->topic_len,
			topic_info->ptopic,
			topic_info->payload_len,
			topic_info->payload);
		break;

	case IOTX_MQTT_EVENT_BUFFER_OVERFLOW:
		EXAMPLE_TRACE("buffer overflow, %s", msg->msg);
		break;

	default:
		EXAMPLE_TRACE("Should NOT arrive here.");
		break;
	}
}

FLASH_FUNC static void mqtt_message_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
	iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt) msg->msg;
	char mqtt_msg_dump[64] = {0};  /*only dump first 64 bytes here!*/
	NOT_USED(pclient);
	NOT_USED(pcontext);

	if(ptopic_info->ptopic!=NULL)
	{
		if(ptopic_info->topic_len < 64)
		{
			memcpy(mqtt_msg_dump,(char*)ptopic_info->ptopic, ptopic_info->topic_len);   
		}
		else
		{
			memcpy(mqtt_msg_dump,(char*)ptopic_info->ptopic, 64);  
		}
	}   
	iot_printf("Topic=%s (Length: %d)\n", mqtt_msg_dump, ptopic_info->topic_len);

	memset(mqtt_msg_dump, '\0', sizeof(mqtt_msg_dump));	
	if(ptopic_info->payload!=NULL)
	{
		if(ptopic_info->payload_len < 64)
		{
			memcpy(mqtt_msg_dump,(char*)ptopic_info->payload, ptopic_info->payload_len);   	
		}
		else
		{
			memcpy(mqtt_msg_dump,(char*)ptopic_info->payload, 64);   	
		}
	}   	
	iot_printf("Payload: %s (Length: %d)\n", mqtt_msg_dump, ptopic_info->payload_len);
	
	memset(mqtt_msg_dump, '\0', sizeof(mqtt_msg_dump));		
	sprintf(mqtt_msg_dump,"+TOPIC_LEN=%02d,PAYLOAD_LEN=%03d:",ptopic_info->topic_len, ptopic_info->payload_len);
	UART_TSPT_Sendbuff(mqtt_msg_dump, strlen(mqtt_msg_dump));
	UART_TSPT_Sendbuff((char*)ptopic_info->ptopic, ptopic_info->topic_len);
	UART_TSPT_Sendbuff((char*)ptopic_info->payload, ptopic_info->payload_len);
}

void mqtt_send_callback(int send_status)
{
	if(send_status == 0)
	{
		UART_TSPT_SendStr("+Send OK\r\n");
	}else
	{
		UART_TSPT_SendStr("+Send ERR\r\n");
	}
}

int  mqtt_connect()
{

	int rc = 0;
	iotx_conn_info_pt pconn_info;

	if (NULL == (at_mqtt_client_cxt.msg_buf = (char *)HAL_Malloc(MQTT_MSGLEN))) {
		EXAMPLE_TRACE("not enough memory");
		rc = -1;
		return rc;
	}

	if (NULL == (at_mqtt_client_cxt.msg_readbuf = (char *)HAL_Malloc(MQTT_MSGLEN))) {
		EXAMPLE_TRACE("not enough memory");
		rc = -1;
		return rc;
	}

	/* Aliyun Device AUTH 
	Aliyun iot device get clientid, username, pwd from auth sever with 3 input key info:
	PRODUCT_KEY  	
	DEVICE_NAME
	DEVICE_SECRET.
	*/
	if(at_mqtt_client_cxt.mqtt_client_info.mqtt_sever_id == ATBM_MQTT_ALIYUN)
	{
		if(	at_mqtt_client_cxt.mqtt_client_info.linkType == ATBM_MQTT_TLS_GUIDER)   //TLS used
		{
#ifdef ATBM_MQTT_WITH_TLS
			if (0 != IOT_SetupConnInfoTLS(at_mqtt_client_cxt.mqtt_client_info.u.mqtt_aliyun_user.product_key, \
				at_mqtt_client_cxt.mqtt_client_info.u.mqtt_aliyun_user.device_name,\
				at_mqtt_client_cxt.mqtt_client_info.u.mqtt_aliyun_user.device_secret, (void **)&pconn_info)) {
					EXAMPLE_TRACE("AUTH request failed!");
					rc = -1;
					return rc;
			}
#endif	 //ATBM_MQTT_WITH_TLS
		}
		else  //TCP direct connected.
		{		
			if (0 != IOT_SetupConnInfo(at_mqtt_client_cxt.mqtt_client_info.u.mqtt_aliyun_user.product_key, \
				at_mqtt_client_cxt.mqtt_client_info.u.mqtt_aliyun_user.device_name,\
				at_mqtt_client_cxt.mqtt_client_info.u.mqtt_aliyun_user.device_secret, (void **)&pconn_info)) {
					EXAMPLE_TRACE("AUTH request failed!");
					rc = -1;
					return rc;
			}
		}
	}


	/* Initialize MQTT parameter */
	memset(&mqtt_params, 0x0, sizeof(mqtt_params));

	if(at_mqtt_client_cxt.mqtt_client_info.mqtt_sever_id == ATBM_MQTT_ALIYUN)
	{
		mqtt_params.port = pconn_info->port;
		mqtt_params.host = pconn_info->host_name;
		mqtt_params.client_id = pconn_info->client_id;
		mqtt_params.username = pconn_info->username;
		mqtt_params.password = pconn_info->password;
		mqtt_params.pub_key = pconn_info->pub_key;
	}
	else if(at_mqtt_client_cxt.mqtt_client_info.mqtt_sever_id == ATBM_MQTT_TEST)
	{

		mqtt_params.port = at_mqtt_client_cxt.mqtt_client_info.port;
		mqtt_params.host       = at_mqtt_client_cxt.mqtt_client_info.host;	
		mqtt_params.client_id  = at_mqtt_client_cxt.mqtt_client_info.u.mqtt_user.client_id;
		mqtt_params.username   = at_mqtt_client_cxt.mqtt_client_info.u.mqtt_user.user_name;
		mqtt_params.password   = at_mqtt_client_cxt.mqtt_client_info.u.mqtt_user.password;

		if(	at_mqtt_client_cxt.mqtt_client_info.linkType == ATBM_MQTT_TLS_GUIDER)
		{		  
			mqtt_params.pub_key = iotx_ca_get_iot_eclipse();
		}
	}
	else
	{

	}
	mqtt_params.request_timeout_ms = 2000;
	mqtt_params.clean_session = 0;
	mqtt_params.keepalive_interval_ms = 60000;
	mqtt_params.pread_buf = at_mqtt_client_cxt.msg_readbuf;
	mqtt_params.read_buf_size = MQTT_MSGLEN;
	mqtt_params.pwrite_buf =at_mqtt_client_cxt.msg_buf;
	mqtt_params.write_buf_size = MQTT_MSGLEN;
	mqtt_params.handle_event.h_fp = mqtt_event_handle;
	mqtt_params.handle_event.pcontext = NULL;

	iot_printf("mqtt_connect %d %s\n ", mqtt_params.port,mqtt_params.host);


	/* Construct a MQTT client with specify parameter */
	at_mqtt_client_cxt.at_mqtt_client = IOT_MQTT_Construct(&mqtt_params);
	if (NULL == at_mqtt_client_cxt.at_mqtt_client) {
		EXAMPLE_TRACE("MQTT construct failed");
		rc = -1;	
	}else
	{
		at_mqtt_client_cxt.mqtt_status = ATBM_MQTT_CONNECTED;	
	}	
	return rc;
}


FLASH_FUNC void  mqtt_at_process_main(void *param)
{
	//struct ip_addr netmask, gw, ip_addr;
	int ret = TRUE;
	NOT_USED(param);
	//TaskDebug_Put();

	//wifi_printk(WIFI_ATCMD,"mqtt_at_process_main++++++++++\n");
	ret = atbmwifi_wait_connect(g_wifinetif[0]);
	if(ret == TRUE){
		hal_sleep(200);
	}
	ret = mqtt_connect();
	if(ret != 0)
	{
		wifi_printk(WIFI_ATCMD,"mqtt_connect error\n");
		UART_Send_status(1);
		return;
	}
	else
	{
		UART_Send_status(0);
		wifi_printk(WIFI_ATCMD,"mqtt_connect loop\n");
	}
	do {
		/* handle the MQTT packet received from TCP or SSL connection */
		//wifi_printk(WIFI_ATCMD,"mqtt_at_process_main yield!1\n");
		IOT_MQTT_Yield(at_mqtt_client_cxt.at_mqtt_client, MQTT_YIELD_TIME);

		//wifi_printk(WIFI_ATCMD,"mqtt_at_process_main yield!2\n");

	} while (at_mqtt_client_cxt.mqtt_status == ATBM_MQTT_CONNECTED);

	wifi_printk(WIFI_ATCMD,"mqtt_at_process_main quit!\n");
}


FLASH_FUNC void mqtt_at_proc_thread(void)
{
	if((at_mqtt_client_cxt.mqtt_status ==  ATBM_MQTT_CONNECTED )||(at_mqtt_client_cxt.mqtt_status ==  ATBM_MQTT_DISCONNECTING))
	{
		iot_printf("mqtt_at_proc_thread  STATUS:%d\n",at_mqtt_client_cxt.mqtt_status);
		UART_Send_status(1);
		return;
	}else
	{
		iot_printf("mqtt_at_main++ %d\n",CONFIG_MQTT_APP_STK_SIZE);
		sys_thread_new("mqtt_at_proc", mqtt_at_process_main, NULL,CONFIG_MQTT_APP_STK_SIZE,CONFIG_TCPAPP_THREAD_PRIORITY);
	}
}


FLASH_FUNC void mqtt_publish_send(char *topic, char *pmsg,unsigned char u8qos)
{
	int rc = 0;
	iotx_mqtt_topic_info_t topic_msg;

	iot_printf("mqtt_publish_send:%s,%s\n",topic, pmsg);


	if(at_mqtt_client_cxt.at_mqtt_client == NULL)
	{
		UART_TSPT_SendStr("MQTT is not established!\r\n");
		return;
	}else
	{

		/* Initialize topic information */
		memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));
		//strcpy(msg_pub, "message: hello! start!");

		topic_msg.qos = u8qos;
		topic_msg.retain = 0;
		topic_msg.dup = 0;
		topic_msg.payload = (void *)pmsg;
		topic_msg.payload_len = strlen(pmsg);

		rc = IOT_MQTT_Publish(at_mqtt_client_cxt.at_mqtt_client, topic, &topic_msg);
	}

	if(rc  < 0 )
	{
		iot_printf("error occur when publish:%d,%d\n");
		at_mqtt_client_cxt.send_callback(rc);

	}
	else
	{	
		at_mqtt_client_cxt.send_callback(0);	
	}			

}


/*
Format:
AT+MQTT_CLIENT_INFO  CLIENT_ID=abcdxxxx, USER_NAME=abcdxxx, PWD=12345678xx
sample:
AT+MQTT_CLIENT_INFO  CLIENT_ID=abcdxxxx, USER_NAME=abcdxxx, PWD=12345678xx
*/
FLASH_FUNC void  AT_MqttClientInfo(char* pLine)
{	
	char * str;

	memset(&at_mqtt_client_cxt, 0x0, sizeof(at_mqtt_client_cxt));

	pLine = CmdLine_SkipSpace(pLine);
	if (!memcmp("CLIENT_ID",pLine,strlen("CLIENT_ID")))
	{
		str = CmdLine_GetToken(&pLine);
		str = CmdLine_GetToken(&pLine);
		wifi_printk(WIFI_ATCMD,"CLIENT_ID is %s\n",str);			
		memcpy(at_mqtt_client_cxt.mqtt_client_info.u.mqtt_user.client_id, str, strlen(str));
	}
	pLine = CmdLine_SkipSpace(pLine);
	if (!memcmp("USER_NAME",pLine,strlen("USER_NAME")))
	{
		str = CmdLine_GetToken(&pLine);
		str = CmdLine_GetToken(&pLine);
		wifi_printk(WIFI_ATCMD,"USER_NAME %s\n",str);			
		memcpy(at_mqtt_client_cxt.mqtt_client_info.u.mqtt_user.user_name, str, strlen(str));
	}
	pLine = CmdLine_SkipSpace(pLine);
	if (!memcmp("PWD",pLine,strlen("PWD")))
	{
		str = CmdLine_GetToken(&pLine);
		str = CmdLine_GetToken(&pLine);
		wifi_printk(WIFI_ATCMD,"PWD %s\n",str);			
		memcpy(at_mqtt_client_cxt.mqtt_client_info.u.mqtt_user.password, str, strlen(str));
	}
	
	
	at_mqtt_client_cxt.mqtt_client_info.mqtt_sever_id = ATBM_MQTT_TEST;
	UART_Send_status(0);

}


/*
Format:
AT+MQTT_CLIENT_INFO_ALI  PRODUCT_KEY=product_keyxx, DEVICE_NAME=device_namexx, DEVICE_SECRET=device_secretxx
sample:
AT+MQTT_CLIENT_INFO_ALI  PRODUCT_KEY=a16INy8YJxw, DEVICE_NAME=atbm6421_test0_1, DEVICE_SECRET=RWhL1AUgXe18QyN07EdcmGIgcUaIfAqE
*/
FLASH_FUNC void  AT_MqttClientInfoAli(char* pLine)
{
	char * str;

	memset(&at_mqtt_client_cxt, 0x0, sizeof(at_mqtt_client_cxt));

	pLine = CmdLine_SkipSpace(pLine);
	if (!memcmp("PRODUCT_KEY",pLine,strlen("PRODUCT_KEY")))
	{
		str = CmdLine_GetToken(&pLine);
		str = CmdLine_GetToken(&pLine);
		wifi_printk(WIFI_ATCMD,"PRODUCT_KEY %s\n",str);			
		memcpy(at_mqtt_client_cxt.mqtt_client_info.u.mqtt_aliyun_user.product_key, str, strlen(str));
	}
	pLine = CmdLine_SkipSpace(pLine);
	if (!memcmp("DEVICE_NAME",pLine,strlen("DEVICE_NAME")))
	{
		str = CmdLine_GetToken(&pLine);
		str = CmdLine_GetToken(&pLine);
		wifi_printk(WIFI_ATCMD,"DEVICE_NAME %s\n",str);			
		memcpy(at_mqtt_client_cxt.mqtt_client_info.u.mqtt_aliyun_user.device_name, str, strlen(str));
	}
	pLine = CmdLine_SkipSpace(pLine);
	if (!memcmp("DEVICE_SECRET",pLine,strlen("DEVICE_SECRET")))
	{
		str = CmdLine_GetToken(&pLine);
		str = CmdLine_GetToken(&pLine);
		wifi_printk(WIFI_ATCMD,"PWD %s\n",str); 		
		memcpy(at_mqtt_client_cxt.mqtt_client_info.u.mqtt_aliyun_user.device_secret, str, strlen(str));
	}
	
	at_mqtt_client_cxt.mqtt_client_info.mqtt_sever_id = ATBM_MQTT_ALIYUN;
	UART_Send_status(0);

}



/*
* AT+MQTT_CONNECT=  <type>,<remote host>,<remote port>
* Description: 
* Connect with MQTT sever with TLS or without TLS.
* Input: 	AT command line string, will parsing the follwoing field.   
* Parameters：
*       <type>   : connect type "TCP", "TLS"
*       <host>   : mqtt host name or IP address
*       <remote port> mqtt port
*Response:OK
*      	   ERROR
*       	   ALREADY CONNECTED
Sample:
1, Aliyun direct no TLS
AT+MQTT_CONNECT=TCP, iot-as-mqtt.cn-shanghai.aliyuncs.com,1883
1, Aliyun direct with TLS 
AT+MQTT_CONNECT=TCP_TLS, iot-as-mqtt.cn-shanghai.aliyuncs.com,1883
2, Aliyun with TLS
AT+MQTT_CONNECT=TLS, iot-as-mqtt.cn-shanghai.aliyuncs.com,1883

3, MQTT test no TLS
AT+MQTT_CONNECT=TCP, iot.eclipse.org,1883
4, MQTT test with TLS
AT+MQTT_CONNECT=TLS, iot.eclipse.org,8883
*/
FLASH_FUNC void AT_MqttConnect(char* pLine)
{
	char *LinkType;
	char *HostName;
	u8_t len;
	ATBM_MQTT_LINK_TYPE linkType = ATBM_MQTT_INVALID;
	char ipTemp[128];
	u32_t re_port;
	u32_t ip = 0;  
	int ret;
	IOT_SetLogLevel(IOT_LOG_DEBUG);

	//wait untill WIFI connected on.
	ret = atbmwifi_wait_connect();
	if(ret== FALSE)
	{
		UART_Send_status(HAL_ERR_NOT_PRESENT);
		return;
	}
	//wait untill ip address leased from DHCP server.
	atbm_wifi_wait_get_ipaddr_ok();

	LinkType  = CmdLine_GetToken(&pLine);         //获取连接类型
	HostName = CmdLine_GetToken(&pLine);         //获取IP地址
	CmdLine_GetInteger(&pLine,&re_port);         //获取远端端口号
	len = strlen(HostName);
	if(len > (sizeof(ipTemp)-1))
	{
		len= (sizeof(ipTemp)-1);
	}
	memcpy(ipTemp,HostName,len);
	ipTemp[len] = 0;

	if(memcmp("TCP_TLS",LinkType,strlen("TCP_TLS")) == 0)
	{
		linkType = ATBM_MQTT_TLS_DIRECT;
		guider_set_secure_mode(MODE_TLS_DIRECT);
	}
	else if(memcmp("TCP",LinkType,strlen("TCP")) ==0)
	{
		linkType = ATBM_MQTT_DIRECT;
		guider_set_secure_mode(MODE_TCP_DIRECT_PLAIN);
	}	 
	else if(memcmp("TLS",LinkType,strlen("TLS")) == 0)
	{
		linkType = ATBM_MQTT_TLS_GUIDER;
		guider_set_secure_mode(MODE_TLS_GUIDER);
	}
	else
	{
		//UART_TSPT_SendStr("+ERR already connected\r\n");;
		UART_Send_status(HAL_ERR_NOT_PRESENT);
		return;
	}

	if(at_mqtt_client_cxt.mqtt_status == ATBM_MQTT_CONNECTED)
	{
		//UART_TSPT_SendStr("+ERR already connected\r\n");
		UART_Send_status(1);
		return;
	}

	memcpy(at_mqtt_client_cxt.mqtt_client_info.host, ipTemp, strlen(ipTemp));
	ip = inet_addr(ipTemp);
	iot_printf("ip = %x  str:%s\n",ip, ipTemp);	
	at_mqtt_client_cxt.mqtt_client_info.linkType = linkType;
	at_mqtt_client_cxt.send_callback = mqtt_send_callback;

	//may be different, actually for ALIYun  when TLS auth used, 
	//the port and host name of MQTT server  is not set here.
	switch(linkType)
	{
	case ATBM_MQTT_DIRECT:
	case ATBM_MQTT_TLS_DIRECT:
	case ATBM_MQTT_TLS_GUIDER:
		if((ip == 0xffffffff) && (os_memcmp(ipTemp,"255.255.255.255",16) != 0))
		{
			get_host_by_name(ipTemp, &at_mqtt_client_cxt.mqtt_client_info.ip);
		}
		else
		{
			at_mqtt_client_cxt.mqtt_client_info.ip = ip;
		}

		at_mqtt_client_cxt.mqtt_client_info.port = re_port;		
		//iot_printf("mqtt_connect direct %d %d\n", re_port, at_mqtt_client_cxt.mqtt_client_info.ip);
		mqtt_at_proc_thread();
		break;

	default:
		break;
	}

}



/*
sample:
AT+MQTT_PUBLISH=  /a16INy8YJxw/atbm6421_test0_1/data,0, 12
hello world!
*/
FLASH_FUNC void  AT_MqttPublish(char* pLine)
{
	u8_t u8Qos = 0;
	u32_t u32Tmp = 0;	
	char *pTopic;
	u16_t u16msg_len = 0;
	char *pMsg;


	if(at_mqtt_client_cxt.mqtt_status != ATBM_MQTT_CONNECTED)
	{
		UART_TSPT_SendStr("MQTT is not connected!\r\n");
		return;
	}

	if(at_mqtt_client_cxt.at_mqtt_client == NULL)
	{
		UART_TSPT_SendStr("MQTT is not established!\r\n");
		return;
	}
	pLine = CmdLine_SkipSpace(pLine);	
	pTopic = CmdLine_GetToken(&pLine);	 

	pLine = CmdLine_SkipSpace(pLine);
	CmdLine_GetInteger(&pLine, &u32Tmp);	
	u8Qos = (u8_t)u32Tmp;		

	pLine = CmdLine_SkipSpace(pLine);
	CmdLine_GetInteger(&pLine, &u32Tmp);
	u16msg_len = (u16_t)u32Tmp;	

	if(u8Qos > 2)
	{
		UART_TSPT_SendStr("qos error\r\n");
		return;
	}		
	wifi_printk(WIFI_ATCMD,"Topic: %s\n",pTopic); 

	UART_TSPT_SendStr("> ");
	while(UART_TSPT_NetData_GetLine() == NULL)
	{
		mdelay(20);
	}
	if(strlen(UART_TSPT_RxStr) > u16msg_len)
	{
		UART_TSPT_RxStr[u16msg_len]='\0';  //force msg length as input.
	}
	else
	{

	}
	pMsg = UART_TSPT_RxStr;
	wifi_printk(WIFI_ATCMD,"msg: %s\n",pMsg); 
	mqtt_publish_send(pTopic, pMsg, u8Qos);

}

/*
Sample:
1,Aliyun sample:
AT+MQTT_SUBSCRIBE=/a16INy8YJxw/atbm6421_test0_1/data, 0

2, Mqtt test sample:
AT+MQTT_SUBSCRIBE=   a16INy8YJxw/atbm6421_test0_1/data, 0
*/
FLASH_FUNC void  AT_MqttSubscribe(char* pLine)
{
	int rc = 0;
	u8_t u8Qos = 0;
	u32_t u32Qos = 0;

	char *pTopic;


	pTopic = CmdLine_GetToken(&pLine);
	CmdLine_GetInteger(&pLine, &u32Qos);
	u8Qos = (u8_t)u32Qos;	


	/* Subscribe the specific topic */
	rc = IOT_MQTT_Subscribe(at_mqtt_client_cxt.at_mqtt_client, pTopic, u8Qos, mqtt_message_arrive, NULL);
	if (rc < 0) {
		IOT_MQTT_Destroy(&at_mqtt_client_cxt.at_mqtt_client);
		EXAMPLE_TRACE("IOT_MQTT_Subscribe() failed, rc = %d", rc);
		rc = -1;
		UART_Send_status(rc);
	}else{
		UART_Send_status(0);	
	}


}


/*
Sample:
1,Aliyun sample:
AT+MQTT_UNSUBSCRIBE=/a16INy8YJxw/atbm6421_test0_1/data

2, Mqtt test sample:
AT+MQTT_UNSUBSCRIBE= a16INy8YJxw/atbm6421_test0_1/data
*/
FLASH_FUNC void  AT_MqttUnSubscribe(char* pLine)
{
	char *pTopic;
	int ret = 0;

	pTopic = CmdLine_GetToken(&pLine);
	if(pTopic!= NULL)
	{
		ret = IOT_MQTT_Unsubscribe(at_mqtt_client_cxt.at_mqtt_client, pTopic);
		if(ret <0)
		{
			UART_Send_status(-1);
		}else
		{
			UART_Send_status(0);
		}		
	}else
	{
		ret = -1;
		UART_Send_status(ret);
	}
	wifi_printk(WIFI_ATCMD,"AT_MqttUnSubscribe ret: %d topic:%s\n",ret, pTopic);
}


/*
There is parameters for this command.
AT+MQTT_CLOSE
*/
FLASH_FUNC void  AT_MqttClose(char* pLine)
{

	UN_USED_PARAMS(*pLine);
	if(at_mqtt_client_cxt.mqtt_status != ATBM_MQTT_CONNECTED)
	{
		UART_Send_status(1);
	}
	at_mqtt_client_cxt.mqtt_status = ATBM_MQTT_DISCONNECTING;
	sleep(MQTT_YIELD_TIME);
	//unSubscribe all topics
	//to be done.  should unSubscribe from MCU.
	IOT_MQTT_Destroy(&at_mqtt_client_cxt.at_mqtt_client);

	if (NULL != at_mqtt_client_cxt.msg_buf) {
		HAL_Free(at_mqtt_client_cxt.msg_buf);
		at_mqtt_client_cxt.msg_buf = NULL;
	}

	if (NULL != at_mqtt_client_cxt.msg_readbuf) {
		HAL_Free(at_mqtt_client_cxt.msg_readbuf);
		at_mqtt_client_cxt.msg_readbuf = NULL;
	}
	at_mqtt_client_cxt.mqtt_status = ATBM_MQTT_DISCONNECTED;
	UART_Send_status(0);
}

/*
There is parameters for this command.
AT+MQTT_STATUS
*/
FLASH_FUNC void  AT_MqttStatus(char* pLine)
{

	UN_USED_PARAMS(*pLine);
	if(at_mqtt_client_cxt.mqtt_status == ATBM_MQTT_CONNECTED)
	{
		UART_TSPT_SendStr("+MQTT_CONNECTED\r\n");
	}
	else if(at_mqtt_client_cxt.mqtt_status == ATBM_MQTT_DISCONNECTED)
	{
		UART_TSPT_SendStr("+MQTT_DISCONNECTED\r\n");
	}
	else if(at_mqtt_client_cxt.mqtt_status == ATBM_MQTT_DISCONNECTING)
	{
		UART_TSPT_SendStr("+MQTT_DISCONNECTING\r\n");
	}else if(at_mqtt_client_cxt.mqtt_status == ATBM_MQTT_STATUS_UNINIT)
	{
		UART_TSPT_SendStr("+MQTT_STATUS_UNINIT\r\n");
	}else
	{
		UART_TSPT_SendStr("+MQTT_STATUS_UNKNOWN\r\n");
	}

}

struct cli_cmd_struct ATCommands_mqtt[] =
{

	{.cmd ="AT+MQTT_CLIENT_INFO",	   .fn = AT_MqttClientInfo,		.next = (void*)0},	
	{.cmd ="AT+MQTT_CLIENT_INFO_ALI",  .fn = AT_MqttClientInfoAli,  .next = (void*)0},	
	{.cmd ="AT+MQTT_CONNECT",		   .fn = AT_MqttConnect,	    .next = (void*)0},	
	{.cmd ="AT+MQTT_PUBLISH",		   .fn = AT_MqttPublish,		.next = (void*)0},
	{.cmd ="AT+MQTT_SUBSCRIBE",		   .fn = AT_MqttSubscribe,		.next = (void*)0},
	{.cmd ="AT+MQTT_UNSUBSCRIBE",	   .fn = AT_MqttUnSubscribe,	.next = (void*)0},
	{.cmd ="AT+MQTT_CLOSE",		   	   .fn = AT_MqttClose,			.next = (void*)0},
	{.cmd ="AT+MQTT_STATUS",		   .fn = AT_MqttStatus,		    .next = (void*)0},

};

FLASH_FUNC void ATCmd_init_mqtt(void)
{
	cli_add_cmds(ATCommands_mqtt,sizeof(ATCommands_mqtt)/sizeof(ATCommands_mqtt[0]));
}
#endif  //CONFIG_ALI_YUN
