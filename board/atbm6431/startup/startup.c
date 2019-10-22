/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "aos/init.h"
#include "board.h"
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <hal/wifi.h>
#include <aos/aos.h>
#include <k_api.h>
#include <aos/kernel.h>


/*
main task stask size(byte)
*/
#define OS_MAIN_TASK_STACK 0x400/2
#define OS_MAIN_TASK_PRI 32

/*  For user config
    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 1;
*/
static kinit_t kinit = {
    .argc = 0,
    .argv = NULL,
    .cli_enable = 1
};

extern void APP_Init(void);
extern void board_init(void);

extern hal_wifi_module_t sim_aos_wifi_icomm ;
extern void hal_uart_dbg_open(void);
extern void atbm_wifi_not_connect_ap_auto();
extern void hal_uart_dbg_close(void);
extern int32_t hal_uart_init(void *uart);

#ifndef AOS_BINS
extern int application_start(int argc, char **argv);
#endif

static void sys_init(void)
{
	int ret;
    /* user code start*/
	hal_uart_dbg_open();
	//printf("sys_init\n");
	hal_uart_init(NULL);

    hal_wifi_register_module(&sim_aos_wifi_icomm);
    ret = hal_wifi_init();
    if (ret){
        printf("waring: wifi init fail ret is %d \r\n", ret);
    }

    /*insert driver to enable irq for example: starting to run tick time.
     drivers to trigger irq is forbidden before aos_start, which will start core schedule.
    */
    /*user_trigger_irq();*/  //for example
    aos_kernel_init(&kinit);

}

static ktask_t *g_main_task;
void aos_start_os(void)  
{	
	atbm_wifi_not_connect_ap_auto();

    /*main task to run */
    krhino_task_dyn_create(&g_main_task, "aos-init", 0, OS_MAIN_TASK_PRI, 0, OS_MAIN_TASK_STACK, (task_entry_t)sys_init, 1);
	
    /*kernel start schedule!*/
    krhino_start();
	return;
}

int aos_main(void)
{
    /*irq initialized is approved here.But irq triggering is forbidden, which will enter CPU scheduling.
    Put them in sys_init which will be called after aos_start.
    Irq for task schedule should be enabled here, such as PendSV for cortex-M4.
    */
    board_init();   //including aos_heap_set();  flash_partition_init();

	
	hal_uart_dbg_close();
    APP_Init();

    /*never run here*/
    return 0;
}


