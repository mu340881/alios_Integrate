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


/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "atbm_hal.h"
//#include "task.h"
/* Processor constants. */
#include <k_api.h>
#include "os_cpu.h"
#include "portmacro.h"
//#include "lmac_os.h"
//#include "ti.h"
//#include "platform.h"
#include "app_flash_param.h"

/* Constants required to setup the task context. */
#define portNO_CRITICAL_SECTION_NESTING	( ( portSTACK_TYPE ) 0 )

/*-----------------------------------------------------------*/

extern  struct flash_boot_volatile_section iot_boot_volatile_sect;
extern  struct flash_boot_const_section iot_boot_const_sect;

extern void vPortISRStartFirstTask( void );
void vPortEndScheduler( void );

#define OLD_VER 0

/*-----------------------------------------------------------*/

/*
 * Initialise the stack of a task to look exactly as if a call to
 * portSAVE_CONTEXT had been called.
 *
 * See header file for description.
 *
 *
 * Stack Layout:
 *		  High  |-----------------|
 *                      |       $R1       |
 *                      |-----------------|
 *                      |       $R0       |
 *                      |-----------------|
 *                      |       $R30 (LP) |
 *                      |-----------------|
 *                      |       $R29 (GP) |
 *                      |-----------------|
 *                      |       $R28 (FP) |
 *                      |-----------------|
 *                      |   $R15   $R27   |
 *                      |-----------------|
 *                      |   $R10   $R26   |
 *                      |-----------------|
 *                      |        .        |
 *                      |        .        |
 *                      |-----------------|
 *                      |       $R2       |
 *                      |-----------------|
 *                      |       $d0.hi    |
 *                      |          .lo    |
 *                      |-----------------|
 *                      |       $d1.hi    |
 *                      |          .lo    |
 *                      |-----------------|
 *                      |       $IFC_LP   |
 *                      |-----------------|
 *                      |       $IPSW     |
 *                      |-----------------|
 *                      |       $IPC      |
 *                      |-----------------|
 *                      |       $PSW      |
 *                      |-----------------|
 *                      |       $SP       |
 *                      |-----------------|
 *                      |       $SP       |  (8-byte alignment)
 *                      |-----------------|
 *                      |       $FPU      |
 *                      |-----------------|
 *                Low
 *
 */
/* For relax support, must initial $gp at task init*/
extern INT32U _SDA_BASE_ __attribute__ ((weak));

AOS_FUNC_SECTION portSTACK_TYPE *pxPortInitialiseStackAos( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE  pxCode, void *pvParameters )
{
	int i;
	//portSTACK_TYPE *pxOriginalTOS;
	portSTACK_TYPE *old_stk;


	//pxOriginalTOS = pxTopOfStack;

	/* Setup the initial stack of the task.  The stack is set exactly as
	expected by the portRESTORE_CONTEXT() macro. */

	/* First on the stack is the return address - which in this case is the
	start of the task.  The offset is added to make the return address appear
	as it would within an IRQ ISR. */
        *--pxTopOfStack = (INT32U)0x01010101L * 1;			/* R1 */
        *--pxTopOfStack = (portSTACK_TYPE)pvParameters;           	/* R0 : argument */
        *--pxTopOfStack = (INT32U)vPortEndScheduler;             	/* $30:LP */
        *--pxTopOfStack = (INT32U)&_SDA_BASE_;                   	/* $29:GP For relax support */
        *--pxTopOfStack = (INT32U)0x01010101UL * 28;			/* $28:FP */
	#ifdef __NDS32_REDUCE_REGS__
        *--pxTopOfStack = (INT32U)0x01010101UL * 15;
	i = 10;
	#else
	i = 27;
	#endif
        for (; i >= 2; i--)
		*--pxTopOfStack = (INT32U)0x01010101UL * i;
	#if (defined(__NDS32_ISA_V2__) && defined(__NDS32_DX_REGS__)) || (!defined(__NDS32_ISA_V2__) && (defined(__NDS32_DIV__)||defined(__NDS32_MAC__)))
	for (i = 35; i >= 32; i--)                      	  	/* d0.hi, d0.lo, d1.hi, d1.lo */
		*--pxTopOfStack = (INT32U)0x01010101UL * i;
	#endif

#ifdef __NDS32_EXT_IFC__
# ifndef CONFIG_NO_NDS32_EXT_IFC
		*--pxTopOfStack = (INT32U)0x01010101UL * 40;		/* $IFC_LP */
# endif
#endif


	/* IPSW, enable GIE, clear IFCON */
        *--pxTopOfStack = (portSTACK_TYPE)((GET_PSW() | (1UL << PSW_offGIE)) & (~(1UL << PSW_offIFCON)));
	/* IPC */
        *--pxTopOfStack = (portSTACK_TYPE)pxCode;
	/* PSW */
        *--pxTopOfStack = (portSTACK_TYPE)(GET_PSW() | (1UL << PSW_offINTL) );
       
       old_stk = pxTopOfStack;                                	  	/* Save curent unaligned sp */
        *--pxTopOfStack = (portSTACK_TYPE)((INT32U)old_stk);    	/* sp(R31) */
        if ((INT32U)pxTopOfStack & 0x7)
                *--pxTopOfStack = (portSTACK_TYPE)((INT32U)old_stk);    /* sp(R31) */
	

	/* push fpu */
#if defined(__NDS32_EXT_FPU_DP__) || defined(__NDS32_EXT_FPU_SP__)
	int start, end;
#if defined(__NDS32_EXT_FPU_CONFIG_0__)
        end = 36; start = 35 + 2 * 4;
#elif defined(__NDS32_EXT_FPU_CONFIG_1__)
        end = 36; start = 35 + 2 * 8;
#elif defined(__NDS32_EXT_FPU_CONFIG_2__)
        end = 36; start = 35 + 2 * 16;
#elif defined(__NDS32_EXT_FPU_CONFIG_3__)
        end = 36; start = 35 + 2 * 32;
#else
	end = 1; start = 0;
#endif
	for (i = start; i >= end; i--)
		*--pxTopOfStack = (INT32U)0x01010101UL * i;
#endif


	return pxTopOfStack;

}

/*-----------------------------------------------------------*/

AOS_FUNC_SECTION portBASE_TYPE xPortStartScheduler( void )
{
	/* Start the first task. */
	vPortISRStartFirstTask();

	/* Should not get here! */
	return 0;
}
/*-----------------------------------------------------------*/
AOS_FUNC_SECTION void vPortEndScheduler( void )
{
	//iot_printf("Current Task will be deleted\n");
	hal_destroy_thread(NULL);
	//vTaskDelete(NULL);
}
#include "hal.h"
AOS_FUNC_SECTION void vPortYield()
{   
	if (g_sched_lock[0] > 0u) {
        return;
    }
     /* trigger swi*/
    __nds32__mtsr(0x10000,NDS32_SR_INT_PEND);
    __nds32__isb();
}
#define portNO_CRITICAL_NESTING		( ( unsigned long ) 0 )
DTCM_ZI volatile unsigned long ulCriticalNesting = 0UL;
//DTCM_ZI volatile unsigned long ulCriticalBHNesting = 0UL;
static DTCM_ZI OS_CPU_SR psw_0 = 0;
DTCM_ZI OS_CPU_SR psw_1 = 0;
AOS_FUNC_SECTION void vPortEnterCritical()
{
       /*
        *       in order to avoid race condition
        *       1.store psw into stack
        *       2.disable gie
        *       3.store psw into global if ulCriticalNesting==0
        *       4.ulCriticalNesting++ 
        */
       volatile unsigned int psw = __nds32__mfsr(NDS32_SR_PSW);
       GIE_DISABLE();
       if (ulCriticalNesting == 0 )
               psw_0 = psw;
       ulCriticalNesting++;

#if 0
	GIE_SAVE( &psw_0);
	ulCriticalNesting++;
#endif
}

AOS_FUNC_SECTION void vPortExitCritical()
{

       --ulCriticalNesting;
       if ( ulCriticalNesting == 0 )
               GIE_RESTORE(psw_0);
//     GIE_RESTORE(psw_0);

#if 0
        if( ulCriticalNesting > portNO_CRITICAL_NESTING )
        {
        	/* Decrement the nesting count as we are leaving a critical section. */
		ulCriticalNesting--;
                /* If the nesting level has reached zero then interrupts should be re-enabled. */			                    if( ulCriticalNesting == portNO_CRITICAL_NESTING )
		{
			GIE_RESTORE(psw_0);
		}
	}
#endif
}
/*
void vPortEnterBHCritical()
{
	disable_interrupt(DEV_INT_NUM_PIT);
	ulCriticalBHNesting++;
}
void vPortExitBHCritical()
{
	--ulCriticalBHNesting;
	if ( ulCriticalBHNesting == 0 )
		enable_interrupt(DEV_INT_NUM_PIT);
}
*/
/*close intr*/
AOS_FUNC_SECTION inline unsigned long vPortSetInterruptMask( void )
{
	unsigned long psw = __nds32__mfsr(NDS32_SR_PSW);
	GIE_DISABLE();
	return psw;
}

/*open intr*/
AOS_FUNC_SECTION inline void vPortClearInterruptMask( unsigned long psw )
{
	GIE_RESTORE(psw);
}
#ifdef LIGHT_SLEEP

/* 配置一个中断，当指定的睡眠时间达到后，将处理器从低功耗中唤醒。这个中断源必须在微控制器进入低功耗时也可以工作。*/
/*进入低功耗 */
AOS_FUNC_SECTION void vSetWakeTimeRtc(TickType_t xExpectedIdleTime )
{
	unsigned long ulLowPowerTimeforSleep;
    //prvSleep();  
    ulLowPowerTimeforSleep = PWR_GetSleepedTime();
}


static unsigned int AtbmSleepXCnt = 0;
static unsigned int AtbmSleepXTick = 0;
static unsigned int AtbmSleepWakeupTime = 1000;/*us*/

/* 定义被宏portSUPPRESS_TICKS_AND_SLEEP()调用的函数 */
uint32 PowerBusyEvents_bak = 0;
//used for ares light sleep,not sleep when initial
extern 	bool g_os_init_done;

extern uint32 System_timer_TimeToNextTimeout(void);
extern void  System_timer_PowerResume(void);
extern  void * ITCM2 TI_NextCall(void);
extern  uint32 ITCM2 TI_NextTimeout(void);
extern  void * ITCM2 System_timer_NextCall(void);
extern u32 SysPowerBusyEvents;
uint32 NextTimeOut = 0;
int gInPowerSave = 0;
AOS_FUNC_SECTION unsigned char AtbmCanGotoSleep(void)
{
	uint32   IState;
	unsigned char CanSleep = 0;
	if(hal_ps_suspend_early()==0)
		return 0;
	IState = OS_DisableFiqIrq();
	CanSleep = !!((sPlatformGlobal.SchedulerEvents <= SYS_EVENT_POWER_CONTROL)&&
		       (sPlatformGlobal.PowerBusyEvents == 0)&&
		       (SysPowerBusyEvents==0));
	OS_RestoreFiqIrq(IState);
	return CanSleep;
}

#if (CHIP_TYPE == CHIP_ATHENAB)

AOS_FUNC_SECTION inline void vPortCloseTickInterrupt( void )
{
	HAL_TIMER_IRQ_ENABLE(USE_TICKET_ID, 0);
}
AOS_FUNC_SECTION inline void vPortOpenTickInterrupt( void )
{
	HAL_TIMER_IRQ_ENABLE(USE_TICKET_ID, 1);
}

#define PROGRAM_HW_TIMER(tmr,intv) do {\
		HAL_TIMER_STOP(tmr); \
		HAL_TIMER_SET_PERIOD(tmr, intv*(APB_CLK/MHz));\
		HAL_TIMER_IRQ_ENABLE(tmr, 1);\
		HAL_TIMER_START(tmr);\
}	while(0)

/* 配置一个中断，当指定的睡眠时间达到后，将处理器从低功耗中唤醒。这个中断源必须在微控制器进入低功耗时也可以工作。*/
/*进入低功耗 */
AOS_FUNC_SECTION uint32 vSetWakeTimePit(uint32 SleepTime)
{
	//iot_printf("vSetWakeTimePit++ %d *tick\n",xExpectedIdleTime);
	/* Start Timer ticket*/ 
	PROGRAM_HW_TIMER(USE_LIGHTSLEEP_ID, SleepTime);
    CPU_Sleep(); 
	//iot_printf("vSetWakeTimePit-- %d\n",xExpectedIdleTime);
	HAL_TIMER_STOP(USE_LIGHTSLEEP_ID);
	return 0;
}

void AtbmGotoSleeep(uint32 u32miniSleepTime)
{   
	uint32 intMaskBak=0;
	uint32 SleepXTick = 0;
    unsigned long ulLowPowerTimeBeforeSleep;
	unsigned long ulLowPowerTimeAfterSleep;  
	
    unsigned long ulTickTimeBefore;
	unsigned long ulTickTimeAfter;  
	
    unsigned long ulDeltaTime;

	ulLowPowerTimeBeforeSleep = lmac_Time();  
	ulTickTimeBefore = HAL_TICK_TIME_US();
	ulDeltaTime = ulLowPowerTimeBeforeSleep-ulTickTimeBefore;
		
	
	/*停止系统节拍时钟中断。*/
	vPortCloseTickInterrupt();	
	
	// disable all interrupt source - PHY,PAC timers, ...
	/*关闭所有中断*/

	AtbmSleepXCnt++;
	intMaskBak = HW_READ_REG(SMU_INTR_MASK);
	HW_WRITE_REG(SMU_INTR_MASK, ~(BIT(PIT_DEV_INT_NUM)|BIT(DEV_INT_NUM_UART)|BIT(DEV_INT_NUM_UART2)));

	gInPowerSave = 1;
	HW_WRITE_REG(PWR_SMU_HW_CTRL_ST,0x0);//set hw_sw_already to 0
	
	SYS_PowerSuspend();

	//goto sleep
	/* 配置一个中断，当指定的睡眠时间达到后，将处理器从低功耗中唤醒。这个中断源必须在微控制器进入低功耗时也可以工作。*/
	vSetWakeTimePit( u32miniSleepTime ); 		

	SYS_PowerResume();
	System_timer_PowerResume();


	/*calibrate the tick time*/
	ulLowPowerTimeAfterSleep = lmac_Time(); 
	ulTickTimeAfter = ulLowPowerTimeAfterSleep - ulDeltaTime;
	//DBG_Printf("tk:%d\n",(ulLowPowerTimeAfterSleep-ulLowPowerTimeBeforeSleep)/1000);
	//DBG_Printf("sleepT:%d Bf:%d, Af:%d, tk:%d ms\n",u32miniSleepTime,ulTickTimeBefore,ulTickTimeAfter,ulTickTimeAfter/1000-ulTickTimeBefore/1000);
	SleepXTick = (ulTickTimeAfter/OS_TICKER_INTERVEL - ulTickTimeBefore/OS_TICKER_INTERVEL);
	/*调整内核系统节拍计数器。*/
	vTaskStepTick(SleepXTick); 
	AtbmSleepXTick += SleepXTick;
	//Console_SetPolling(0);
	///iot_printf("SleepXTick %d %d %d\n",SleepXTick,xTaskGetTickCount(),xTaskGetTickOverflowCnt());
	// If we have any interrupts
	HW_WRITE_REG(0x16100008, intMaskBak);

	/*重新启动系统节拍时钟中断。*/
	vPortOpenTickInterrupt(); 
}
#else //#if (CHIP_TYPE == CHIP_ARES)

AOS_FUNC_SECTION void AtbmGotoSleeep(uint32 u32miniSleepTime)
{   
	uint32 intMaskBak=0;
	uint32 SleepXTick = 0;
	unsigned long ulTickTimeAfter;  

			
	AtbmSleepXCnt++;
	// disable all interrupt source - PHY,PAC timers, ...
	intMaskBak = HW_READ_REG(SMU_INTR_MASK);
	HW_WRITE_REG(SMU_INTR_MASK, ~(BIT(DEV_INT_NUM_PMU)|BIT(DEV_INT_NUM_UART)|BIT(DEV_INT_NUM_UART2)));

	gInPowerSave = 1;
	HW_WRITE_REG(PWR_SMU_HW_CTRL_ST,0x0);//set hw_sw_already to 0
	
	SYS_PowerSuspend();
	//u32miniSleepTime = u32miniSleepTime -AtbmSleepWakeupTime;

	//goto sleep
	PWR_GotoLightSleep(u32miniSleepTime);

	//wakeuop
	lmac_Wakeup();
	System_timer_PowerResume();
	hal_ps_resume();
	//HAL_TIME_US is hw+sw wakeup time
	AtbmSleepWakeupTime += HAL_TIME_US();
	AtbmSleepWakeupTime =AtbmSleepWakeupTime /2;
	ulTickTimeAfter = sPlatformLocal.RTCSleepTime+HAL_TIME_US();//PWR_GetSleepedTime();

	//Console_SetPolling(1);
	iot_printf("SleepedTime %d ,tim %d  %d delta %d SlepTime %d\n",ulTickTimeAfter, HAL_TIME_US(),lmac_Time(),NextTimeOut?(lmac_Time()-NextTimeOut):0,u32miniSleepTime);
	//iot_printf("SleepedTime %d ,tim %d,%d\n",ulTickTimeAfter, HAL_TIME_US(),HW_READ_REG(PWR_RTC2_EN));
	OS_Ticket_Init();

	SleepXTick = (ulTickTimeAfter/OS_TICKER_INTERVEL);

	vTaskStepTick(SleepXTick); 
	AtbmSleepXTick += SleepXTick;
	
	// If we have any interrupts
	HW_WRITE_REG(SMU_INTR_MASK, intMaskBak);
	//vPortOpenTickInterrupt(); 
}

#endif //#if (CHIP_TYPE == CHIP_ARES)

AOS_FUNC_SECTION void vApplicationLightSleep(TickType_t xExpectedIdleTime )  
{
	
	if((g_os_init_done==0) || (iot_boot_volatile_sect.wifi_param.system_sleep_mode != HAL_LIGHT_SLEEP)){
		//iot_printf("system_sleep_mode %d\n",iot_boot_volatile_sect.wifi_param.system_sleep_mode);
		return;
	}
	
#if 1
	eSleepModeStatus eSleepStatus;
	//unsigned long ulLowPowerTickTimeAfter;  	
	uint32 u32miniSleepTime;
	uint32 u32IdleTime;
    uint32 SleepTime;
    uint32 hmac_SleepTime;
	uint32 IState;
	

	eSleepStatus = eTaskConfirmSleepModeStatus();
	if(eSleepStatus == eAbortSleep)
	{
		//DBG_Printf("eAbortSleep\n");
	}
	else if(eSleepStatus == eNoTasksWaitingTimeout)
	{
	}
	else {
		//iot_printf("SchedulerEvents %x %x  %x\n",sPlatformGlobal.SchedulerEvents,sPlatformGlobal.PowerBusyEvents,SysPowerBusyEvents);
		//if(PowerBusyEvents_bak != sPlatformGlobal.PowerBusyEvents)
		//{
		//	DBG_Printf("idle: 0x%x\n", sPlatformGlobal.PowerBusyEvents);
		//}
		//PowerBusyEvents_bak = sPlatformGlobal.PowerBusyEvents;

		//AtbmCanGotoSleep function will check PowerBusyEvents && SchedulerEvents
		if (AtbmCanGotoSleep())
		//if ( (sPlatformGlobal.SchedulerEvents <= SYS_EVENT_POWER_CONTROL) &&  (sPlatformGlobal.PowerBusyEvents == 0))		
		{	
			u32IdleTime = xExpectedIdleTime*OS_TICKER_INTERVEL;
			//u32IdleTime = u32IdleTime>5000?u32IdleTime-5000:u32IdleTime;
#if ENABLE_WDT
			os_wdt_disable();
#endif
			IState = OS_DisableFiqIrq();
			SleepTime = TI_TimeToNextTimeout();
			NextTimeOut = TI_NextTimeout();
			hmac_SleepTime = System_timer_TimeToNextTimeout();
			OS_RestoreFiqIrq(IState);
			
			u32miniSleepTime = MIN(SleepTime,u32IdleTime);
			u32miniSleepTime = MIN(hmac_SleepTime,u32miniSleepTime);			
			
			if (u32miniSleepTime > HW__LIGHT_WAKEUP_TIME)      
			{  
				
				{	
					IState = OS_DisableFiqIrq();
					//iot_printf("PowerBusyEvents %x g_os_init_done %d\n",sPlatformGlobal.PowerBusyEvents,g_os_init_done);

					{
						//Console_SetPolling(1);
						if(SleepTime== u32miniSleepTime){
							//iot_printf("lmac_timer %x %x\n",TI_NextCall(),HW_READ_REG(PWR_RTC2_EN));
						}
						else if(hmac_SleepTime== u32miniSleepTime){
							//iot_printf("hmac_timer %x\n",System_timer_NextCall());
							NextTimeOut = 0;
						}
						else if(u32IdleTime== u32miniSleepTime){
							//iot_printf("sys_timer %d\n",xTaskGetTickCount());
							NextTimeOut = 0;
						}
						else {
							//iot_printf("unk_timer \n");
							NextTimeOut = 0;
						}
						//Console_SetPolling(0);
						AtbmGotoSleeep(u32miniSleepTime);
					}
//POWER_OUT:
					OS_RestoreFiqIrq(IState);

				}
				
				//DBG_Printf("Sleep cancelled: %x\n", sPlatformGlobal.SchedulerEvents );
			}
			else
			{
				/* A sort timer is identified, TI_ProcessTimers() will clear the event */
				lmac_SetPowerBusyEvent(SYS_PWR_EVENT_SHORT_TIMER);
			}
			os_wdt_Resume();
		}

	}

	
	gInPowerSave = 0;

#endif
}  



/* 定义被宏portSUPPRESS_TICKS_AND_SLEEP()调用的函数 */
void vApplicationDeepSleep(TickType_t xExpectedIdleTime )  
{
}
#endif //LIGHT_SLEEP
