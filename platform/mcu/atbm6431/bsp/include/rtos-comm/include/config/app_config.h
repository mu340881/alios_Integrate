
/*
 * App config
 */
#ifdef CFG_B2B_SIMU
#define CPU_CLK	(80 * MHz)
#define TICK_HZ	 1000
///#define UARTC_CLOCK  	(20 * MHz)	/* in XC7, 20M Hz*/
#else /*CFG_B2B_SIMU*/
#define CPU_CLK	(80 * MHz)
//#define UARTC_CLOCK  	(20 * MHz)	/* in XC7, 20M Hz define in cfg_cw1260_iot_fpga.h*/
#define TICK_HZ		100
#endif //CFG_B2B_SIMU


/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*��λbyte
*********************************************************************************************************
*/
#define TASK_TCPIP_THREAD_STACK_SIZE		(1024*2+512)
#define CONFIG_IRQ_STACK_SIZE				(1024*2)	/* Define the IRQ stack size */
#define CONFIG_HMAC_STK_SIZE 				(1024*3+512+256)
#define CONFIG_LMAC_STK_SIZE 				(1024*2+512)
#define CONFIG_MAIN_STACK_SIZE 				(1024+512)
#define CONFIG_TCPIP_APP_STK_SIZE 			(1024+512)
#define OS_TASKLET_STACK_SIZE				1024
#define CONFIG_WEB_STK_SIZE					1024
#define CONFIG_NET_REC_STK_SIZE				(512+1024)
#define CONFIG_MBEDTLS_STACK_SIZE 			(1024*7)
#define CONFIG_TCPIP_APP_HAILIN_STK_SIZE 			(1024*3)

#define CONFIG_WORK_QUEUE_NUM				4
#define CONFIG_ELOOP_TIMER_NUM				4
#define PRINTF_BUFF_LEN 					1024

/*
 *  priority  LMAC is highest priority ,1 is lowest
 */
#define CONFIG_LMAC_THREAD_PRIORITY		10
#define CONFIG_HMAC_THREAD_PRIORITY		10
#define CONFIG_TCPIP_THREAD_PRIORITY	8
#define CONFIG_HAL_THREAD_PRIORITY		5
#define CONFIG_WORK_QUEUE_PRIORITY		5
#define CONFIG_DMAD_HISR_PRIORITY		5
#define CONFIG_TCPAPP_THREAD_PRIORITY	3
#define CONFIG_MAIN_TASK_PRIORITY		3

#define configGPIO_TASK_PRIORITY		2
#define configTIMER_TASK_PRIORITY		2	/* Daemon task priority */

//#define MINI_MEMORY_MODE               0
 
/***********************************************************/
#define PUBLIC
#define PRIVATE	    static
#include "reloc_defs.h"



#define MAX_VMAC_NUM 1
/***********************************************************/

#include "lwipopts.h"

