/*
    FreeRTOS V8.0.0 - Copyright (C) 2014 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that has become a de facto standard.             *
     *                                                                       *
     *    Help yourself get started quickly and support the FreeRTOS         *
     *    project by purchasing a FreeRTOS tutorial book, reference          *
     *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
     *                                                                       *
     *    Thank you!                                                         *
     *                                                                       *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    >>! NOTE: The modification to the GPL is included to allow you to distribute
    >>! a combined work that includes FreeRTOS without being obliged to provide
    >>! the source code for proprietary components outside of the FreeRTOS
    >>! kernel.

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available from the following
    link: http://www.freertos.org/a00114.html

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org - Documentation, books, training, latest versions,
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/


/* Scheduler includes. */
#include "atbm_hal.h"
//#include "task.h"

#include "os_cpu.h"
#include "portmacro.h"
#include <k_api.h>
#include "app_flash_param.h"

/* Constants required to handle critical sections. */
/*-----------------------------------------------------------*/

void vPortISRStartFirstTask( void );
void atbm_hal_reset_cpu();
/*-----------------------------------------------------------*/


AOS_FUNC_SECTION void vPortISRStartFirstTask( void )
{
	/* Simply start the scheduler. */
	portRESTORE_CONTEXT();
}
/*-----------------------------------------------------------*/

/* 
 * The ISR used for the scheduler tick depends on whether the cooperative or
 * the preemptive scheduler is being used.
 */


/**
* @System tick handler example.
shoule include:
  krhino_intrpt_enter();
  krhino_tick_proc();
  krhino_intrpt_exit();
*/




/*-----------------------------------------------------------*/
#define MAX_WDT_TIME 3
u32 lastfeed_wdt_time[MAX_WDT_TIME];
bool lastfeed_wdt_time_en[MAX_WDT_TIME]={0};
AOS_FUNC_SECTION void wdt_pit_feed(u8 id)
{
	if(id>=MAX_WDT_TIME)
		return;
			
//	printf("wdt_pit_feed %d\n",id);
	lastfeed_wdt_time[id]=g_tick_count;
}
AOS_FUNC_SECTION void wdt_pit_feed_en(u8 id)
{
	if(id>=MAX_WDT_TIME)
		return;
			
	printf("wdt_pit_feed_en %d\n",id);
	lastfeed_wdt_time_en[id]=1;
}



typedef void (*RunFunc)(void);

AOS_FUNC_SECTION void vApplicationTickHook( void )
{
	u32 nowtick = g_tick_count;	
	//RunFunc flashfunc= NULL;

	
  	if((s32)(nowtick-lastfeed_wdt_time[0]) > (20*RHINO_CONFIG_TICKS_PER_SECOND)){
		printf("LMAC_WDT timeout, restart CPU\n");		
		goto __retart_cpu;
	}
  	if((s32)(nowtick-lastfeed_wdt_time[1]) > (20*RHINO_CONFIG_TICKS_PER_SECOND)){		
		printf("HMAC_WDT call, restart CPU\n");
		goto __retart_cpu;
	}
	
  	if(lastfeed_wdt_time_en[2] && ((s32)(nowtick-lastfeed_wdt_time[2]) > (20*RHINO_CONFIG_TICKS_PER_SECOND))){		
		printf("WDT3_WDT call, restart CPU\n");
		goto __retart_cpu;
	}
	return;
__retart_cpu:
	//may need add phy reset,mac reset,and so o
	atbm_hal_reset_cpu();
}


AOS_FUNC_SECTION void Wdt_feed_check()
{
	static int wdt_feed_cnt =0;
	//800ms feed wdt
	if(wdt_feed_cnt++ > 30){
		hal_wdt_feed();
		wdt_feed_cnt= 0;
	}
	vApplicationTickHook();
		
}

AOS_FUNC_SECTION 	void vPreemptiveTick( void )
{		 
	Wdt_feed_check();
	krhino_intrpt_enter();
	krhino_tick_proc();
	krhino_intrpt_exit();
}

/*-----------------------------------------------------------*/
