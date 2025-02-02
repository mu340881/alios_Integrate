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


#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------
 * Port specific definitions.  
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short
#define portSTACK_TYPE		unsigned portLONG
#define portBASE_TYPE		portLONG

typedef portSTACK_TYPE StackType_t;
typedef portLONG BaseType_t;
typedef unsigned portLONG UBaseType_t;

#if( configUSE_8_BIT_TICKS == 1 )	
	typedef unsigned char  TickType_t;
	#define portMAX_DELAY ( portTickType ) 0xff
#elif( configUSE_16_BIT_TICKS == 1 )
	typedef unsigned portSHORT TickType_t;
	#define portMAX_DELAY ( portTickType ) 0xffff
#else
	typedef unsigned portLONG TickType_t;
	#define portMAX_DELAY ( portTickType ) 0xffffffff
#endif
/*-----------------------------------------------------------*/	

/* Architecture specifics. */
#define portSTACK_GROWTH		( -1 )
//#define portTICK_RATE_MS		( ( portTickType ) 1000 / configTICK_RATE_HZ )		
#define portTICK_PERIOD_MS             ( ( portTickType ) 1000 / configTICK_RATE_HZ )
#define portBYTE_ALIGNMENT		32
#if( portBYTE_ALIGNMENT == 32 || portBYTE_ALIGNMENT == 16)
	#define portBYTE_ALIGNMENT_MASK ( portBYTE_ALIGNMENT-1 )
#endif


#define portNOP()			asm volatile ( "nop" );
/*-----------------------------------------------------------*/	

#include "os_cpu.h"
extern OS_CPU_SR psw_1;

/* Scheduler utilities. */
#define portRESTORE_CONTEXT		CtxRestore		
#define portSAVE_CONTEXT		CtxSave

void vPortYield();
#define portYIELD()			vPortYield()

/* Critical section management. */

#define portDISABLE_INTERRUPTS()	do{ GIE_SAVE( &psw_1);} while(0)
#define portENABLE_INTERRUPTS()		do{ GIE_RESTORE(psw_1);} while(0)

void vPortEnterCritical();
void vPortExitCritical();

#define portENTER_CRITICAL()     	vPortEnterCritical()
#define portEXIT_CRITICAL()     	vPortExitCritical()

/* Kernel_API_From_ISR barrier */
/* If is the priority interrupt, should changed into mask & unmask */
/* Since Fixed priority interrupt, just disable interrupt */
extern unsigned long vPortSetInterruptMask();
extern void vPortClearInterruptMask( unsigned long ulNewMaskValue );
#define portSET_INTERRUPT_MASK_FROM_ISR()	vPortSetInterruptMask()	
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x)	vPortClearInterruptMask(x)

/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */

