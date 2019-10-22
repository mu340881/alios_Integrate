/*****************************************************************************
 *
 *            Copyright Andes Technology Corporation 2007-2008
 *                         All Rights Reserved.
 *
 *  Revision History:
 *
 *    Aug.21.2007     Created.
 *    Nov.22.2009     Add queue HAL API
 ****************************************************************************/
 /*****************************************************************************
  *
  * 		   change by altobeam.inc.
  *
  *  Revision History:
  *
  *    2018/4/6	    HAL API
  ****************************************************************************/

#ifndef _ATBM_HAL_H__
#define _ATBM_HAL_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "os_cpu.h"
#include "ae210p_defs.h"
#include "ae210p_regs.h"
#include "n12_def.h"
#include <string.h>
#include "reloc_defs.h"
#include "atbm_type.h"
//#include "debug.h"
#include "rtos-freertos/hal/hal.h"

#define SMU_SYS_ST					(0x16100000 + 0x1030)
#define SMU_SYS_ST_RESET_FLAG		BIT(3)
#define SMU_SYS_ST_MARK_DATA	    BIT(0)
#define PWR_SMU_MARK_ADDR			SMU_SYS_ST


typedef hal_mutex_t mutex_t;

#define mutex_init(_m)  hal_create_mutex(_m)
#define mutex_lock(_m)  hal_wait_for_mutex(_m,HAL_SUSPEND)
#define mutex_unlock(_m) hal_release_mutex(_m)
#define mutex_destroy(_m) hal_destroy_mutex(_m)


void hal_wdt_feed();

#if defined (ATBM6431)
#define AOS_FUNC_SECTION	  __attribute__ ((section (".EILM_REGION_CODE"))) 
#else
#define AOS_FUNC_SECTION	
#endif

#endif /* __HAL_H__ */
