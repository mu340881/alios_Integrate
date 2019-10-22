/**************************************************************************************************************
 * altobeam iot at cmd file 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 * Permission is hereby granted for use on Altobeam IOT 6421 only,
 * in which case ,it is free to  redistribute it and/or modify
 *
*****************************************************************************************************************/

#include "app_AT_cmd.h"
#include "lwip/sockets.h"
#include "lwip/api.h"
#include "lwip/lwipopts.h"
#include "wifi_api.h"
#include "joylink_smnt.h"
#include "os_api.h"
#include "cmd_line.h"
#include "atbmconn.h"
#include "atbm_mbedtls_app.h"
#include "app_net_AT_cmd.h"


#define NET_SERVER_RCV_SIZE 1600

static AT_LinkConType pLink[AT_LinkMax];
static BOOL SOCKETMUX_flag = FALSE;
uint32 servermaxconn = AT_LinkMax;
BOOL IPMODE = FALSE;
static struct atbmconn *pTcpServer;
static mbedtls_cli_connect_t cli_connect = {
    .state = MBEDTLS_CLIENT_STATE_IDLE
};

static mbedtls_st session;
//static BOOL serverEn = FALSE;
/*smartconfig type: atbm smartconfig or joylink*/
extern uint8 smt_type;
extern joylink_smnt_priv_t joylink_priv;
extern char UART_TSPT_RxStr[TSPT_RX_BUF_SIZE];
extern u32_t Uart_TSPT_buffer_len;
extern mutex_t socket_mutex;
/*
 * == 1 Net_Sarial mode uart transparent mode
 * == 0 AT mode
*/
extern u32_t Uart_TSPT_Enable_flag;
extern FLASH_FUNC int get_host_by_name(char *hostname, uint32_t *addr);
extern int atbmwifi_wait_ipaddr_ok(/*struct netif * netdev*/); 
struct iot_ip_param ip_params_user_set;
extern void sample_http_ota_APP_Init(char * http_url);

FLASH_FUNC	int parse_ip_cmd(char *pLine)
{		
	char * str;	

	if (!memcmp("IP",pLine,2))
	{
		str = CmdLine_GetToken(&pLine);
		if (!memcmp("IP",str,2)) {			
			str = CmdLine_GetToken(&pLine);
			ip_params_user_set.static_ipaddr = inet_addr(str);				
		}

		str = CmdLine_GetToken(&pLine);
		if (!memcmp("NETMASK",str,7)) {
			str = CmdLine_GetToken(&pLine);
			ip_params_user_set.static_ipmask	= inet_addr(str);		
		}

		str = CmdLine_GetToken(&pLine);
		if (!memcmp("GW",str,2)) {
			str = CmdLine_GetToken(&pLine);
			ip_params_user_set.static_gwaddr = inet_addr(str);			
		}
	}	
	return 0;	

}


/* example: AT+ALIYUN_TEST  awss */
FLASH_FUNC void AT_Aliyun_test(char *pLine)
{
#ifndef ARDUINO
#ifdef CONFIG_ALI_YUN
	if(!strncmp(pLine, "ota", strlen("ota"))){
		iot_printf("----start ota sample----\n");
		at_mqtt_ota_main();
	}
	else if(!strncmp(pLine, "awss", strlen("awss"))){
		iot_printf("----start awss----\n");
		awss_test_main();
	}
	else if(!strncmp(pLine, "solo", strlen("solo"))){
		iot_printf("----start solo----\n");
		solo_example();
	}
	else{
		iot_printf("----invalid param----\n");
	}
#endif //CONFIG_ALI_YUN
#endif //#ifndef ARDUINO
}

/*
  function:  smartConfigLinkedNotify_sample
  params: none
  description:
                After smartconfig done! some APP listening broadcast msg from wifi device.
                This function will send smart config done msg with device's mac address! 
                after App received the done msg, quit from samrtconfig waiting.
                This function should be called after connected with AP and IP address set or leased.
*/
FLASH_FUNC int smartConfigLinkedNotify_sample(void)
{


    int sock_id = -1;
    unsigned char i =0;
    int ret = 0;
    char smart_msg[32] = {"smart_config"};
    char ch_mac_addr[32] = {0};
    u8 mac_addr[6] = {0};
    int mac_start_pos = 0;
    struct sockaddr_in addrto;
    if ((sock_id = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        iot_printf("SOCK_DGRAM socket error!\n");
        return false;
    }

    const int opt = 1;
    //设置该套接字为广播类型，
    int nb = 0;
    nb = setsockopt(sock_id, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
    if(nb == -1)
    {
        iot_printf("set socket error...!\n");
        close_socket(sock_id);
        return false;
    }

    bzero(&addrto, sizeof(struct sockaddr_in));
    addrto.sin_family = AF_INET;
    addrto.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    addrto.sin_port = htons(3778);
    int nlen = sizeof(addrto);
    mdelay(2000);
    iot_printf("send start.....!\n");

    mac_start_pos = strlen(smart_msg);
    smart_msg[mac_start_pos] =' ';
    mac_start_pos = mac_start_pos + 1;
    atbm_wifi_get_mac_addr(mac_addr);
    sprintf(ch_mac_addr, "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2],
                                                          mac_addr[3], mac_addr[4], mac_addr[5]);

    memcpy(&smart_msg[mac_start_pos], ch_mac_addr, strlen(ch_mac_addr));


    while(i < 60)
    {
        //从广播地址发送消息
        ret = sendto(sock_id, smart_msg, strlen(smart_msg), 0, (struct sockaddr*)&addrto, nlen);
        if(ret<0)
        {
            iot_printf("send error.....!\n");
            break;
        }
        else
        {
        	//iot_printf("ok\n");
        }
        i++;
        mdelay(10);
    }
    close_socket(sock_id);
    //iot_printf("send end.....!\n");
    return 0;
}


FLASH_FUNC void AT_TcpIpConfig(char *Args)
{
	UN_USED_PARAMS(*Args);
}

int g_rxbyte=0;
int g_rxcnt=0; 

FLASH_FUNC void AT_IperfStatus(char *Args)
{	
	int loop =0;
	int rxbyte=0;
	unsigned int timeout = 10;
	
	if(Args){	
	    CmdLine_GetInteger(&Args, &timeout);
	}

	if(timeout > 10000)
		timeout = 10000;

	
	for(loop=0;loop<timeout;loop++){
		rxbyte = g_rxbyte-rxbyte;
		iot_printf("g_rxcnt %d,rxbyte %d [%d]Kbps\n",g_rxcnt,rxbyte,(rxbyte*8/1000));
		rxbyte = g_rxbyte;
		mdelay(1000);
	}
	g_rxbyte = 0;
	g_rxcnt = 0;
}

FLASH_FUNC void AT_TcpIpStatus(char *Args)
{	
	UN_USED_PARAMS(*Args);
}


/* AT+iperfs    test TCP rx throughput */
FLASH_FUNC void AT_RXIperfTestfunc(char* pLine)
{
	UN_USED_PARAMS(*pLine);
	int  socket_id = -1, socket_server = -1, size, err;
	struct sockaddr_in serv;
	struct sockaddr addr;
	int port = 5001;
	u32 starttime = 0, rcv_byte_counter = 0, pre_couner = 0, timeindex = 0;
	unsigned char *net_rcv_buff = NULL;
	int iperf_err_check =0;
    int timeout;

	memset(&serv, 0, sizeof(struct sockaddr_in));	
	iot_printf("AT_RXIperfTestfunc++ %s\n",pLine);
	
	serv.sin_family = AF_INET;
	serv.sin_port = htons(port);	
	serv.sin_addr.s_addr = htons(INADDR_ANY); // htons(INADDR_ANY);

	socket_server = socket(AF_INET, SOCK_STREAM, 0);
	if((socket_server == -1)||( socket_server >= NUM_SOCKETS))
	{
		iot_printf("AT_RXIperfTestfunc err1,port %d sock %d\n",port,socket_server);
		return;
	}
	err = bind(socket_server, (struct sockaddr*)&serv, sizeof(struct sockaddr_in));
	if(err == -1)
	{
		iot_printf("AT_RXIperfTestfunc err2,port %d socket_server %d\n",port,socket_server);
		closesocket(socket_server);
		return;
	}
	iot_printf("AT_RXIperfTestfunc ,port %d socket_server %d\n",port,socket_server);

	err = listen(socket_server, 4);

		
	if(err == -1)
	{
		iot_printf("AT_RXIperfTestfunc err3,port %d\n",port);	
		closesocket(socket_server);
		return;
	}
	else
	{	
		size = sizeof(struct sockaddr);
		socket_id = accept(socket_server, &addr, (socklen_t*)&size);
		
		iot_printf("accept socket_id:%d\n", socket_id);
		if(socket_id == -1)
		{
			iot_printf("AT_RXIperfTestfunc accept err1\n");
			closesocket(socket_server);
			return;
		}
		
		net_rcv_buff = os_malloc(1600);
		if(net_rcv_buff == NULL)
		{
			iot_printf("os_malloc failed socket_id:%d\n", socket_id);
			closesocket(socket_id);
			closesocket(socket_server);
			return;
		}
	    // Set the socket IO timeout
	    timeout  = 10000;
		if( setsockopt(socket_id, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0 )
	    {
			closesocket(socket_id);
			closesocket(socket_server);
			return;
	    }
		starttime = jiffies;
		while(1)
		{
			//lwip_recv(int s, void *mem, size_t len, int flags)
			size = recv(socket_id, net_rcv_buff, 1600, 0/*MSG_DONTWAIT*/);
			
			//iot_printf("net_rcv_buff size:%d\n",size);
			if (size ==  - 1)
			{	
				p_err("tcp_recv fatal err:%d,%d\n", socket_id, errno);
				break;
			}			
			/*scoket connnection closed ....*/			
			else if (size == 0)
			{	
				if (errno != 0)
					break;
				iot_printf("size:%d close_socket socket_id:%d\n", size, socket_id);
				continue;
			}

			if(((s32)jiffies - (s32)starttime) > 1000)  //update status every 1 second.
			{
				iot_printf("Time:[%d-%d]second, Rx:[%d]Kbytes [%d]Kbps/sec %d\n",timeindex, (timeindex+1), (rcv_byte_counter - pre_couner), (rcv_byte_counter - pre_couner) *8/1000 ,iperf_err_check);
				starttime = jiffies;
				if((rcv_byte_counter - pre_couner) == 0)
					iperf_err_check++;
				else 
					iperf_err_check = 0;
				pre_couner = rcv_byte_counter;
				timeindex = timeindex + 1;
				
				if(iperf_err_check > 5){
					iot_printf("iperf_err_check:%d\n", iperf_err_check);
					break;
				}
			}

			rcv_byte_counter = rcv_byte_counter + size;	
		}
		kfree(net_rcv_buff);
		net_rcv_buff = NULL;
		closesocket(socket_id);
	}		
	closesocket(socket_server);
	iot_printf("iperfs close and quit!!\n");
}
/*
    sntp using tcp/ip socket, sys_thread_new is required, after time quiry succeed, a timer will work.
*/
#define CONFIG_IPERFS_STK_SIZE 2048
#define CONFIG_IPERFS_THREAD_PRIORITY CONFIG_TCPAPP_THREAD_PRIORITY

FLASH_FUNC void  iperfs_test_thread(void *param)
{			
	static bool b_iperfs_init =0; 

	if(b_iperfs_init != 0){
		iot_printf("iperfs have been running\n");
		return ;
	}
	
	b_iperfs_init = 1;
	AT_RXIperfTestfunc(param);
	b_iperfs_init = 0;
}

FLASH_FUNC void AT_RXIperfTest(char* pLine)
{	
   	sys_thread_new("iperfs_thread", iperfs_test_thread, pLine,CONFIG_IPERFS_STK_SIZE,CONFIG_IPERFS_THREAD_PRIORITY);
}


/*
添加了一个通过iperf -c xx.xx.xx.xx ，测试发送throughput 功能
*/
FLASH_FUNC void AT_TXIperfTest(char* pLine)
{
	int socket_s = -1;
	struct sockaddr_in sockaddr;
	char *send_buf = (char *)(0x2C00000);
	char *str;
	int length = 1400;
	int sendtime = 10*1000;/*ms*/
	u32 starttime = 0;

	
	str = CmdLine_GetToken(&pLine);	
	sockaddr.sin_addr.s_addr = inet_addr(str);
	
	iot_printf("lwip_socket %s\n",str);
	
	if((socket_s = lwip_socket(PF_INET, SOCK_STREAM, IP_PROTO_TCP)) < 0){ 
		iot_printf("unable to create socket for Iperf\r\n"); 
	    return; 
	}	
	
	/* connect to iperf server */
	sockaddr.sin_family      = AF_INET;
	//sockaddr.sin_addr.s_addr = inet_addr(ip); 
	sockaddr.sin_port        = htons(5001);
	//iot_printf("lwip_connect %x\n",sockaddr.sin_addr.s_addr);
	if(lwip_connect(socket_s, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
	{
		iot_printf("unable to connect to iperf server\n");
		goto done;
	}
	
	iot_printf("Connected to iperf server\n");
	starttime = jiffies;
	do {
		lwip_send(socket_s, send_buf, length, 0); 
		if(((s32)jiffies - (s32)starttime)>sendtime){
			break;
		}
	}while(1);
	
	iot_printf("iperf end\n");

done:
	lwip_close(socket_s);

}

/*
添加了一个通过tftp OTA升级code的功能，步骤如下:
// fw_update1.bin   为iccm.bin,注意需要将名称修改为fw_update1.bin  
1. AT+tftp 192.168.x.x fw_update1.bin  
// fw_update2.bin   为flash.bin，注意需要将名称修改为fw_update2.bin  
2. AT+tftp 192.168.x.x fw_update2.bin
// fw_update3.bin   为任意的长度小于512的bin文件，内容不关心，注意需要将名称修改为fw_update3.bin  ，
//主要是用来触发重启系统开始更新code的操作
2. AT+tftp 192.168.x.x fw_update3.bin
*/
FLASH_FUNC int Tftp_APP_Init(u32 ip,char *filename);
FLASH_FUNC void AT_Tftp(char* pLine)
{	
	char *str = CmdLine_GetToken(&pLine);	
	char *filename = CmdLine_GetToken(&pLine);	
	Tftp_APP_Init(inet_addr(str),filename);


}

/***********************************************************************************************/
/**************************************TCP/IP功能AT指令******************************************/
/***********************************************************************************************/


/**
  *  Client received callback function.
  * param  arg: contain the ip link information
  * param  pdata: received data
  * param  len: the lenght of received data
  */
FLASH_FUNC void at_tcpclient_recv_cb(void *arg, char *pdata, unsigned short len)
{
	  //iot_printf("at_tcpclient_recv_cb\n");
      char str[10] = "RECV OK\r\n";
	  struct atbmconn *patbmconn = (struct atbmconn *)arg;
	  AT_LinkConType *linkTemp = (AT_LinkConType *)patbmconn->reverse;
	  char temp[32];
	  if(SOCKETMUX_flag)
	  {
		  sprintf(temp, "LinkID %d,len %d:",linkTemp->linkId, len);
		  UART_TSPT_SendStr(temp);
		  UART_TSPT_Sendbuff(pdata,len);
	  }
	  else
	  {
		  //iot_printf("recv: \r\n");
		  //UART_TSPT_SendStr(pdata);
		  UART_TSPT_Sendbuff(pdata, len);
		  UART_TSPT_SendStr("\n");
	  }
	  //UART_TSPT_Sendbuff(str, 10);
	  UART_TSPT_SendStr("RECV OK\r\n");
}


/**
  *  Client send over callback function.
  * param  arg: contain the ip link information
  * retval None
  */

FLASH_FUNC void at_tcpclient_sent_cb(int sendok)
{
	if(sendok == 1)
	{
		UART_TSPT_SendStr("\r\nSEND OK\r\n");
	}
	else
	{
		UART_TSPT_SendStr("\r\nSEND FAIL\r\n");
	}
}
/**
  *  Tcp client disconnect success callback function.
  * param  arg: contain the ip link information
  * retval None
  */
FLASH_FUNC void at_tcpclient_discon_cb(void *arg,int needfree)
{
	  struct atbmconn *patbmconn = (struct atbmconn *)arg;
	  AT_LinkConType *linkTemp = (AT_LinkConType *)patbmconn->reverse;

	  if(patbmconn == NULL)
	  {
	    return;
	  }
	  if(needfree){
	  	os_free(patbmconn);
		linkTemp->pCon=NULL;
	  }
	  linkTemp->linkEn = FALSE;
	  if(SOCKETMUX_flag)
	  {
		  iot_printf("close LinkID %d\r\n",linkTemp->linkId);
	  }

	  UART_Send_status(0);
}

/**
  * brief  Tcp client connect repeat callback function.
  * param  arg: contain the ip link information
  * etval None
  */
FLASH_FUNC void at_tcpclient_recon_cb(void *arg, int err)
{
	UART_TSPT_SendStr("reconn seccess\r\n");
}

/**
  *  Tcp client connect success callback function.
  * param  arg: contain the ip link information
  * retval None
  */

FLASH_FUNC void at_tcpclient_connect_cb(void *arg)
{
  struct atbmconn *patbmconn = (struct atbmconn *)arg;
  AT_LinkConType *linkTemp = (AT_LinkConType *)patbmconn->reverse;

  //iot_printf("tcp client connect\r\n");
  //iot_printf("patbmconn %p\r\n", patbmconn);
  //iot_printf("socket num %d\r\n", patbmconn->socket_num);
  linkTemp->linkEn = TRUE;
  //nkTemp->teType = teClient;
  atbmconn_regist_disconcb(patbmconn, at_tcpclient_discon_cb);
  atbmconn_regist_reconcb(patbmconn, at_tcpclient_recon_cb);
  atbmconn_regist_recvcb(patbmconn, at_tcpclient_recv_cb);////////
  atbmconn_regist_sentcb(patbmconn, at_tcpclient_sent_cb);///////
  UART_Send_status(0);
}



FLASH_FUNC void at_udp_create_cb(void *arg)
{
	struct atbmconn *patbmconn = (struct atbmconn *)arg;
	AT_LinkConType *linkTemp = (AT_LinkConType *)patbmconn->reverse;
	linkTemp->linkEn = TRUE;
	//nkTemp->teType = teClient;
	atbmconn_regist_recvcb(patbmconn, at_tcpclient_recv_cb);////////
	atbmconn_regist_sentcb(patbmconn, at_tcpclient_sent_cb);///////
	UART_Send_status(0);
}



/**
  * @brief  Tcp server disconnect success callback function.
  * @param  arg: contain the ip link information
  * @retval None
  */
FLASH_FUNC void at_tcpserver_discon_cb(void *arg,int needfree)
{
	//iot_printf("at_tcpserver_discon_cb\r\n");
	struct atbmconn *patbmconn = (struct atbmconn *)arg;
	char temp[64];
	AT_LinkConType *linkTemp;
	if (patbmconn == NULL)
	{
	   return;
	}
	linkTemp = (AT_LinkConType *)patbmconn->reverse;

	linkTemp->linkEn = FALSE;
	// tcpserver can't free here
	//if(needfree){
	//	os_free(linkTemp->pCon);
	//	linkTemp->pCon = NULL;
	//}
	sprintf(temp, "client ID: %d close",patbmconn->socket_num);
	UART_TSPT_SendStr(temp);
}
/**
  * @brief  Tcp server listend callback function.
  * @param  arg: contain the ip link information
  * @retval None
  */

FLASH_FUNC void at_tcpserver_listen_cb(void *arg)
{
	struct atbmconn *patbmconn = (struct atbmconn *)arg;
	uint8_t i;
    i = patbmconn->socket_num;
    if(pLink[i].linkEn == FALSE)
    {
    	 pLink[i].linkEn = TRUE;
    }
    if(i >= servermaxconn+1)
	{
    	 UART_TSPT_SendStr("ID ERROR\r\n");
	     return;
	}
	pLink[i].linkId = i;
	//pLink[i].teType = teServer;
	pLink[i].pCon = patbmconn;
	patbmconn->reverse = &pLink[i];
	atbmconn_regist_recvcb(pLink[i].pCon, at_tcpclient_recv_cb);
	atbmconn_regist_sentcb(pLink[i].pCon, at_tcpclient_sent_cb);///////
	atbmconn_regist_disconcb(pLink[i].pCon, at_tcpserver_discon_cb);
}

/*
 * AT+SOCKETCONN - 建立TCP 连接, UDP传输,SSL传输
 */
FLASH_FUNC void AT_socketconn(char* pLine)
{
    char *temp;
    char *str;
    u8_t len;
    enum atbmconn_type linkType = ATBMCONN_INVALID;
    char ipTemp[128];
    u32_t re_port,lo_port;
    u32_t ip = 0;
    u32_t linkID;
    int ret;

	//这边一定是DHCP解析成功，获取IP地址以后
    ret = atbmwifi_wait_connect();
	if(ret== FALSE)
	{
		UART_TSPT_SendStr("no ip\r\n");
		return;
	}
	atbm_wifi_wait_get_ipaddr_ok();

	//如果是多连接就进行ID号解析，单连接默认为0
	if(SOCKETMUX_flag)
	{
		//获取ID
		CmdLine_GetInteger(&pLine, &linkID);

	}
	else
	{
		linkID = 0;
	}

	if(linkID >= AT_LinkMax)
	{
		UART_TSPT_SendStr("ID ERROR\r\n");
		return;
	}

	temp  = CmdLine_GetToken(&pLine);            //获取连接类型
	str = CmdLine_GetToken(&pLine);              //获取远端IP地址
	CmdLine_GetInteger(&pLine,&re_port);         //获取远端端口号
	len = strlen(str);
	if(len > (sizeof(ipTemp)-1))
	{
		len= (sizeof(ipTemp)-1);
	}
	memcpy(ipTemp,str,len);
	ipTemp[len] = 0;

	if(memcmp("TCP",temp,strlen("TCP")) ==0)
	{
		linkType = ATBMCONN_TCP;
	}
	else if(memcmp("UDP",temp,strlen("UDP")) == 0)
	{
		linkType = ATBMCONN_UDP;
	}
	else
	{
		UART_TSPT_SendStr("Link type ERROR\r\n");
		return;
	}

	if(pLink[linkID].linkEn == TRUE)
	{
		UART_TSPT_SendStr("ALREAY CONNECT\r\n");
		return;
	}

	pLink[linkID].pCon = (struct atbmconn *)os_zalloc(sizeof(struct atbmconn));
	if (pLink[linkID].pCon == NULL)
	{
		UART_TSPT_SendStr("CONNECT FAIL\r\n");
		return;
	}

	pLink[linkID].linkId = linkID;
	//pLink[linkID].teType = teClient;
	pLink[linkID].pCon->state = ATBMCONN_NONE;
	pLink[linkID].pCon->type = linkType;
	pLink[linkID].pCon->teType = teClient;
	ip = inet_addr(ipTemp);
	//iot_printf("ip = %x\n",ip);
	switch(linkType)
	{
		case ATBMCONN_TCP:
		    if((ip == 0xffffffff) && (os_memcmp(ipTemp,"255.255.255.255",16) != 0))
			{
		    	get_host_by_name(ipTemp,&(pLink[linkID].pCon->proto.tcp.remote_ip));
			}
		    else
		    {
		    	pLink[linkID].pCon->proto.tcp.remote_ip = ip;
		    }

		    pLink[linkID].pCon->proto.tcp.remote_port = re_port;

		    //iot_printf("AT_socketconn port %d,ip%x\n",pLink[linkID].pCon->proto.tcp.remote_port,pLink[linkID].pCon->proto.tcp.remote_ip);

		    pLink[linkID].pCon->reverse = &pLink[linkID];
			pLink[linkID].pCon->socket_close_enable = 0;

		    atbmconn_regist_connectcb(pLink[linkID].pCon, at_tcpclient_connect_cb);
		    atbmconn_connect(pLink[linkID].pCon);
			break;

		case ATBMCONN_UDP:
			CmdLine_GetInteger(&pLine,&lo_port);            //获取本地端口号
			//CmdLine_GetInteger(&pLine,&udp_eg);             //获取UDP mode

			//pLink[linkID].pCon->proto.udp = (atbm_udp *)os_zalloc(sizeof(atbm_udp));
			if((ip == 0xffffffff) && (os_memcmp(ipTemp,"255.255.255.255",16) != 0))
			{
				get_host_by_name(ipTemp,&(pLink[linkID].pCon->proto.udp.remote_ip));
			}
			else
			{
				pLink[linkID].pCon->proto.udp.remote_ip = ip;
			}
			pLink[linkID].pCon->proto.udp.remote_port = re_port;
			pLink[linkID].pCon->proto.udp.local_port = lo_port;
			pLink[linkID].pCon->proto.udp.local_ip = g_wifinetif[0]->ip_addr.addr;
			//pLink[linkID].pCon->proto.udp.udp_class = udp_eg;
			pLink[linkID].pCon->reverse = &pLink[linkID];
			pLink[linkID].linkEn = TRUE;
			atbmconn_regist_createcb(pLink[linkID].pCon, at_udp_create_cb);
			atbmconn_regist_disconcb(pLink[linkID].pCon, at_tcpclient_discon_cb);
			atbmconn_create_server(pLink[linkID].pCon);
			break;
		default:
		  break;
	}
}

FLASH_FUNC void AT_socketserver(char* pLine)
{
	u32_t server_flag;
	u32_t server_port;
	
	if(SOCKETMUX_flag == FALSE)
	{
		UART_Send_status(1);
		return;
	}
	CmdLine_GetInteger(&pLine,&server_flag);         //mode

	if(server_flag == 0)
	{
	   UART_TSPT_SendStr("not support,must reboot\r\n");
	}

	if(server_flag == 1)
	{
		//serverEn = TRUE;
		CmdLine_GetInteger(&pLine,&server_port);         //mode
		pTcpServer = (struct atbmconn *)os_zalloc(sizeof(struct atbmconn));
		if (pTcpServer == NULL)
		{
			UART_TSPT_SendStr("TcpServer Failure\r\n");
		    return;
		}
		pTcpServer->type = ATBMCONN_TCP;
		pTcpServer->state = ATBMCONN_NONE; 
		pTcpServer->teType= teServer;
		pTcpServer->socket_close_enable = 0;
		//pTcpServer->proto.tcp = (atbm_tcp *)os_zalloc(sizeof(atbm_tcp));
		pTcpServer->proto.tcp.local_port = server_port;
		pTcpServer->proto.tcp.local_ip = g_wifinetif[0]->ip_addr.addr;
		atbmconn_regist_connectcb(pTcpServer, at_tcpserver_listen_cb);
		atbmconn_create_server(pTcpServer);
	}
	UART_Send_status(0);
}



FLASH_FUNC void  Net_Uart(void)
{
     while(IPMODE)
     {
    	if(UART_TSPT_NetData_GetLine() == NULL)
		{
			mdelay(10);
			continue;
		}
    	atbmconn_send(pLink[0].pCon,UART_TSPT_RxStr,strlen(UART_TSPT_RxStr));
     }
}

/*
 *   1.单连接时：(CIPMUX_flag = 0)
 *     AT+CIPSEND = <length>
 *   2.多连接时：(CIPMUX_flag = 1)
 *     AT+CIPSEND = <link ID>,<length>
 *   3.如果是 UDP 传输，可以设置远端IP和端口：
 *     AT+CIPSEND = [<link ID>,] <length>[,<remote IP>,<remote port>]
 */
FLASH_FUNC void AT_socketsend(char* pLine)
{
	u32_t sendID;
	u32_t sendLen;

	if(IPMODE == TRUE)
	{
		//iot_printf("Uart_TSPT_Enable_flag = 1\n");
		Uart_TSPT_Enable_flag = 1;
		sys_thread_new("Net_Uart",Net_Uart,NULL,CONFIG_TCPIP_APP_STK_SIZE ,CONFIG_TCPAPP_THREAD_PRIORITY);
	}
	else
	{
		if(SOCKETMUX_flag)
		{
			CmdLine_GetInteger(&pLine, &sendID);
		    //iot_printf("sendID = %d link =%d\n",sendID,pLink[sendID].linkEn);
			if(pLink[sendID].linkEn == FALSE)
			{
				UART_TSPT_SendStr("link is not\r\n");
				return;
			}

			// iot_printf("sendID = %d link =%d++++\n",sendID,pLink[sendID].linkEn);
			if(pLink[sendID].pCon->teType == teClient)
			{

				if(sendID >= AT_LinkMax)
				{
					UART_Send_status(1);
					return;
				}
			}
			if((pLink[sendID].pCon->teType == teServer)||(pLink[sendID].pCon->teType == teClient_inServer))
			{
				// iot_printf("sendID = %d link =%d-----\n",sendID,pLink[sendID].linkEn);
				if(sendID > AT_LinkMax)
				{
					UART_Send_status(1);
					return;
				}

			}
			//iot_printf("sendID = %d link =%d****\n",sendID,pLink[sendID].linkEn);
		}
		else
		{
			sendID = 0;
			if(pLink[sendID].linkEn == FALSE)
			{
				UART_TSPT_SendStr("link is not\r\n");
				return;
			}
		}

		CmdLine_GetInteger(&pLine, &sendLen);
		if(sendLen > NET_SEND_SIZE)
		{
			UART_TSPT_SendStr("too long\r\n");
			return;
		}
		UART_TSPT_SendStr("> ");
		while(UART_TSPT_NetData_GetLine() == NULL)
		{
			mdelay(20);
		}
		if(strlen(UART_TSPT_RxStr) > sendLen)
		{
			UART_TSPT_SendStr("\r\nbusy\r\n");
			atbmconn_send(pLink[sendID].pCon,UART_TSPT_RxStr,sendLen);
		}
		else
		{
			//iot_printf("send num %d\n",pLink[sendID].pCon->socket_num);
			atbmconn_send(pLink[sendID].pCon,UART_TSPT_RxStr,strlen(UART_TSPT_RxStr));
		}

	}
}


FLASH_FUNC void AT_socketmode_end()
{
	   IPMODE = FALSE;
}



FLASH_FUNC void AT_socketmode(char* pLine)
{
	u32_t flag;
	if(SOCKETMUX_flag == 1)
	{
		UART_Send_status(1);
		return;
	}
	else
	{
	    CmdLine_GetInteger(&pLine, &flag);
	    if(flag == 1)
	    {
	    	IPMODE = TRUE;
	    }
	    else if(flag == 0)
	    {
	    	IPMODE = FALSE;
	    }
	    else if(flag == 2)
		{
	    	iot_printf("+SOCKETMODE: %d\n",IPMODE);
		}
	    else
	    {
	    	UART_Send_status(1);
	    	return;
	    }
		UART_Send_status(0);
	}
}

/*
 *   CIPMUX = 2 查询
 *   CIPMUX = 1 多链接
 *   CIPMUX = 0 单连接
 *
 */
FLASH_FUNC void AT_socketmux(char* pLine)
{
	u32_t flag;
	CmdLine_GetInteger(&pLine, &flag);

	if(flag == 0)
	{
		SOCKETMUX_flag = FALSE;
	}

	else if(flag == 1)
	{
		SOCKETMUX_flag = TRUE;
	}

	else if(flag == 2)
	{
		iot_printf("+SOCKETMUX: %d\n",SOCKETMUX_flag);
	}
	else
	{
		UART_Send_status(1);
	}
	UART_Send_status(0);
}

FLASH_FUNC void AT_socketclose(char* pLine)
{
	//iot_printf("AT_socketclose\n");
    u32_t linkID;
    if(SOCKETMUX_flag)
    {
    	CmdLine_GetInteger(&pLine, &linkID);
    }
    else
    {
    	linkID = 0;
    }
    if(linkID > AT_LinkMax)
	{
    	UART_Send_status(1);
		return;
	}

    if((linkID == AT_LinkMax) && (pLink[linkID].pCon->teType == teClient))
    {
		for(linkID = 0; linkID < AT_LinkMax; linkID++)
		{
			if(pLink[linkID].linkEn)
			{
				pLink[linkID].pCon->socket_close_enable = 1;
			}
			else
			{
				 UART_TSPT_SendStr("link is not\r\n");
			}
		}
     }
    else
    {
    	//iot_printf("AT_socketclose-----\n");
    	if(pLink[linkID].linkEn == FALSE)
		{
    		UART_TSPT_SendStr("link is not\r\n");
		    return;
		}
    	else
    	{
    		if(pLink[linkID].pCon->type == ATBMCONN_UDP)
    		{
    			//atbmconn_disconnect(pLink[linkID].pCon);
				pLink[linkID].pCon->socket_close_enable = 1;
    		}
    		else
    		{
    			//atbmconn_disconnect will be call later at atbmconn_recv
				if(pLink[linkID].pCon->teType == teClient)
				{
					pLink[linkID].pCon->socket_close_enable = 1;
				}
				else
				{
					atbmconn_disconnect(pLink[linkID].pCon);
				}
    		}
    	}
    }
}


FLASH_FUNC void AT_socketstatus(char* pLine)
{
	int i=0;
	char temp[64];
	for(i=0; i<AT_LinkMax; i++)
	{
		if(pLink[i].linkEn)
		{
			if(pLink[i].pCon->type == ATBMCONN_TCP)
			{
			   sprintf(temp, "\"+SOCKETSTATUS\":%d,\"TCP\",\"%d.%d.%d.%d\",%d,%d\r\n",
				                   pLink[i].linkId,
				                   ip4_addr1(&(pLink[i].pCon->proto.tcp.remote_ip)),
				                   ip4_addr2(&(pLink[i].pCon->proto.tcp.remote_ip)),
				                   ip4_addr3(&(pLink[i].pCon->proto.tcp.remote_ip)),
				                   ip4_addr4(&(pLink[i].pCon->proto.tcp.remote_ip)),
				                   pLink[i].pCon->proto.tcp.remote_port,
				                   pLink[i].pCon->teType);
			   UART_TSPT_SendStr(temp);
			}
			else
			{
				sprintf(temp, "\"+SOCKETSTATUS\":%d,\"UDP\",\"%d.%d.%d.%d\",%d,%d\r\n",
								   pLink[i].linkId,
								   ip4_addr1(&(pLink[i].pCon->proto.tcp.remote_ip)),
								   ip4_addr2(&(pLink[i].pCon->proto.tcp.remote_ip)),
								   ip4_addr3(&(pLink[i].pCon->proto.tcp.remote_ip)),
								   ip4_addr4(&(pLink[i].pCon->proto.tcp.remote_ip)),
								   pLink[i].pCon->proto.tcp.remote_port,
								   pLink[i].pCon->teType);
				UART_TSPT_SendStr(temp);
			}
		}
	}
	UART_Send_status(0);
}

FLASH_FUNC void AT_servermaxconn(char* pLine)
{
	CmdLine_GetInteger(&pLine, &servermaxconn);
	if((servermaxconn >= 0) && (servermaxconn <= 3))
	{
		UART_Send_status(0);
	}
	else
	{
		UART_Send_status(1);
	}
}

FLASH_FUNC void AT_socketsto(char* pLine)
{
	u32_t time;
	CmdLine_GetInteger(&pLine, &time);
	conntimeout = (int)time;
}

/***********************************************************************************************/
/***********************************************************************************************/
/***********************************************************************************************/
/*
   SNTP query internal function.
*/
FLASH_FUNC void sntp_query_func()
{
    time_t time = 0;
    uint8 u8cnt = 0;
    /*Set ntp server*/
	sntp_setservername(0, "cn.ntp.org.cn");
	sntp_setservername(1, "us.pool.ntp.org");
	sntp_setservername(2, "cn.pool.ntp.org");

	/*Set time zone, here set as East 8 time zone, Beijing time*/
	//sntp_set_timezone(8);

    /*start sntp query*/
	sntp_init();	

	/*get ntp time and print converted date*/
	while((time == 0)&&(u8cnt < 15))
	{
		time = sntp_get_current_timestamp();	
		//wifi_printk(WIFI_ATCMD,"sntp_sample_codes time:%u  date:%s\n", time, sntp_get_real_time(time));
		hal_sleep(200);
		u8cnt++;
	}	
//	wifi_printk(WIFI_ATCMD,"sntp_sample_codes time:%u  date:%s\n", time, sntp_get_real_time(time));
	sntp_stop();
}



/*
    sntp using tcp/ip socket, sys_thread_new is required, after time quiry succeed, a timer will work.
*/
#define CONFIG_SNTP_STK_SIZE (1024*3)
#define CONFIG_SNTP_THREAD_PRIORITY CONFIG_TCPAPP_THREAD_PRIORITY

FLASH_FUNC void  sntp_query_thread(void *param)
{				
	 sntp_query_func();	
	 
}
FLASH_FUNC void sntp_query_Init(void)
{	
	if(sntp_enabled()){
		iot_printf("sntp_query not end,retry again\n");
		return ;
	}
	//wifi_printk(WIFI_ATCMD,"sntpthread++ \n");	
	sys_thread_new("sntpthread", sntp_query_thread, NULL,CONFIG_SNTP_STK_SIZE,CONFIG_SNTP_THREAD_PRIORITY);
}

/*
 *AT+SNTP_SET	设置时区,NTP Serve
 *sample:	AT+SNTP_SET  ZONE  8
  zone range [0:23]
 *This cmd is optional, used when time zone set is required, default set is Beijing time zone.
 *
 */
FLASH_FUNC void AT_SntpSet(char* pLine)
{
		u32 tmpdata = 0;

		signed char s8TimeZone = 8; 
		char *str = NULL;
		//char server_name[32]={0};
		if(!memcmp("ZONE",pLine,strlen("ZONE")))
		{				
			str = CmdLine_GetToken(&pLine);
			if(str !=NULL)
			{
				CmdLine_GetInteger(&pLine, &tmpdata);
			}
			//wifi_printk(WIFI_ATCMD,"SNTP time zone set : %d\n",tmpdata);	
			#if 0
			if((tmpdata >= -11)&&(tmpdata <= 13))
			{
				s8TimeZone = (signed char)tmpdata;
				//wifi_printk(WIFI_ATCMD,"AT_SntpSet tmpdata %d\n",tmpdata);	
				sntp_set_timezone(s8TimeZone);
			}
			#endif		
			if((tmpdata >= 0)&&(tmpdata < 24))
			{
				if(tmpdata > 12)
				{
					s8TimeZone = (signed char)(tmpdata - 24);				
				}else
				{
					s8TimeZone = (signed char)tmpdata;
				}
				wifi_printk(WIFI_ATCMD,"AT_SntpSet tmpdata %d\n",s8TimeZone);	
				sntp_set_timezone(s8TimeZone);
				
				UART_Send_status(0);
				return;
			}					
		}	
		
		UART_Send_status(1);
}


/*
 * AT+SNTP_GET	获取SNTP 时间
 * sample:	AT+SNTP_GET
 */
FLASH_FUNC void AT_SntpGet(char* pLine)
{	
	UN_USED_PARAMS(*pLine);
    uint32 time = 0;

	atbm_wifi_wait_get_ipaddr_ok();
	
	sntp_query_Init();  
	
	hal_sleep(1000);
	
	time = sntp_get_current_timestamp();	
	
	UART_SendString("time:%u  date:%s\n", time, sntp_get_real_time(time));
	//stop will call in sntp thread
	//sntp_stop(); 

}

void AT_test(char* pLine){
	UN_USED_PARAMS(*pLine);
#if CONFIG_ATBM_PRODUCT_TEST
	int channel=1;
	if(pLine){
		CmdLine_GetInteger(&pLine, &channel);
		if((channel < 1) ||(channel>11))
			channel = 1;
	}
	sta_scan_atbm_product_test(channel); 
#endif //#if CONFIG_ATBM_PRODUCT_TEST
}

FLASH_FUNC void AT_DhcpSetTmp(char *pLine)
{	
	UN_USED_PARAMS(*pLine);
	int enable=1;
//	ret =parse_gpio_dir_cmd(pLine);
	
	CmdLine_GetInteger(&pLine, &enable);
	if((enable != 0)&&(enable != 1)){
		UART_Send_status(HAL_ERR_UNAVAILABLE);
		return;
	}
	
	if(atbm_wifi_is_iftype_ap()) {
		UART_Send_status(HAL_ERR_UNAVAILABLE);
		return;
	}
	
	if(enable)	{	
		int static_ipaddr = 0;
		iot_boot_volatile_sect.STA_tcpip_param.static_ip = 0;			
		//clear ip addr
		netif_set_addr(atbm_wifi_get_netdev(),&static_ipaddr,&static_ipaddr,&static_ipaddr);
		dhcp_start(atbm_wifi_get_netdev());
	}
	else {		
		iot_boot_volatile_sect.STA_tcpip_param.static_ip = 1;
		//set  IP address     
		netif_set_addr(atbm_wifi_get_netdev(),
			(ip4_addr_t *)iot_boot_volatile_sect.STA_tcpip_param.static_ipaddr,
			(ip4_addr_t *)iot_boot_volatile_sect.STA_tcpip_param.static_ipmask,
			(ip4_addr_t *)iot_boot_volatile_sect.STA_tcpip_param.static_gwaddr);
		
	}

	UART_Send_status(0);
}


FLASH_FUNC void AT_DhcpSet(char *pLine)
{	
	UN_USED_PARAMS(*pLine);
	int enable=1;
	
	CmdLine_GetInteger(&pLine, &enable);	
	if((enable != 0)&&(enable != 1)){
		UART_Send_status(HAL_ERR_UNAVAILABLE);
		return;
	}
	if(atbm_wifi_is_iftype_ap()) {
		UART_Send_status(HAL_ERR_UNAVAILABLE);
		return;
	}
	if(enable)	{	
		int static_ipaddr = 0;
		iot_boot_volatile_sect.STA_tcpip_param.static_ip = 0;			
		//clear ip addr
		netif_set_addr(atbm_wifi_get_netdev(),&static_ipaddr,&static_ipaddr,&static_ipaddr);
		dhcp_start(atbm_wifi_get_netdev());
	}
	else {		
		iot_boot_volatile_sect.STA_tcpip_param.static_ip = 1;
		//set  IP address     
		netif_set_addr(atbm_wifi_get_netdev(),
			(ip4_addr_t *)iot_boot_volatile_sect.STA_tcpip_param.static_ipaddr,
			(ip4_addr_t *)iot_boot_volatile_sect.STA_tcpip_param.static_ipmask,
			(ip4_addr_t *)iot_boot_volatile_sect.STA_tcpip_param.static_gwaddr);
		
	}
	atbmwifi_flash_param_staipconfig_change();
	
	UART_Send_status(0);
}

#ifdef AT_SSLCONN
static FLASH_FUNC void AT_sslcon(char *pLine)
{
      char *class;
      char *addr;
      char *port;
      int LinkID = 0;
      class = CmdLine_GetToken(&pLine);
      addr = CmdLine_GetToken(&pLine);
      port = CmdLine_GetToken(&pLine);
      iot_printf("class = %s,addr = %s,port = %s\n",class,addr,port);
      if(LinkID > 0)
      {
    	  UART_TSPT_SendStr("ATMB8266 only none connection is supported\r\n");
      }
      if(memcmp("SSL",class,strlen("SSL")) != 0)
      {
    	  UART_TSPT_SendStr("Connection type error\r\n");
      }
      memset(cli_connect.svr_info.Host,0,strlen(cli_connect.svr_info.Host));
      memset(cli_connect.svr_info.Port,0,strlen(cli_connect.svr_info.Port));
      os_memcpy(cli_connect.svr_info.Host,addr,strlen(addr));
      os_memcpy(cli_connect.svr_info.Port,port,strlen(port));
      cli_connect.svr_info.Host[15] = '\0';
      cli_connect.svr_info.Port[4] = '\0';
      iot_printf("addr = %s,port = %s\n",cli_connect.svr_info.Host,cli_connect.svr_info.Port);
      //atbm_mbedtls_client_connect(&cli_connect);
      sys_thread_new(" SSLCON", atbm_mbedtls_client_connect, &cli_connect,CONFIG_MBEDTLS_STACK_SIZE,CONFIG_TCPAPP_THREAD_PRIORITY);
}

static FLASH_FUNC void AT_sslsend(char *pLine)
{
	u32_t sendLen;
	CmdLine_GetInteger(&pLine, &sendLen);
	if(sendLen > NET_SEND_SIZE)
	{
		UART_TSPT_SendStr("too long\r\n");
		return;
	}
	UART_TSPT_SendStr("> ");
	while(UART_TSPT_NetData_GetLine() == NULL)
	{
		mdelay(20);
	}
	if(strlen(UART_TSPT_RxStr) > sendLen)
	{
		UART_TSPT_SendStr("\r\nbusy\r\n");
		atbm_mbedtls_write(&cli_connect,UART_TSPT_RxStr,sendLen);
	}
	else
	{
		//iot_printf("send num %d\n",pLink[sendID].pCon->socket_num);
		atbm_mbedtls_write(&cli_connect,UART_TSPT_RxStr,strlen(UART_TSPT_RxStr));
	}
}

static FLASH_FUNC void AT_sslclose(char *pLine)
{
	atbm_mbedtls_client_disconnect(&cli_connect);
}
#endif //#ifdef AT_SSLCONN

static FLASH_FUNC void AT_http_ota(char *pLine)
{
	char *posChar = NULL;
	posChar = strstr(pLine, "http");  /*URL input*/
	if (posChar)
	{
		sample_http_ota_APP_Init(pLine);
	}else
	{
		sample_http_ota_APP_Init(NULL);
	}
}


FLASH_FUNC static int do_dhcp(char *pLine)
{
#if LWIP_DHCP
	if(dhcp_start(g_wifinetif[0]) != ERR_OK)
		iot_printf("dhcp start error\n");
	else
		iot_printf("dhcp start successfully\n");	
#endif //#if LWIP_DHCP
	return 0;
}
/*ifconfig ip 192.168.1.221  netmask 255.255.255.0 gw 192.168.1.2*/
FLASH_FUNC static int do_ifconfig(char *pLine)
{
	ip4_addr_t netmask, gw, ip_addr;
	int idx = 1;
	char * str;
	
	if (!memcmp("ip",pLine,2))
	{
		str = CmdLine_GetToken(&pLine);
		if (!memcmp("ip",str,2)) {			
			str = CmdLine_GetToken(&pLine);
			ip_addr.addr = inet_addr(str);
			netif_set_ipaddr(g_wifinetif[0], &ip_addr);
			idx += 2;
		}
		
		str = CmdLine_GetToken(&pLine);
		if (!memcmp("netmask",str,7)) {
			str = CmdLine_GetToken(&pLine);
			netmask.addr = inet_addr(str);
			netif_set_netmask(g_wifinetif[0], &netmask);
			idx += 2;
		}
		
		str = CmdLine_GetToken(&pLine);
		if (!memcmp("gw",str,2)) {
			str = CmdLine_GetToken(&pLine);
			gw.addr = inet_addr(str);
			netif_set_gw(g_wifinetif[0], &gw);
			idx += 2;
		}
	}

	DEBUG(1, 0, "ip: %s, ", inet_ntoa(*(struct in_addr*)&g_wifinetif[0]->ip_addr.addr));
	DEBUG(1, 0, "netmask: %s, ", inet_ntoa(*(struct in_addr*)&g_wifinetif[0]->netmask.addr));
	DEBUG(1, 0, "gw: %s\n", inet_ntoa(*(struct in_addr*)&g_wifinetif[0]->gw.addr));

	return 0;
}
#if LWIP_RAW

#define PING_ID        0x0100
#define PING_LEN      (16)      /** ping additional data size to include in the packet */
static u16_t    ping_seq_num;

FLASH_FUNC static void ping_build( struct icmp_echo_hdr *iecho, u16_t len )
{
    int i;

    ICMPH_TYPE_SET( iecho, ICMP_ECHO );
    ICMPH_CODE_SET( iecho, 0 );
    iecho->chksum = 0;
    iecho->id = PING_ID;
    iecho->seqno = htons( ++ping_seq_num );

    /* fill the additional data buffer with some data */
    for ( i = 0; i < PING_LEN; i++ )
    {
        ( (char*) iecho )[sizeof(struct icmp_echo_hdr) + i] = 'a'+i;
    }

    iecho->chksum = inet_chksum((void*) iecho, len );
	
}

FLASH_FUNC static err_t ping_send( int socket_s, ip4_addr_t *addr )
{
    int err;
    struct icmp_echo_hdr *iecho;
    struct sockaddr_in d_addr;
    size_t ping_size = sizeof(struct icmp_echo_hdr) + PING_LEN;

    // Allocate memory for packet
    if ( ( iecho = os_malloc( ping_size ) ) == NULL)
    {
    	iot_printf("meme alloc failed\n");
        return ERR_MEM;
    }
	
    ping_build( iecho, ping_size );

    // Send the ping request
    d_addr.sin_len = sizeof( d_addr );
    d_addr.sin_family = AF_INET;
    d_addr.sin_addr.s_addr = addr->addr;

    err = lwip_sendto( socket_s, (const void*)iecho, ping_size, 0, (struct sockaddr*) &d_addr, sizeof( d_addr ) );

    // free packet
    os_free( (void*)iecho );

    return ( err ? ERR_OK : ERR_VAL );
}

FLASH_FUNC static err_t ping_recv( int socket_s )
{
    char buf[64];
    int fromlen, len;
    struct sockaddr_in from;
    struct ip_hdr *iphdr;
    struct icmp_echo_hdr *iecho;

    while ( ( len = lwip_recvfrom( socket_s, buf, sizeof( buf ), 0,
            (struct sockaddr*) &from, (socklen_t*) &fromlen ) ) > 0 )
    {
        if ( len >= (int)( sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr) ) )
        {
            iphdr = (struct ip_hdr *) buf;
            iecho = (struct icmp_echo_hdr *) ( buf + ( IPH_HL( iphdr ) * 4 ) );

            if ( ( iecho->id == PING_ID ) &&
                 ( iecho->seqno == htons( ping_seq_num ) ) &&
                 ( ICMPH_TYPE( iecho ) == ICMP_ER ) )
            {
                return ERR_OK; // Echo reply received - return success
            }
        }
    }

    return ERR_TIMEOUT; // No valid echo reply received before timeout
}

FLASH_FUNC  void do_ping(char* pLine)
{
    int socket_s = -1;
    int recv_timeout = 1000;
	int i = 0;
	u32_t send_time;
	err_t result;
	u32_t ip;
	ip4_addr_t ipaddr; 
	char *str;
	
	str = CmdLine_GetToken(&pLine);	
	ipaddr.addr = inet_addr(str);
	ip = inet_addr(str);

	if((ip == 0xffffffff) && (os_memcmp(str,"255.255.255.255",16) != 0))
	{
		get_host_by_name(str,&(ipaddr.addr));
	}
	else
	{
		ipaddr.addr = ip;
	}

    // Open a local socket for pinging
    if ( ( socket_s = lwip_socket( AF_INET, SOCK_RAW, IP_PROTO_ICMP ) ) < 0 )
    {
        iot_printf("unable to create socket for Ping\r\n");
        return;
    }

    // Set the receive timeout on local socket so pings will time out.
    lwip_setsockopt( socket_s, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof( recv_timeout ) );

    iot_printf("ping: %u.%u.%u.%u\r\n", 
			(unsigned char) ( ( htonl(ipaddr.addr) >> 24 ) & 0xff ),
            (unsigned char) ( ( htonl(ipaddr.addr) >> 16 ) & 0xff ),
            (unsigned char) ( ( htonl(ipaddr.addr) >> 8 ) & 0xff ),
            (unsigned char) ( ( htonl(ipaddr.addr) >> 0 ) & 0xff ));


    do {
        // Send a ping
        if ( ping_send( socket_s, (ip4_addr_t*) &ipaddr.addr ) != ERR_OK )
        {
            iot_printf("Unable to send Ping\r\n");
            return;
        }

        // Record time ping was sent
        send_time = hal_time_get();

        // Wait for ping reply
        result = ping_recv( socket_s );
		
        // iot_printf result
        if ( ERR_OK ==  result)
        {
            iot_printf("ping %d reply %dms\r\n",i+1, (int)( hal_time_get() - send_time ));
        }
        else
        {
            iot_printf("ping timeout\r\n");
        }

        hal_sleep(500);
    } while (i++<10);

	
	close_socket(socket_s);

	iot_printf("Ping completed!\n");
}
#endif //#if LWIP_RAW



/*
 * @brief  域名解析
 *
 */
FLASH_FUNC int get_host_by_name(char *hostname, uint32_t *addr)
{
	char err_t;
	ip4_addr_t ip_addr;
	/**
	 * Execute a DNS query, only one IP address is returned
	 *
	 * @param name a string representation of the DNS host name to query
	 * @param addr a preallocated ip_addr_t where to store the resolved IP address
	 * @return ERR_OK: resolving succeeded
	 *         ERR_MEM: memory error, try again later
	 *         ERR_ARG: dns client not initialized or invalid hostname
	 *         ERR_VAL: dns server response was invalid
	 */
	mutex_lock(&socket_mutex);
	err_t = netconn_gethostbyname(hostname, &ip_addr);
	mutex_unlock(&socket_mutex);
	if (err_t == ERR_OK)
	{
		*addr = ip_addr.addr;
		return 0;
	}

	return  - 1;
}


FLASH_FUNC int do_dns_query(char* pLine)
{
	uint32_t addr;
	int ret=-1;
	char *hostname;
	char strTmp[64] = {0};


	hostname = CmdLine_GetToken(&pLine);	
	if (NULL == hostname)
	{
		iot_printf("input param error\r\n");
		goto __err;
	}

	iot_printf("dns query start %s\n",hostname);

    //需要指定dns server的地址?
    //dns_setserver(u8_t numdns,struct ip_addr * dnsserver);

    ret = get_host_by_name(hostname, &addr);
	if (ret == 0)
	{
		sprintf(strTmp, "+ok=ipaddr:%d.%d.%d.%d\n", ip4_addr1(&addr), ip4_addr2(&addr), ip4_addr3(&addr), ip4_addr4(&addr));
		//iot_printf("get %s, ipaddr:: %d.%d.%d.%d\n", hostname, ip4_addr1(&addr), ip4_addr2(&addr), ip4_addr3(&addr), ip4_addr4(&addr));
		//UART_Send_status(ret);
		UART_SendString(strTmp);
		return 0;

	}
__err:
	UART_SendString("DNS FAIL\n");
	UART_Send_status(ret);

	return 0;
}




static  struct cli_cmd_struct ATCommands_net[] =
{
	{ .cmd = "dhcp",	.fn = do_dhcp},
	{ .cmd = "AT+IFCONFIG",.fn = do_ifconfig},
#if LWIP_RAW
	{ .cmd = "AT+PING",	.fn = do_ping},
#endif //#if LWIP_RAW
	{ .cmd = "AT+DNS",		.fn = do_dns_query},
	//{.cmd ="AT+tcpipSt",			   .fn = AT_TcpIpStatus,		.next = (void*)0},
	{.cmd ="AT+iperfc", 			   .fn = AT_TXIperfTest,		.next = (void*)0},
	{.cmd ="AT+iperfs", 			   .fn = AT_RXIperfTest,		.next = (void*)0},
	//{.cmd ="AT+test",				   .fn = AT_test,			    .next = (void*)0},
	{.cmd ="AT+SOCKETMUX",		 	   .fn = AT_socketmux,			.next = (void*)0},
	{.cmd ="AT+SOCKETMODE",			   .fn = AT_socketmode,			.next = (void*)0},
	{.cmd ="AT+SOCKETCONN",			   .fn = AT_socketconn,			.next = (void*)0},
	{.cmd ="AT+SOCKETSERVER",		   .fn = AT_socketserver,	    .next = (void*)0},
	{.cmd ="AT+SOCKETCLOSE",		   .fn = AT_socketclose,		.next = (void*)0},
	{.cmd ="AT+SOCKETSEND",			   .fn = AT_socketsend,			.next = (void*)0},
	{.cmd ="AT+SOCKETSTATUS",		   .fn = AT_socketstatus,		.next = (void*)0},
	{.cmd ="AT+SOCKETSTO",		       .fn = AT_socketsto,		    .next = (void*)0},
	{.cmd ="AT+SERVERMAXCONN",		   .fn = AT_servermaxconn,		.next = (void*)0},
#ifdef AT_SSLCONN
	{.cmd ="AT+SSLCONN",		       .fn = AT_sslcon,		        .next = (void*)0},
	{.cmd ="AT+SSLSEND",		       .fn = AT_sslsend,		    .next = (void*)0},
	{.cmd ="AT+SSLCLOSE",		       .fn = AT_sslclose,		    .next = (void*)0},
#endif //AT_SSLCONN
	{.cmd ="AT+SNTP_SET",			   .fn = AT_SntpSet,            .next = (void*)0},
	{.cmd ="AT+SNTP_GET",			   .fn = AT_SntpGet,            .next = (void*)0},
	{.cmd ="AT+WIFI_DHCP_TMP",		   .fn = AT_DhcpSetTmp, 	    .next = (void*)0},
	{.cmd ="AT+WIFI_DHCP",			   .fn = AT_DhcpSet, 		    .next = (void*)0},
#ifdef CONFIG_OTA_SUPPORT
	{.cmd ="AT+tftp",				   .fn = AT_Tftp,				.next = (void*)0},
	{.cmd ="AT+HTTP_OTA",		  	   .fn = AT_http_ota,	    .next = (void*)0},
#endif  //CONFIG_OTA_SUPPORT
	{.cmd ="AT+ALIYUN_TEST",		   .fn = AT_Aliyun_test,	    .next = (void*)0},

}; 

FLASH_FUNC void ATCmd_init_net(void)
{
	 cli_add_cmds(ATCommands_net,sizeof(ATCommands_net)/sizeof(ATCommands_net[0]));
}
