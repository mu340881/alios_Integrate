/**************************************************************************************************************
 * altobeam iot at cmd file 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 * Permission is hereby granted for use on Altobeam IOT 6421 only,
 * in which case ,it is free to  modify it
 *
*****************************************************************************************************************/

#include <netif.h>
#include "cli.h"
#include "lwip/init.h"
#include "lwip/dhcp.h"
#include "lwip/tcpip.h"
#include "lwip/inet.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "wifi_api.h" 
#include "atbm_etf.h"
#include "os_api.h"
#include "hal.h"

extern void lmac_Main(void);
extern void ATCmd_init(void);
extern void ATCmd_loop(void);
extern void user_app_init();

extern struct netif * g_wifinetif[MAX_VMAC_NUM];
extern void flash_write_user_key_v2(void);
extern void key_recheck();

PASRAM_SHARE  u8 sram_pool[1];
DTCM_ALLOC u8 heap_pool[1];



hal_bh_t app_bh;
hal_bh_t lmac_bh;
hal_bh_t hmac_bh;

#ifdef OS_TASK_DEBUG
struct task_debug os_task_dbg;
#endif //OS_TASK_DEBUG 


//socket互斥锁
mutex_t socket_mutex;

FLASH_FUNC void  OS_TaskHMac (void *param);
void TcpIp_InitDone(void * param)
{
	//wifi_printk(WIFI_OS,"TcpIp_InitDone++ \n");
}

void network_init(void)
{
	mutex_init(&socket_mutex);
	tcpip_init(NULL, NULL);
}




void  OS_TaskHMac (void *param)
{

	//wifi_save_ande_printk_Stack();
	//TaskDebug_Put();
	//wait lmac init
	hal_sleep(10);
	//start wifi hmac
	atbmwifi_init();
}


void  OS_TaskLMac(void *param)
{
	//wifi_save_ande_printk_Stack();	
	//TaskDebug_Put();
	lmac_Main();
}


void  OS_TaskApp(void *param)
{		
	//wifi_save_ande_printk_Stack();	
	//wait wifi init
	enable_interrupt(DEV_INT_NUM_UART2);
	enable_interrupt(DEV_INT_NUM_UART);
	iot_printf("\n\n\n#######################################################\n");
	iot_printf("# 		                                     #\n");
	iot_printf("# 		  ALTOBEAM IOT  WIFI                 #\n");
	iot_printf("# 		                                     #\n");
	iot_printf("#######################################################\n");
	
		/***********************init wifi******************************/
	lmac_bh.th.fn		= OS_TaskLMac;
	lmac_bh.th.name 	= "lmac";
	lmac_bh.th.stack_size = CONFIG_LMAC_STK_SIZE; 
	lmac_bh.th.arg		= &lmac_bh;
	lmac_bh.th.prio 	= CONFIG_LMAC_THREAD_PRIORITY;
	lmac_bh.th.task 	= NULL;
	lmac_bh.th.ptos 	= NULL; 

	if(hal_create_bh(&lmac_bh) != HAL_SUCCESS){
		wifi_printk(WIFI_OS,"lmac_bh fail\n");
		return; 
	}

	hmac_bh.th.fn		= OS_TaskHMac;
	hmac_bh.th.name 	= "hmac";
	hmac_bh.th.stack_size = CONFIG_HMAC_STK_SIZE; 
	hmac_bh.th.arg		= &hmac_bh;
	hmac_bh.th.prio 	= CONFIG_HMAC_THREAD_PRIORITY;
	hmac_bh.th.task 	= NULL; 
	hmac_bh.th.ptos 	= NULL; 
 
	if(hal_create_bh(&hmac_bh) != HAL_SUCCESS){
		wifi_printk(WIFI_OS,"app_bh fail\n");  
		return;
	}


	/***********************init wifi end******************************/ 
	//wifi_save_ande_printk_Stack();	
	//wait wifi init
	hal_sleep(10);
	//
	atbm_uart0_init();

	network_init();
	
#if (CHIP_TYPE==CHIP_ARES_B)
	ATBM_init_hardware_hash_cipher();
#endif
 	//
	ATCmd_init();
	//
#if CONFIG_ATBM_PRODUCT_TEST
	if(atbmwifi_is_etf_testmode()==0)
#endif //#if CONFIG_ATBM_PRODUCT_TEST
	{
		user_app_init();
	}
	/*AT cmd loop function,this function will not return*/
	ATCmd_loop();

}
#define INIT_STACK_LEN (1024*2)

/****** main() function *******/  
/**************************************************************************************************
注意:APP_Init 中不能修改任何函数，
连打印也不能删除和添加，否则会出现未知错误
***************************************************************************************************/
FLASH_FUNC void APP_Init(void)
{
	extern char STACK_NZI_BEGIN;/*define in ld script file*/
	u32 heap_pool_len =(u32)&STACK_NZI_BEGIN- (u32)&heap_pool[0];
	u32 sram_pool_len = SRAM_ADDRESS + SRAM_LEN -64/*just for debug*/  - (u32)&sram_pool[0];

	_pre_user_app_init();

	//iot_printf("dccm %d sram %d\n",heap_pool_len,sram_pool_len);
#if CONFIG_ATBM_PRODUCT_TEST_NO_UART
		hal_uart_dbg_close();
#endif

		
#if CONFIG_ATBM_PRODUCT_TEST
	atbmwifi_etf_init();	
#endif	//CONFIG_ATBM_PRODUCT_TEST
	flash_param_config_init();
#if CONFIG_ATBM_PRODUCT_TEST
	//flash_write_user_key();
	//flash_write_user_key_v2();
#endif	

	//check usr key valid
	flash_init_check_user_key();
#if CONFIG_ATBM_PRODUCT_TEST
	//key_recheck();
#endif


	UART_change_params(iot_boot_volatile_sect.uart_param.uart_Baudrate,
						iot_boot_volatile_sect.uart_param.uart_DataBits, 
						iot_boot_volatile_sect.uart_param.uart_StopBits,
						iot_boot_volatile_sect.uart_param.uart_Parity);


	hal_init_os(sram_pool,sram_pool_len,heap_pool,heap_pool_len);




	app_bh.th.fn 		= OS_TaskApp;
	app_bh.th.name 		= "main";
	app_bh.th.stack_size = CONFIG_MAIN_STACK_SIZE;
	app_bh.th.arg 		= &app_bh; 
	app_bh.th.prio 		= CONFIG_LMAC_THREAD_PRIORITY;// CONFIG_MAIN_TASK_PRIORITY;  
	app_bh.th.task		= NULL;
	app_bh.th.ptos		= NULL;	

	if(hal_create_bh(&app_bh) != HAL_SUCCESS){
		wifi_printk(WIFI_OS,"app_bh fail\n");
		return;
    } 
	
	
	//os tick  	
	Tmr_TickInit();		/* Initialize tick ISR */

	
	//enable os vTaskSwitchContext interrupt
	//enable_interrupt(DEV_INT_NUM_WIFI_SW);
	//disable_interrupt(DEV_INT_NUM_UART2);
	//disable_interrupt(DEV_INT_NUM_UART);
	
	hal_start_os();

}

