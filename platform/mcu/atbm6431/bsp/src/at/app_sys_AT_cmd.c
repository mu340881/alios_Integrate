/**************************************************************************************************************
 * altobeam iot at cmd file 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 * Permission is hereby granted for use on Altobeam IOT 6421 only,
 * in which case ,it is free to  modify it
 *
*****************************************************************************************************************/


#include "app_AT_cmd.h"
 
extern unsigned int wifi_debug_mask;
extern unsigned int wifi_lwip_level;
extern FLASH_FUNC void AT_WifiStatus(char *Args);
extern 	char* sntp_get_real_time(time_t t);

FLASH_FUNC void AT_reboot(char *Args)
{
	UN_USED_PARAMS(*Args);
	system_reboot();
}


/*显示memory 分配情况，如果需要更详细的，请打开宏DEBUG_MALLOC_LIST and DEBUG_MALLOC*/
FLASH_FUNC void AT_MemoryStatus(char *Args)
{
	UN_USED_PARAMS(*Args);
	sys_heapShow(MEM_HEAP);
	sys_heapShow(MEM_SRAM);
}



/*打开lwip 打印，等级越低打印越多
#define LWIP_DEBUGF(debug, message) do { \
       if ( \
           ((debug) & LWIP_DBG_ON) && \
           ((debug) & LWIP_DBG_TYPES_ON) && \
           ((s16_t)((debug) & LWIP_DBG_MASK_LEVEL) >= wifi_lwip_level)) { \
         LWIP_PLATFORM_DIAG(message); \
         if ((debug) & LWIP_DBG_HALT) { \
           while(1); \
         } \
       } \
     } while(0)
*/
FLASH_FUNC void AT_SetLwipDbgMask(char *Args)
{
    CmdLine_GetHex(&Args, &wifi_lwip_level);
}

/**
  *****************************************************************************
  * @Name   : 系统状态任务,cpu 占用率,
  * 第一次是初始化结果是不对的,第二次开始正常
  *  
  *****************************************************************************
**/
#define TASK_STATE_SHOW_NUM 20
DTCM2_ZI uint32 g_taskRunTimeCounter[TASK_STATE_SHOW_NUM];
DTCM2_ZI uint32 g_taskTotalRunTime=0;

FLASH_FUNC void rtos_state_task(int print)
{
	//EventBits_t EventStateValue;
	#if ( configUSE_TRACE_FACILITY == 1 )
	char *tmp;
	const char task_state[] = {'r','R','B','S','D'};  
    volatile UBaseType_t uxArraySize, x;  
    uint32_t ulTotalRunTime,ulStatsAsPercentage;
	TaskStatus_t *pxTaskStatusArray;

	
	uxArraySize = uxTaskGetNumberOfTasks();  //获取任务总数
	pxTaskStatusArray = kmalloc(uxArraySize * sizeof(TaskStatus_t));
	if (pxTaskStatusArray != NULL)
	{
		uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalRunTime );  //获取任务状态信息
		#if ( configGENERATE_RUN_TIME_STATS == 1 )
		
		if(print){
			iot_printf("任务状态:   r-运行  R-就绪  B-阻塞  S-挂起  D-删除 %d\r\n",portGET_RUN_TIME_COUNTER_VALUE());
			iot_printf("任务名          状态  ID    优先级  堆栈    CPU使用率\r\n");
		}
		if ( (ulTotalRunTime - g_taskTotalRunTime) > 0 )
		{
			tmp = kmalloc(128);
			if (tmp != NULL)
			{
				if(uxArraySize > TASK_STATE_SHOW_NUM)
					uxArraySize = TASK_STATE_SHOW_NUM;
				//格式化输出每个人物状态信息
				for ( x = 0; x < uxArraySize; x++ )
				{
					if(pxTaskStatusArray[x].xTaskNumber >= TASK_STATE_SHOW_NUM)
						continue;
					ulStatsAsPercentage =(uint64_t)(pxTaskStatusArray[ x ].ulRunTimeCounter - g_taskRunTimeCounter[pxTaskStatusArray[x].xTaskNumber])*100 / (ulTotalRunTime - g_taskTotalRunTime);  //计算任务运行时间与总运行时间百分比
					
						
					if ( ulStatsAsPercentage > 0UL )
					{
						sprintf(tmp, "%12s%6c%6d%8d%8d    %d%%", pxTaskStatusArray[x].pcTaskName,\
						                                          task_state[pxTaskStatusArray[x].eCurrentState],\
						                                          pxTaskStatusArray[x].xTaskNumber,\
						                                          pxTaskStatusArray[x].uxCurrentPriority,\
						                                          pxTaskStatusArray[x].usStackHighWaterMark,\
						                                          ulStatsAsPercentage);
					}
					else  //任务运行时间不足总时间的1%
					{
						sprintf(tmp, "%12s%6c%6d%8d%8d    t<1%%",pxTaskStatusArray[x].pcTaskName,\
						                                          task_state[pxTaskStatusArray[x].eCurrentState],\
						                                          pxTaskStatusArray[x].xTaskNumber,\
						                                          pxTaskStatusArray[x].uxCurrentPriority,\
						                                          pxTaskStatusArray[x].usStackHighWaterMark);
					}
					if(print){
						iot_printf("%s\r\n", tmp);
					}
				}

				
				for ( x = 0; x < uxArraySize; x++ )
				{					
					if(pxTaskStatusArray[x].xTaskNumber >= TASK_STATE_SHOW_NUM)
						continue;
					g_taskRunTimeCounter[pxTaskStatusArray[x].xTaskNumber] = pxTaskStatusArray[ x ].ulRunTimeCounter;
					g_taskTotalRunTime = ulTotalRunTime;
				}
				
			}
			kfree(tmp);
		} 
		#endif
	}
	kfree(pxTaskStatusArray);
	// RTOS_RunTimeStop();
	#endif
	
}

/*打开cpu 占用率等debug 信息*/
FLASH_FUNC void AT_SetDbgMask(char *Args)
{
    CmdLine_GetHex(&Args, &wifi_debug_mask);
	/*
	iot_printf("InpItemPut %d\n",Hif_Internal.InpItemPut);
	iot_printf("InpItemhmacPut %d\n",Hif_Internal.InpItemhmacPut);
	iot_printf("InpItemGet %d\n",Hif_Internal.InpItemGet);
	iot_printf("OutItemPut %d\n",Hif_Internal.OutItemPut);
	iot_printf("OutItemHmacGet %d\n",Hif_Internal.OutItemHmacGet);
	iot_printf("OutItemGet %d\n",Hif_Internal.OutItemGet);
	iot_printf("num_pending %d num_queued %d\n",g_hw_prv.tx_queue[2].num_pending,g_hw_prv.tx_queue[2].num_queued);
	*/
}

FLASH_FUNC void AT_OSTaskShow(char *Args)
{	
	//iot_printf("g_taskTotalRunTime %d\n",g_taskTotalRunTime);  
	rtos_state_task(0);
	hal_sleep(1000);
	rtos_state_task(1);
}

FLASH_FUNC void AT_StandbySleep(char *pLine)
{	
   //sleep infinit or timeout 
   UN_USED_PARAMS(*pLine);
   u8 enable=1;
   atbm_wifi_set_ps_mode(enable);
}

FLASH_FUNC void AT_StandbyWakeup(char *pLine){	
  //wakeup form standby sleep state
	UN_USED_PARAMS(*pLine);
	u8 enable=0;
	atbm_wifi_set_ps_mode(enable);
	UART_Send_status(0);
}

FLASH_FUNC void AT_LightSleep(char *pLine)
{	
   //sleep infinit or timeout 
	uint32 enable;
	//str = CmdLine_GetToken(&pLine);
	CmdLine_GetInteger(&pLine, &enable);
	enable = !!enable;

	iot_printf("AT_LightSleep:%d\n", enable);
	if(enable){
		hal_set_sleep_mode(HAL_LIGHT_SLEEP);
	}
	else {
		hal_set_sleep_mode(HAL_NO_SLEEP);
	}
	atbmwifi_flash_param_wificonfig_change();	

	atbm_wifi_set_ps_mode(enable);
	
	UART_Send_status(0);
}

FLASH_FUNC void AT_LightSleepWakeupGpio(char *pLine)
{	
   //sleep infinit or timeout 
	uint32 pin;
    uint32 low;
	//str = CmdLine_GetToken(&pLine);
	CmdLine_GetInteger(&pLine, &pin);
	CmdLine_GetInteger(&pLine, &low);
	
	iot_printf("light sleep gpio_wakeup_set:%d low %d\n", pin,low);
	hal_ps_gpio_wakeup_clear();
	if(hal_ps_gpio_wakeup_set(pin,!!low)<0){
		UART_Send_status(-1);
	}	
	
	UART_Send_status(0);
}

FLASH_FUNC void AT_ModemSleep(char *pLine)
{	
   //sleep infinit or timeout 
	uint32 enable;
	//str = CmdLine_GetToken(&pLine);
	CmdLine_GetInteger(&pLine, &enable);
	enable = !!enable;

	iot_printf("AT_ModemSleep:%d\n", enable);
	if(enable){
		hal_set_sleep_mode(HAL_MODEM_SLEEP);
	}
	else {
		hal_set_sleep_mode(HAL_NO_SLEEP);
	}
	atbmwifi_flash_param_wificonfig_change();	

	atbm_wifi_set_ps_mode(enable);
	
	UART_Send_status(0);
}


FLASH_FUNC void AT_DeepSleep(char *pLine)
{
#if (CHIP_TYPE == CHIP_ATHENAB)
  //enter deep sleep, infinite or timeout
  UART_Send_status(HAL_ERR_NOT_PRESENT);
#else //(CHIP_TYPE == CHIP_ARES)
	uint32 second;/*second*/
	//str = CmdLine_GetToken(&pLine);
	CmdLine_GetInteger(&pLine, &second);

	if(second > 0){
		PWR_GotoDeepSleep(second*1000);
	}
#endif //CHIP_ATHENAB
}


/*Copy backed flash boot params to boot params section */	
FLASH_FUNC void AT_RestoreFactory(char *Args)
{	
	UN_USED_PARAMS(*Args);
   //set params to default value.  
   int ret = 0;
   ret = atbmwifi_flash_param_config_restore();
   
   UART_Send_status(ret);
}

/*保存相关参数到flash中*/
FLASH_FUNC void AT_SaveFlash(char *Args)
{
	UN_USED_PARAMS(*Args);
	UART_Send_status(HAL_ERR_NOT_PRESENT);
	//
}

FLASH_FUNC void AT_GetVer(char *Args)
{	
	UN_USED_PARAMS(*Args);
	char strTmp[64] = {0};	

	CmdLine_GetPlatformVar(strTmp, sizeof(strTmp));
	UART_SendString("%s\n", strTmp);
}


extern const char SYS_SwTitle[];
FLASH_FUNC void AT_GetSysStatus(char *Args)
{	
	UN_USED_PARAMS(*Args);
	
	UART_SendString("\n\n%s\n",SYS_SwTitle);
	UART_SendString("atbm_iot SDK version %s\n",IOT_SDK_VER);
	UART_SendString("flash map size %dMbyte\n",FLASH_SIZE);	
	UART_SendString("OS	: freeRTOS\n");
	UART_SendString("wifi	: 11n 1X1 20M\n");
    AT_WifiStatus(NULL);
	UART_SendString("\ncpu	: 160Mhz\n");
	UART_SendString("memory total: 168Kbyte\n");
	UART_SendString("memory free: %d byte\n",hal_sys_mem_free_size_get());
	UART_SendString("TCPIP	: LWIP\n");
	UART_SendString("lwip config: lwipopts.h\n");
}

/*
use this AT cmd must ,get sntp  one time

*/
FLASH_FUNC void AT_GetSysTime(char *Args)
{	
    uint32 time = hal_get_current_real_time();
	if(sntp_get_current_timestamp()){
		UART_SendString("+ok=date:%s\n", sntp_get_real_time(time));
	}
	else {
		UART_Send_status(HAL_ERR_UNAVAILABLE);
		UART_SendString("you may need \"AT+SNTP_GET\" first\n", sntp_get_real_time(time));
	}
}

FLASH_FUNC void AT_Help(char *Args)
{	
	UN_USED_PARAMS(*Args);
	char strTmp[64] = {0};	

	CmdLine_GetPlatformVar(strTmp, sizeof(strTmp));
	UART_SendString("%s\n", strTmp);
}

extern uint8 u16PpaGainValTbl_20M[];
extern uint8 u16PpaGainValTbl_40M[];
//输入发射功率的强度[-60,60]
//范围是多少
extern s16 TX_POWER;
FLASH_FUNC void AT_Set_Transmit_Power(char *Args)
{
	int Power;
	int i;
	CmdLine_GetSignInteger(&Args, &Power);
	if((Power < -60) || (Power > 60))
	{
		UART_Send_status(1);
		return;
	}
	else
	{
		TX_POWER = (s16)Power;
	    PHY_RF_TPC_Cal();
	    UART_SendString("txpwr change %d/10 dB\n", Power);
	    UART_Send_status(0);
	}
}

FLASH_FUNC void AT_DCXOCode_Read(char *Args)
{
	u32 dcxo;
	dcxo = DCXOCodeRead();
	UART_SendString("DCXO = %d",dcxo);
}

//输入频偏
//范围是多少[0,127]
FLASH_FUNC void AT_DCXOCode_Write(char *Args)
{
	uint32 dcxo;
	CmdLine_GetInteger(&Args, &dcxo);
	iot_printf("dcxo = %d\n",dcxo);
	if((dcxo < 0) || (dcxo > 127))
	{
		UART_Send_status(1);
		return;
	}
	else
	{
		DCXOCodeWrite(dcxo);
		UART_Send_status(0);
	}

}

struct cli_cmd_struct ATCommands_System[] =
{
	{.cmd ="AT+HELP",			.fn = AT_Help,			 .next = (void*)0},
	{.cmd ="AT+GET_VER",		.fn = AT_GetVer,		 .next = (void*)0},
	{.cmd ="AT+GET_SYS_STATUS", .fn = AT_GetSysStatus, 	.next = (void*)0},
	{.cmd ="AT+REBOOT", 		.fn = AT_reboot,		 .next = (void*)0},
	{.cmd ="AT+RESTORE",		.fn = AT_RestoreFactory, .next = (void*)0},
	{.cmd ="AT+LIGHT_SLEEP",	.fn = AT_LightSleep,.next = (void*)0},
	{.cmd ="AT+WAKEUP_GPIO",	.fn = AT_LightSleepWakeupGpio,.next = (void*)0},
	{.cmd ="AT+DEEP_SLEEP", 	.fn = AT_DeepSleep, 	 .next = (void*)0},
	{.cmd ="AT+MODEM_SLEEP", 	.fn = AT_ModemSleep, 	 .next = (void*)0},
	{.cmd ="AT+GET_SYS_TIME",   .fn = AT_GetSysTime,	.next = (void*)0},
	
	//{.cmd ="AT+flashsave",		.fn = AT_SaveFlash,		.next = (void*)0},	/*save param to flash*/
	{.cmd ="AT+memoryShow", 		   .fn = AT_MemoryStatus,		    .next = (void*)0},
	{.cmd ="AT+taskShow",			   .fn = AT_OSTaskShow, 		    .next = (void*)0},
	{.cmd ="AT+SetDbgMask", 		   .fn = AT_SetDbgMask, 		    .next = (void*)0},
	{.cmd ="AT+SetLwipDbg", 		   .fn = AT_SetLwipDbgMask, 	    .next = (void*)0},
	{.cmd ="AT+TX_POWER",              .fn = AT_Set_Transmit_Power, 	.next = (void*)0},
	{.cmd ="AT+R_DCXO", 	           .fn = AT_DCXOCode_Read, 	        .next = (void*)0},
	{.cmd ="AT+W_DCXO", 	           .fn = AT_DCXOCode_Write, 	    .next = (void*)0},
};
FLASH_FUNC void ATCmd_init_sys(void)
{
	 cli_add_cmds(ATCommands_System,sizeof(ATCommands_System)/sizeof(ATCommands_System[0]));
 
}

