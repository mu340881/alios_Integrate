/**************************************************************************************************************
 * altobeam iot at cmd file 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 * Permission is hereby granted for use on Altobeam IOT 6421 only,
 * in which case ,it is free to  modify it
 *
*****************************************************************************************************************/

#ifndef __APP_WIFI_H__
#define __APP_WIFI_H__


#include "hal.h"
#include "config/app_config.h"

extern hal_bh_t app_bh;
extern hal_bh_t lmac_bh;
extern hal_bh_t hmac_bh;


//GPIOName_InternalChannelNum
#define GPIO3_CHANNEL_0  0 
#define GPIO2_CHANNEL_1  1
#define GPIO4_CHANNEL_2  2
#define GPIO1_CHANNEL_3  3
#define INVALID_CHANNEL_NUM 0xff






#define OS_HMACTASK_HOLD(__timeout) hal_pend_semaphore(&hmac_bh.sem, __timeout);//HAL_SUSPEND
#define OS_LMACTASK_HOLD(__timeout) hal_pend_semaphore(&lmac_bh.sem, __timeout);//HAL_SUSPEND
#define OS_APPTASK_HOLD(__timeout) 	hal_pend_semaphore(&app_bh.sem, __timeout);//HAL_SUSPEND
#define OS_LMAC_SCHED_IRQ()			hal_raise_bh(&lmac_bh)
#define OS_HMAC_SCHED_IRQ()			hal_raise_bh(&hmac_bh)
#define OS_APP_SCHED_IRQ()			hal_raise_bh(&app_bh)
#define OS_HMAC2LMAC_TRY_SCHED(__timeout)			{ hal_raise_bh(&lmac_bh);  OS_HMACTASK_HOLD(__timeout);}
//#define OS_HMAC2APP_TRY_SCHED(__timeout)			{ hal_raise_bh(&app_bh);   OS_HMACTASK_HOLD(__timeout);}
#define OS_LMAC2HMAC_TRY_SCHED(__timeout)			{ hal_raise_bh(&hmac_bh);  OS_LMACTASK_HOLD(__timeout);}
#define OS_APP2HMAC_TRY_SCHED(__timeout)			{ hal_raise_bh(&hmac_bh);  OS_APPTASK_HOLD(__timeout);}
#define OS_APP_TRY_SCHED(__timeout)				{OS_APP_SCHED_IRQ();hal_sleep(__timeout);}
#define OS_HMAC_TRY_SCHED(__timeout)			{OS_LMAC_SCHED_IRQ();hal_sleep(__timeout);}
#define OS_HMAC2LMAC_SCHED_FORCE(__timeout) 	OS_HMAC2LMAC_TRY_SCHED(__timeout)
#define OS_APP2HMAC_SCHED_FORCE(__timeout) 		OS_APP2HMAC_TRY_SCHED(__timeout)
#define OS_LMAC2HMAC_SCHED(__timeout)			OS_LMAC2HMAC_TRY_SCHED(__timeout)
//#define OS_TASK_HOLD(__timeout)						hal_sleep(__timeout)

extern 	struct netif * g_wifinetif[MAX_VMAC_NUM];
#endif //__APP_WIFI_H__
