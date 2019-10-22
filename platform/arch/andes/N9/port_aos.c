#include <k_api.h>

#include "atbm_hal.h"
#include "portmacro.h"

extern int printf(const char *fmt, ...);



unsigned          cpu_sr;

/* Each task maintains its own interrupt status in the critical nesting
variable. */
//static unsigned int uxCriticalNesting = 0;

void vPortEnterCritical(void);
void vPortExitCritical(void);
void vPortISRStartFirstTask( void );

extern StackType_t *pxPortInitialiseStackAos( StackType_t *pxTopOfStack, pdTASK_CODE  pxCode, void *pvParameters );

/*
 * See header file for description.
 */
// int *
AOS_FUNC_SECTION void *cpu_task_stack_init(cpu_stack_t *stack_base, size_t stack_size, void *arg,
                          task_entry_t entry)
{
	


	StackType_t *pxTopOfStack  = (StackType_t *)(stack_base + stack_size - 1);
	pxTopOfStack = ( StackType_t * ) ( ( ( int ) pxTopOfStack ) & ( ( int ) ~portBYTE_ALIGNMENT_MASK  ) ); /*lint !e923 MISRA exception.  Avoiding casts between pointers and integers is not practical.  Size differences accounted for using portPOINTER_SIZE_TYPE type. */
	
		
	return pxPortInitialiseStackAos(pxTopOfStack,entry,arg);
}



AOS_FUNC_SECTION void cpu_task_switch(void)
{
    taskYIELD();
}

AOS_FUNC_SECTION void cpu_intrpt_switch(void)
{
    taskYIELD();
}






/*
 * See header file for description.
 */
AOS_FUNC_SECTION void cpu_first_task_start(void)
{
    // set pendsv and systemtick as lowest priority ISR.
    // pendsv setting

	//enable os vTaskSwitchContext interrupt
	hal_global_int_en(DEV_INT_NUM_WIFI_SW);
	hal_global_int_dis(DEV_INT_NUM_UART2);
	hal_global_int_dis(DEV_INT_NUM_UART);
	GIE_ENABLE();


    /*******software isr*********/
    vPortISRStartFirstTask();

    /* Should not get here as the tasks are now running! */
}

void krhino_task_sched_stats_get(void);


AOS_FUNC_SECTION void vTaskSwitchContext( void )
{
#if (RHINO_CONFIG_TASK_SCHED_STATS > 0)
	krhino_task_sched_stats_get();
#endif //#if (RHINO_CONFIG_TASK_SCHED_STATS > 0)

    if (g_sched_lock[0] > 0u) {
        return;
    }

	g_active_task[0]=g_preferred_ready_task[0];
}

