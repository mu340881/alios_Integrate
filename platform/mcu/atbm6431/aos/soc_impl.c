/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

extern void OS_Exception(int entry_num);

size_t soc_get_cur_sp()
{
    volatile size_t dummy = (size_t)&dummy;
    return dummy;
}

#if (RHINO_CONFIG_HW_COUNT > 0)
hr_timer_t soc_hr_hw_cnt_get(void)
{
    return 0;
}
#endif


void trace_start(void)
{
    printf("trace config close!!!\r\n");
}


extern void hal_reboot(void);
void soc_err_proc(kstat_t err)
{
	printf("soc_err_proc err[%d]\n",err);
	OS_Exception(0x1222);
    if ( RHINO_NO_MEM == err )
    {
        /* while mem not enought, reboot */
        hal_reboot();
    }
}

krhino_err_proc_t g_err_proc = soc_err_proc;

