
/*****************************************************************************
 *
 *            Copyright Andes Technology Corporation 2009-2010
 *                         All Rights Reserved.
 *
 *  Revision History:
 *
 *    Dec.28.2009     Created.
 ****************************************************************************/

#include "atbm_hal.h"
#include "os_api.h"
#ifdef LWIP_ATBM
#include "lwip/sys.h"
#endif  //LWIP_ATBM
//#include "lmac_os.h"
//#include "platform.h"
#include "app_flash_param.h"
#include "k_api.h"
#include "aos/kernel.h"
#define iot_printf printf


#ifdef LWIP_ATBM

#define SYS_THREAD_MAX		16 //task total numbers
#define THREAD_NAME_LEN		16 //task name length, follow kernel length
typedef struct task_thread
{
	//void (* thread_func)(void *);		//task callback function
	//void *arg_func;					//task parameters of callback function
	void *thread_pid;					//task handle
	char thread_name[THREAD_NAME_LEN];	//task name for debug
	struct sys_timeouts timeouts;		//In lwIP,each thread has a list of timeouts which is represented 
										//as a linked list of sys_timeout structures.
}TASK_THREAD;

static TASK_THREAD thread_arry[SYS_THREAD_MAX];
static unsigned short thread_id = 0;

#endif  //LWIP_ATBM
extern void lmac_Wait(uint32 delay_in_us);
extern int os_random(void);
extern void sys_mem_init(unsigned char * sram,unsigned int sramlen, unsigned char * heap,unsigned int heaplen);
extern void aos_start_os(void);

extern u32 sys_mem_free_size_get(void);
extern void hal_alarm_init(void);  
extern void system_reboot(void);
extern void OS_Exception(int entry_num);
extern void  atcwdt200_restart_wdt();

  int hal_register_isr(int vector, hal_isr_t isr, hal_isr_t *old)
{
	register_isr(vector, isr, old);
	return HAL_SUCCESS;
}

  void hal_system_error(int err)
{
	OS_Exception(2019);
	while (1)
		;
}

  void hal_global_int_en(int intr_no) {
         u32 int_mask;
 
         __asm__ __volatile__ ("mfsr %0, $INT_MASK2" : "=r"(int_mask));
         int_mask |= ((u32)0x1) << intr_no;
         __asm__ __volatile__ ("mtsr %0, $INT_MASK2" : : "r"(int_mask));
}
 
 
  void hal_global_int_dis(int intr_no) {
         u32 int_mask;
 
         __asm__ __volatile__ ("mfsr %0, $INT_MASK2" : "=r"(int_mask));
         int_mask &= ~(((u32)0x1) << intr_no);
         __asm__ __volatile__ ("mtsr %0, $INT_MASK2" : : "r"(int_mask));
}


  unsigned int hal_global_int_ctl(int int_op)
{
	int ret = GET_PSW() & PSW_mskGIE;

	if (int_op == HAL_DISABLE_INTERRUPTS) {

		__asm__("setgie.d");
		__asm__("isb");
	}
	else{
		__asm__("setgie.e");
	}

	return ret ? !HAL_DISABLE_INTERRUPTS : HAL_DISABLE_INTERRUPTS;
}


NO_FLASH_FUNC  unsigned int hal_irq_save(void)
{	
	return OS_DisableFiqIrq();
}

NO_FLASH_FUNC  void hal_irq_restore(unsigned int cpu_sr)
{
	OS_RestoreFiqIrq(cpu_sr);
}

AOS_FUNC_SECTION void hal_wdt_feed()
{
	atcwdt200_restart_wdt();
}


NO_FLASH_FUNC   unsigned int hal_ms2ticks(unsigned int timeout)
{
        return timeout = (timeout * RHINO_CONFIG_TICKS_PER_SECOND + 999)/1000;
}

NO_FLASH_FUNC   int hal_sleep(int ms)
{
	aos_msleep(ms );
	return HAL_SUCCESS;
}

NO_FLASH_FUNC   int hal_udelay(int us)
{
	lmac_Wait(us);
	return HAL_SUCCESS;
}



NO_FLASH_FUNC  unsigned int hal_time_get(void)
{
	return (unsigned int)(krhino_sys_tick_get()*(1000/RHINO_CONFIG_TICKS_PER_SECOND));
}

NO_FLASH_FUNC  unsigned int hal_hz_get(void)
{
	return RHINO_CONFIG_TICKS_PER_SECOND;
}

NO_FLASH_FUNC  int hal_random_get(void)
{
	return os_random();
}

NO_FLASH_FUNC   void* hal_get_pid_current(void)
{
	return krhino_cur_task_get();
}
FLASH_FUNC   int hal_create_semaphore(hal_semaphore_t *sem, unsigned int initCnt, unsigned int maxCnt)
{
//	unsigned int max;
    aos_sem_t aos_sem;

	if(sem == NULL){
		printf("semaphore create param is invalid\n");
		return HAL_FAILURE;
	}
	
	/*
	 * max : the maximun count value that can be reached. 
	 * 	 When the semaphore reaches this value it can no longer be given 
	 * initCnt : the count value assigend to the semaphore when it is created  */	
    if (0 != aos_sem_new(&aos_sem, initCnt)) {
		printf("semaphore create param is fail\n");
        return HAL_FAILURE;
    }
	
	sem->obj = aos_sem.hdl;
	//printf("semaphore create sem[%x]->obj %x\n",sem,sem->obj);
	return HAL_SUCCESS;
}

FLASH_FUNC   int hal_destroy_semaphore(hal_semaphore_t *sem)
{
    aos_sem_t aos_sem;

	if((sem == NULL) || (sem->obj== NULL)){
		printf("semaphore destroy param is invalid1 %x\n",sem);
		return HAL_FAILURE;
	}
	aos_sem.hdl = sem->obj;
	aos_sem_free((aos_sem_t *)&aos_sem);
	return HAL_SUCCESS;
 
}

NOCACHE_NOFLASH_FUNC   int hal_pend_semaphore(hal_semaphore_t *sem, unsigned int timeout)
{
    aos_sem_t aos_sem;
	if((sem == NULL) || (sem->obj == NULL)){

		printf("semaphore pend param is invalid1 %x\n",sem);
		if(sem)
		printf("semaphore pend param is invalid2 %x\n",sem->obj);
		return HAL_FAILURE;
	}

	if(timeout == HAL_SUSPEND)
		timeout = AOS_WAIT_FOREVER;
	
#ifdef SEMAPHORE_DEBUG	
	printf("pend_sem %x\n",sem->obj);
	if(sem->name){
		printf("pend_semaname %s\n",sem->name);
	}
#endif  //#ifdef SEMAPHORE_DEBUG	
	aos_sem.hdl = sem->obj;
	if (aos_sem_wait((aos_sem_t *)&aos_sem,  timeout ) == 0){
		return HAL_SUCCESS;
	}
	
	return HAL_FAILURE;
}

NOCACHE_NOFLASH_FUNC    int hal_post_semaphore(hal_semaphore_t *sem)
{
    aos_sem_t aos_sem;
	if((sem == NULL) || (sem->obj== NULL)){

		printf("semaphore post param is invalid1 %x\n",sem);
		if(sem)
		printf("semaphore post param is invalid2 %x\n",sem->obj);
		return HAL_FAILURE;
	}
	aos_sem.hdl = sem->obj;
	aos_sem_signal((aos_sem_t *)&aos_sem ) ;

	return HAL_SUCCESS;
}

/*
 *
 * @param pxHigherPriorityTaskWoken xSemaphoreGiveFromISR() will set
 * *pxHigherPriorityTaskWoken to pdTRUE if giving the semaphore caused a task
 * to unblock, and the unblocked task has a priority higher than the currently
 * running task.  If xSemaphoreGiveFromISR() sets this value to pdTRUE then
 * a context switch should be requested before the interrupt is exited.
 */
 extern bool in_irq;
NO_FLASH_FUNC   int hal_post_semaphore_in_isr(hal_semaphore_t *sem)
{
	hal_post_semaphore(sem);
	return HAL_SUCCESS;
}


FLASH_FUNC  int hal_create_mutex(hal_mutex_t *mutex)
{
	return hal_create_semaphore((hal_semaphore_t *)mutex, 1, 1);
}
FLASH_FUNC  int hal_destroy_mutex(hal_mutex_t *mutex)
{
	return hal_destroy_semaphore((hal_semaphore_t *)mutex);
}
NOCACHE_NOFLASH_FUNC int hal_wait_for_mutex(hal_mutex_t *mutex, unsigned int timeout/*ms*/)
{
	return hal_pend_semaphore((hal_semaphore_t *)mutex, timeout);
}
NOCACHE_NOFLASH_FUNC int hal_release_mutex(hal_mutex_t *mutex)
{
	return hal_post_semaphore((hal_semaphore_t *)mutex);
}


FLASH_FUNC   int hal_create_thread(hal_thread_t *th)
{
	int ret;
	unsigned int irq;
#ifdef LWIP_ATBM
	unsigned int nameLen;
#endif
	if(th == NULL)
		return HAL_FAILURE;
	if(th->ptos){
		printf("<WARNING>hal_create_thread not need ptos\n");
	}
	//disable interrupt
	irq = hal_irq_save();

#ifdef LWIP_ATBM
	nameLen = strlen(th->name);

	memset(thread_arry[thread_id].thread_name, 0, THREAD_NAME_LEN);
	memcpy(thread_arry[thread_id].thread_name, th->name, nameLen>THREAD_NAME_LEN?THREAD_NAME_LEN:nameLen);
#endif  //LWIP_ATBM

    ret = (int)krhino_task_dyn_create((ktask_t **)&th->task, th->name, th->arg, 32-2 - th->prio, 
                                  1, (th->stack_size >> 2), th->fn, 1u);

	/* 
	 * th->stack_size in byte for th->stack_size
	 * th->stack_size in word for xTaskGenericCreate 
	 *
	 * pxNewTCB->pxStack is a unsigned long pointer
	 * pxTopOfStack = pxNewTCB->pxStack + ( usStackDepth - 1 );
	 * */
	if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED))
	{
		ret = HAL_FAILURE;
	}
	else
	{	
#ifdef LWIP_ATBM
		thread_arry[thread_id].thread_pid = th->task;
		thread_id++;
#endif //#ifdef LWIP_ATBM
		ret = HAL_SUCCESS;
	}

	//enable interrupt
	hal_irq_restore(irq);

	return ret;
}
NO_FLASH_FUNC    void* hal_getTimeouts_thread(void)
{
#ifdef LWIP_ATBM
	void *pid;

	pid = hal_get_pid_current();
	
	for(i=0; i<thread_id; i++)
	{
		if(thread_arry[i].thread_pid == pid)
			return (void*)&(thread_arry[i].timeouts);
	}
#endif //#ifdef LWIP_ATBM

	return NULL;//not found
}
FLASH_FUNC   void hal_dump_thread(void)
{
#ifdef LWIP_ATBM
	int i;
	unsigned int irq;

	//disable interrupt
	irq = hal_irq_save();

	wifi_printk(WIFI_ATCMD,"user layer dump task name:\n");
	wifi_printk(WIFI_ATCMD,"/////////////////\n");

	for(i=0; i<thread_id; i++)
	{
		wifi_printk(WIFI_ATCMD,"[%d] %s\n", i, thread_arry[i].thread_name);
	}
	
	wifi_printk(WIFI_ATCMD,"/////////////////\n");


	//enable interrupt
	hal_irq_restore(irq);
#endif //#ifdef LWIP_ATBM

	return;
}

FLASH_FUNC   int hal_destroy_thread(hal_thread_t *th)
{
	void *pid;
#ifdef LWIP_ATBM
	int i;
	unsigned int irq;
#endif //#ifdef LWIP_ATBM

	//get task handle
	if((th != NULL) && (th->task != NULL))
		pid = th->task;
	else
		pid = hal_get_pid_current();
#ifdef LWIP_ATBM

	//disable interrupt
	irq = hal_irq_save();	
	
	for(i=0; i<thread_id; i++)
	{
		if(thread_arry[i].thread_pid == pid)
		{
			thread_id--;
			break;
		}
	}
	
	for( ;i<thread_id; i++)
	{
		thread_arry[i]=thread_arry[i+1];
		thread_arry[i]=thread_arry[i+1];
	}

	//enable interrupt
	hal_irq_restore(irq);
#endif //#ifdef LWIP_ATBM

	krhino_task_dyn_del(pid);

	return HAL_SUCCESS;
}



FLASH_FUNC  int hal_suspend_thread(hal_thread_t *th)
{
	if((th != NULL) && (th->task != NULL))
		krhino_task_suspend(th->task);
	else
		krhino_task_suspend(krhino_cur_task_get());
	
	return HAL_SUCCESS;
}

FLASH_FUNC  int hal_resume_thread(hal_thread_t *th)
{
	if((th != NULL) && (th->task != NULL))
		krhino_task_resume(th->task);
	else{
		printf("hal_resume_thread <error>\n");
	}
	
	return HAL_SUCCESS;
}

FLASH_FUNC  unsigned long hal_get_stack_freeSize_thread(hal_thread_t *th)
{
	unsigned int free_sz;
	
	if((th != NULL) && (th->task != NULL))
		free_sz = krhino_task_stack_min_free(th->task,&free_sz);
	else
		free_sz = krhino_task_stack_min_free(krhino_cur_task_get(),&free_sz);
	
	return free_sz;
}
NOCACHE_NOFLASH_FUNC  void hal_TaskSuspendAll(void)
{
	krhino_sched_disable();
	return;
}

NOCACHE_NOFLASH_FUNC  unsigned int hal_TaskResumeAll(void)
{
	unsigned int ret=HAL_FAILURE;
	if(krhino_sched_enable()==RHINO_SUCCESS)
	{
		ret = HAL_SUCCESS;
	}
	return ret;
}

FLASH_FUNC   int hal_create_bh(hal_bh_t *bh)
{

	if(hal_create_semaphore(&bh->sem, 0, 1) == HAL_FAILURE)
		return HAL_FAILURE;
#ifdef SEMAPHORE_DEBUG
		memcpy(bh->sem.name,bh->th.name,3);
		bh->sem.name[3] = 0;
#endif
	return hal_create_thread(&bh->th);
}

FLASH_FUNC   int hal_destroy_bh(hal_bh_t *bh)
{
	if(bh != NULL) {
		hal_destroy_semaphore(&bh->sem);
		hal_destroy_thread(bh->th.task);
		return HAL_SUCCESS;
	} else
		return HAL_FAILURE;
}

  NO_FLASH_FUNC int hal_register_bh(hal_bh_t *bh, void *param)
{
	return HAL_SUCCESS;
}

 NO_FLASH_FUNC  int hal_raise_bh(hal_bh_t *bh)
{
	//printf("hal_raise_bh %s [%x]\n",bh->th.name,g_task->task_stack);
	return hal_post_semaphore_in_isr(&bh->sem);
}

/*
mem is SRAM address ,used in LWIP
heap is dccm address used in kmalloc
*/
  int hal_init_os(unsigned char * sram,unsigned int sramlen, unsigned char * heap,unsigned int heaplen)
{	
/*
	printf_init();
	
	printf("hal_init_os memtype %d,heap %x\n",MEM_SRAM,sram);	
	sys_heapinit(MEM_SRAM,sram, sramlen);//used mem_malloc smalloc
	sys_heapinit(MEM_HEAP,heap,heaplen);//used kmalloc
	hal_workqueue = hal_create_task();
	if(hal_workqueue == NULL){
		return HAL_FAILURE;
	}
	Timer_Init();
*/
#ifdef LWIP_ATBM
	//initial task arry
	memset(&thread_arry[0], 0, sizeof(TASK_THREAD)*SYS_THREAD_MAX);
#endif //#ifdef LWIP_ATBM
	//initial memory
	sys_mem_init(sram,sramlen,heap,heaplen);

	//initial system alarm
	hal_alarm_init();
	
	return HAL_SUCCESS;
}

void hal_start_os(void)
{	
    /*kernel start schedule!*/
    aos_start_os();
	return;
}

  void hal_end_os(void)
{	
	GIE_ENABLE();
	//vTaskEndScheduler();
	return;
}


static bool  uart_printf_support =1;
void hal_uart_dbg_open(void)
{
	uart_printf_support = 1;
	return;
}
void hal_uart_dbg_close(void)
{
	uart_printf_support = 0;
	return;
}
bool hal_uart_dbg_is_open(void)
{
	return uart_printf_support;
}


void hal_sys_reboot(void)
{
	system_reboot();
	return;
}

unsigned int hal_sys_mem_free_size_get(void)
{
	return sys_mem_free_size_get();
}

void hal_sys_flash_reset(void)
{
	atbmwifi_flash_param_config_restore();
	return;
}

/****************************************
*Description: 2 32bit data multiplication
*	m1*m2=pstRst
*Parameters:
*	pstRst:  : 64bit store of multiple result
*	m1	  :    multiplier one
*	m2	  :	   multiplier two
*Return: void
*
****************************************/
FLASH_FUNC  void hal_64bit_mult(HAL_64BIT *pstRst,unsigned int m1, unsigned int m2)
{
	unsigned int ui32LowPart,ui32Tmp;
	pstRst->ui32Low = (m1&0xffff) * (m2&0xffff);
	pstRst->ui32High = (m1>>16) * (m2>>16);

	ui32LowPart = pstRst->ui32Low;
	ui32Tmp = (m1>>16) * (m2&0xffff);
	pstRst->ui32Low  += (ui32Tmp<<16);
	pstRst->ui32High += (ui32Tmp>>16);
	if(ui32LowPart > pstRst->ui32Low)
	{
		pstRst->ui32High++;
	}

	ui32LowPart = pstRst->ui32Low;
	ui32Tmp = (m2>>16) * (m1&0xffff);
	pstRst->ui32Low  += (ui32Tmp<<16);
	pstRst->ui32High += (ui32Tmp>>16);
	if(ui32LowPart > pstRst->ui32Low)
	{
		pstRst->ui32High++;
	}

	return;
}

FLASH_FUNC  static unsigned char _hal_64bit_ComPare(HAL_64BIT stPara,HAL_64BIT stPara1)
{
	unsigned char ui8Flag = 0;//default :stPara<=stPara1
	if (stPara.ui32Low>=stPara1.ui32Low)
	{
		if (stPara.ui32High >= stPara1.ui32High)
		{
			ui8Flag = 1;
		}
	}
	else
	{
		if (stPara.ui32High > stPara1.ui32High)
		{
			ui8Flag = 1;
		}
	}
	return ui8Flag;

}

/***********************************
*Description: 2 64bit data division
*	stDivisor/stDividend=unsigned int
*Parameters:
*	stDivisor: numerator
*	stDividend:denominator
*Return:division result
*
************************************/
NO_FLASH_FUNC  unsigned int hal_64bit_div(HAL_64BIT stDivisor,HAL_64BIT stDividend)
{
	unsigned char  ui8DivLeftShit = 0;
	unsigned int ui32Rst = 0;
	unsigned int ui32flag = 1;
	HAL_64BIT stDataTmp = stDividend;
	
	if (stDivisor.ui32High == 0&&stDivisor.ui32Low == 0)
	{
		return 0;
	}
	else if (stDividend.ui32High == 0&&stDividend.ui32Low == 0)
	{
		return 0;
	}
	else if (stDividend.ui32High > stDivisor.ui32High)
	{
		return 0;
	}
	else if (!_hal_64bit_ComPare(stDivisor,stDividend))
	{
		return 0;
	}
	else if (stDividend.ui32High == 0 && stDivisor.ui32High == 0)
	{
		return stDivisor.ui32Low/stDividend.ui32Low;
	}
	else// normal condition  stDivisor >= stDividend
	{
		while(ui32flag)
		{
			++ui8DivLeftShit;
			stDataTmp.ui32High<<=1;
			if(stDataTmp.ui32Low&0x80000000)
			{
				stDataTmp.ui32High +=1;
			}
			stDataTmp.ui32Low<<=1;

			if (!_hal_64bit_ComPare(stDivisor,stDataTmp)||stDataTmp.ui32High>=0x80000000)
			{
				if (stDataTmp.ui32High<0x80000000)
				{
					stDataTmp.ui32Low>>=1;
					if (stDataTmp.ui32High&1)
					{
						stDataTmp.ui32Low |= 0x80000000;
					}
					stDataTmp.ui32High>>=1;
					ui8DivLeftShit--;
				}


				stDivisor.ui32High -= stDataTmp.ui32High;
				if(stDivisor.ui32Low >= stDataTmp.ui32Low)
				{
					stDivisor.ui32Low -= stDataTmp.ui32Low;
				}
				else
				{
					stDivisor.ui32Low +=0xffffffff - stDataTmp.ui32Low;
					stDivisor.ui32Low += 1;
					stDivisor.ui32High -= 1;
				}
				ui32Rst = (1<<ui8DivLeftShit);
				break;
			}
		}
		ui32Rst += hal_64bit_div(stDivisor,stDividend);
	}
	return ui32Rst;

}
extern 	unsigned int APB_Get_TimeBase()
;
extern uint32 hal_timer_swarp_get(void);
extern uint32  HAL_TIME_US();

/*unit :second*/
unsigned int hal_get_os_second_time(void)
{
	unsigned int time_part2= hal_timer_swarp_get();
	unsigned int time_part1= HAL_TIME_US();
	
	HAL_64BIT data1;
	HAL_64BIT data2;
	unsigned int ret =0;
	//if swarp immediate
	data1.ui32High = hal_timer_swarp_get();
	if(data1.ui32High != time_part2){
		time_part1= HAL_TIME_US();
	}
	
	time_part1 += APB_Get_TimeBase();
	if(time_part1 < APB_Get_TimeBase())	
		time_part2++;
	
	data1.ui32Low = time_part1;
	data1.ui32High= time_part2;
	
	data2.ui32Low = 1000000;
	data2.ui32High= 0;

	//ret =data1/data2
	ret = hal_64bit_div(data1,data2);

	return ret;
}
/*unit :second*/
unsigned int hal_get_os_ms_time(void)
{
	unsigned int time_part2= hal_timer_swarp_get();
	unsigned int time_part1= HAL_TIME_US();
	
	HAL_64BIT data1;
	HAL_64BIT data2;
	unsigned int ret =0;
	//if swarp immediate
	data1.ui32High = hal_timer_swarp_get();
	if(data1.ui32High != time_part2){
		time_part1= HAL_TIME_US();
	}
	
	time_part1 += APB_Get_TimeBase();
	if(time_part1 < APB_Get_TimeBase()) 
		time_part2++;

	
	data1.ui32Low = time_part1;
	data1.ui32High= time_part2;
	
	data2.ui32Low = 1000;
	data2.ui32High= 0;

	//ret =data1/data2
	ret = hal_64bit_div(data1,data2);

	return ret;
}

void hal_reboot(void)
{
	system_reboot();
}

void hal_system_reboot(void)
{
	system_reboot();
}
//used by system sleep
u32 SysPowerBusyEvents=0;

void hal_system_set_powerbusy(u32 busyevent)
{
    u32 IState;

    IState = OS_DisableFiqIrq();
	SysPowerBusyEvents |= busyevent;
	OS_RestoreFiqIrq(IState);
}

void hal_system_clear_powerbusy(u32 busyevent)
{

    u32 IState;

    IState = OS_DisableFiqIrq();
	SysPowerBusyEvents &= ~busyevent;
	OS_RestoreFiqIrq(IState);
}
extern  struct flash_boot_volatile_section iot_boot_volatile_sect;
extern int atbm_wifi_set_ps_mode(u8 enable);

void hal_set_sleep_mode(enum HAL_SLEEP_MODE mode)
{
	iot_boot_volatile_sect.wifi_param.system_sleep_mode = mode;
	atbm_wifi_set_ps_mode(mode != HAL_NO_SLEEP);
}


enum HAL_SLEEP_MODE  hal_get_sleep_mode()
{
	return iot_boot_volatile_sect.wifi_param.system_sleep_mode;
}

int hal_sys_spin_lock_init(struct hal_spin_lock_s *spin)
{
	hal_semaphore_t sem;
	spin->spin_cnt = 0;
	spin->wait_spin_cnt= 0;

	hal_create_semaphore( &sem ,0,0xffff);
	
	if( sem.obj== NULL )
	{		
		return HAL_FAILURE;	// TODO need assert
	}
	spin->xSemaphore = sem.obj;
	return HAL_SUCCESS;
}

void hal_sys_spin_lock_free(struct hal_spin_lock_s *spin)
{
	hal_semaphore_t sem;
	sem.obj=spin->xSemaphore;
	hal_destroy_semaphore(&sem);
	spin->xSemaphore = NULL;
}

NOCACHE_NOFLASH_FUNC  void hal_sys_spin_lock(struct hal_spin_lock_s *spin)
{
	hal_TaskSuspendAll();
	if(spin->spin_cnt==0){
		spin->spin_cnt++;
		hal_TaskResumeAll();
		return ;
	}	
	else {
		spin->wait_spin_cnt++;
		while(spin->spin_cnt){			
			hal_TaskResumeAll();
			//xSemaphoreTake( spin->xSemaphore , 1 );			
            krhino_sem_take(spin->xSemaphore, 1);
			hal_TaskSuspendAll();
		}		
		//one task have get the spin lock
		spin->spin_cnt++;
		spin->wait_spin_cnt--;		
		hal_TaskResumeAll();
	}
}
NOCACHE_NOFLASH_FUNC   void hal_sys_spin_unlock(struct hal_spin_lock_s *spin)
{
	//hal_TaskSuspendAll();
	spin->spin_cnt=0;
	if(spin->wait_spin_cnt){
		krhino_sem_give(  spin->xSemaphore  );
	}
	//hal_TaskResumeAll();
}
#if (RHINO_CONFIG_MM_TLF > 0)

u32 sys_mem_free_size_get(void)
{

    extern k_mm_head *g_kmm_head;
    //int               free = g_kmm_head->free_size;
	

	return g_kmm_head->free_size;
}
char * smalloc(int size)
{
	//not support
	return malloc(size);
}
#endif  //#if (RHINO_CONFIG_MM_TLF == 0)

