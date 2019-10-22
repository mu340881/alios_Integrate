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

#ifndef __HAL_H__
#define __HAL_H__

#ifndef _ATBM_HAL_H__
#include "stdlib.h"
#include "stdio.h"

#include "os_cpu.h"

#include "ae210p_defs.h"
#include "ae210p_regs.h"


#include "n12_def.h"

#include "debug.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include <string.h>
#include "type.h"
#include "reloc_defs.h"
#endif  //_ATBM_HAL_H__

//#define SEMAPHORE_DEBUG
typedef struct hal_semaphore {

	void	*obj;
#ifdef SEMAPHORE_DEBUG	
	u8      name[4];
#endif  //SEMAPHORE_DEBUG
} hal_semaphore_t;

typedef struct hal_mutex {

	void	*obj;
#ifdef SEMAPHORE_DEBUG	
	u8		name[4];
#endif  //SEMAPHORE_DEBUG

} hal_mutex_t;

typedef struct hal_queue {
	void	*msg;
	unsigned int  size;
	unsigned int	count;
	unsigned int  timeout;
	void	*event;
} hal_queue_t;


typedef struct hal_thread {

	void		(*fn)(void *);
	void		*arg;
	unsigned char		prio;
	void		*ptos;		/* high_address should be assigned to NULL 
					   if you want kernel to allocate stack. */
	unsigned int		stack_size;	/* in bytes */
	char		*name;
	void		*task;

} hal_thread_t;

typedef struct hal_bh {

	hal_semaphore_t	sem;
	hal_thread_t	th;

} hal_bh_t;


struct hal_spin_lock_s{
	unsigned int spin_cnt;	
	unsigned int wait_spin_cnt;
	void	*  xSemaphore;
};

typedef struct hal_timer {

	/*If the timer is successfully created then "obj" pointer to the newly created timer.*/
	void	*obj;
	
	/*A text name that is assigned to the timer.This is done purely to assist debugging.
	 *The kernel itself only ever references a timer by its handle, and never by its name.*/
	char	*name;
	
	/*The function to call when the timer expires.*/
	void	(*fn)(void *);
	
	/*An identifier that is assigned to the timer being created.
	 *Typically this would be used in the timer callback function to identify which
	 *timer expired when the same callback function is assigned to more than one timer.*/
	unsigned int timer_id;
	
	/*If auto_reload is set to TRUE then the timer will expire repeatedly
	 *with a frequency set by the xTimerPeriodInTicks parameter.
	 *If auto_reload is set to FALSE then the timer will be a one-shot timer and
	 *enter the dormant state after it expires.*/
	unsigned int auto_reload;
	
	/*The timer period. The time is defined in tick periods.*/
	unsigned int timer_period;
} hal_timer_t;

typedef struct hal_64bit
{
	unsigned int ui32Low;
	unsigned int ui32High;
}HAL_64BIT;

#define hal_memset			os_memset
#define hal_memzero(a,b,l)	os_memzero(a,b,l)
#define hal_memcpy			os_memcpy
#define hal_memcpy32(a,b,l)	os_memcpy32(a,b,l)
#define hal_zalloc			os_zalloc
#define hal_malloc			os_malloc
#define hal_free(p)			do{os_free(p);p=NULL;}while(0)
#define hal_memcmp			os_memcmp

#define HAL_DISABLE_INTERRUPTS		0
#define HAL_ENABLE_INTERRUPTS		1

#define HAL_NULL			0
#define HAL_TRUE			1
#define HAL_FALSE			0

#define HAL_SUSPEND			-1 /* wait for object timeout */
#define HAL_DEL_NO_PEND              	0
#define HAL_DEL_ALWAYS               	1

#define HAL_SUCCESS			0
#define HAL_FAILURE			-1

#define HAL_ERR_UNHANDLED_INTERRUPT	-1
#define HAL_ERR_INVALID_POINTER		-2
#define HAL_ERR_NOT_PRESENT		-3
#define HAL_ERR_UNAVAILABLE		-4
#define HAL_ERR_TIMEOUT			-5
#define HAL_ERR_NO_MEMORY		-6
#define HAL_ERR_INVALID_ENTRY		-7
#define HAL_ERR_INVALID_OPERATION	-8
#define HAL_ERR_INVALID_DRIVER		-9
#define HAL_ERR_INVALID_START           -23


typedef isr_t 		hal_isr_t;

int 		hal_register_isr(int vector, hal_isr_t isr, hal_isr_t *old);
void 		hal_system_error(int err);
void 		hal_global_int_en(int intr_no);
void 		hal_global_int_dis(int intr_no);
unsigned int	hal_global_int_ctl(int int_op);
unsigned int	hal_ms2ticks(unsigned int timeout);
/*delay ms*/
int				hal_sleep(int ms);
/*delay us*/
int 			hal_udelay(int us);
/*get os time uilt ms*/
unsigned int	hal_time_get(void);
unsigned int	hal_hz_get(void);
int				hal_random_get(void);

void* 		hal_get_pid_current();

int 		hal_create_semaphore(hal_semaphore_t *sem, unsigned int initCnt, unsigned int maxCnt);
int 		hal_destroy_semaphore(hal_semaphore_t *sem);
int 		hal_pend_semaphore(hal_semaphore_t *sem, unsigned int timeout);
int 		hal_post_semaphore(hal_semaphore_t *sem);
int			hal_getCnt_semphore(hal_semaphore_t *sem, unsigned int *validCnt);
int 		hal_post_semaphore_in_isr(hal_semaphore_t *sem);

int 		hal_create_mutex(hal_mutex_t *mutex);
int 		hal_destroy_mutex(hal_mutex_t *mutex);
/*timeout == HAL_SUSPEND.is not timeout*/
int 		hal_wait_for_mutex(hal_mutex_t *mutex, unsigned int timeout);
int 		hal_release_mutex(hal_mutex_t *mutex);

int			hal_create_queue(hal_queue_t *queue);
int			hal_destroy_queue(hal_queue_t *queue);
int			hal_recv_queue(hal_queue_t *queue);
int			hal_send_queue(hal_queue_t *queue);
int			hal_send_queue_from_lisr(hal_queue_t *p_queue);
int 		hal_getCnt_queue(hal_queue_t *queue, unsigned int *validCnt);

int				hal_create_thread(hal_thread_t *th);
void* 			hal_getTimeouts_thread(void);
void			hal_dump_thread(void);
int				hal_destroy_thread(hal_thread_t *th);
unsigned int	hal_get_prio_thread(hal_thread_t *th);
int				hal_set_prio_thread(hal_thread_t *th, unsigned int val);
int				hal_suspend_thread(hal_thread_t *th);
int				hal_resume_thread(hal_thread_t *th);
unsigned long 	hal_get_stack_freeSize_thread(hal_thread_t *th);

void			hal_TaskSuspendAll(void);
unsigned int	hal_TaskResumeAll(void);

int 		hal_create_bh(hal_bh_t *bh);
int 		hal_destroy_bh(hal_bh_t *bh);
int 		hal_register_bh(hal_bh_t *bh, void *param);
int 		hal_raise_bh(hal_bh_t *bh);
int 		hal_init_os(unsigned char * sram,unsigned int sramlen, unsigned char * heap,unsigned int heaplen);
void 		hal_start_os();
void 		hal_end_os(void);

unsigned int 	hal_irq_save(void);
void 			hal_irq_restore(unsigned int cpu_sr);
#if configUSE_TIMERS
int 			hal_create_timer(hal_timer_t *ptimer);
int 			hal_start_timer(hal_timer_t *ptimer, unsigned int waitMs);
int 			hal_stop_timer(hal_timer_t *ptimer, unsigned int waitMs);
int 			hal_delete_timer(hal_timer_t *ptimer, unsigned int waitMs);
int 			hal_reset_timer(hal_timer_t *ptimer, unsigned int waitMs);
int 			hal_change_period_timer(hal_timer_t *ptimer, unsigned int newPeriod, unsigned int waitMs);
#endif //configUSE_TIMERS
/*if uart is used to iot_printf debug information*/
bool 			hal_uart_dbg_is_open(void);
/*open uart used to iot_printf debug information*/
void 			hal_uart_dbg_open(void);
/*
close uart used to iot_printf/printf debug information
this function not close UART_TSPT_xxxxx function
*/
void 			hal_uart_dbg_close(void);
/*get system sdk version */
unsigned int hal_get_system_version(void);

//system
void 			hal_system_reboot(void);
unsigned int	hal_sys_mem_free_size_get(void);
void			hal_sys_flash_reset(void);
void 			hal_64bit_mult(HAL_64BIT *pstRst,unsigned int m1, unsigned int m2);
unsigned int 	hal_64bit_div(HAL_64BIT stDivisor,HAL_64BIT stDividend);
unsigned int 	hal_get_os_second_time(void);
unsigned int hal_get_os_ms_time(void);

/*get current real time xx/xx/xx ,see AT_GetSysTime*/
unsigned int hal_get_current_real_time();
void pwm_start(u16 pin,u16 PWM16_Hi,u16 PWM16_Lo);
void pwm_stop(u16 pin);


#define SYS_SLEEP_BUSY_EVENT_DMA BIT(0)
#define SYS_SLEEP_BUSY_EVENT_SPI BIT(1)
#define SYS_SLEEP_BUSY_EVENT_APP BIT(2)
#define SYS_SLEEP_BUSY_EVENT_SOCKET BIT(3)
#define SYS_SLEEP_BUSY_EVENT_SOCKET_ARDUINO_RX BIT(4)
#define SYS_SLEEP_BUSY_EVENT_SOCKET_ARDUINO_TX BIT(5)
#define SYS_SLEEP_BUSY_EVENT_HMAC_TX BIT(6)


enum HAL_SLEEP_MODE{
	HAL_NO_SLEEP,
	HAL_MODEM_SLEEP,
	HAL_LIGHT_SLEEP,
	HAL_DEEP_SLEEP,
};
/*used by system sleep, if have SysPowerBusyEvents set, system will not goto sleep*/
void hal_system_set_powerbusy(u32 busyevent);
void hal_system_clear_powerbusy(u32 busyevent);
bool hal_ps_suspend_early();
int hal_ps_gpio_wakeup_set(u8 gpio,bool low_level_wakeup);
int hal_ps_gpio_wakeup_clear();
enum HAL_SLEEP_MODE  hal_get_sleep_mode();
void hal_set_sleep_mode(enum HAL_SLEEP_MODE mode);

/*
no gpio wakeup return -1;
else return gpio num
*/
int hal_ps_gpio_wakeup_get();


int hal_sys_spin_lock_init(struct hal_spin_lock_s *spin);
void hal_sys_spin_lock_free(struct hal_spin_lock_s *spin);
void hal_sys_spin_lock(struct hal_spin_lock_s *spin);
void hal_sys_spin_unlock(struct hal_spin_lock_s *spin);

/*********************512 Bytes OTA header section in bin file*********
/magic/ver/---- ---                   4  bytes Magic number  and VER for OTA update sync check. 0x5a47 is used for ota data sync, xx xx is version informatiion.
---- ----  ---- ----  ---- ----/ 16 bytes reserved data for future 
/--LEN1--  --LEN2 -/                   8 bytes for OTA total length.
------------------ 
-----........ --------
------------------ (512 -28 = 484) bytes reserved
***************************************************************/

#define magic_header0 0x5a
#define magic_header1 0x47
#define OTA_FW_HEAD_LEN 0x200



struct ota_fw_header
{
    unsigned char magic_data0;
	unsigned char magic_data1;
	unsigned short version;
    unsigned char reserved[16];   //reserved for future.

	unsigned int fw1_length;
	unsigned int fw2_length;
};

#ifndef _ATBM_HAL_H__
//#include "os_api.h"
#include "uart.h"
#endif// _ATBM_HAL_H__

#endif /* __HAL_H__ */
