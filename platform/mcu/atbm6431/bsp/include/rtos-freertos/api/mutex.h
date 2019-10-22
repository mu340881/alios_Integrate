#ifndef _MUTEX_H_
#define _MUTEX_H_
#include "api.h"
#include "FreeRTOS.h"
//#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "arch/sys_arch_opts.h"
#include "arch/sys_arch.h"
#include "hal.h"



typedef hal_mutex_t mutex_t;

#define mutex_init(_m)  hal_create_mutex(_m)
#define mutex_lock(_m)  hal_wait_for_mutex(_m,HAL_SUSPEND)
#define mutex_unlock(_m) hal_release_mutex(_m)
#define mutex_destroy(_m) hal_destroy_mutex(_m)

#if 0


#define wake_up(wq)  mutex_unlock((wait_queue_head_t *)&wq)
#define down(sem) mutex_lock(sem)
#define up(sem) mutex_unlock(sem)

void sema_init(sys_sem_t * sem,int value);
typedef xSemaphoreHandle wait_queue_head_t;
#define init_waitqueue_head(sem) sema_init(sem,0)
int wait_event_timeout(wait_queue_head_t wq,unsigned int timeout);
int wait_event_interruptible(wait_queue_head_t wq,unsigned int condition);
int wait_event_interruptible_timeout(wait_queue_head_t wq,unsigned int condition,int timeout);

/*if timeout return 0;
else return 1;*/
#define wait_event_interruptible_timeout(wq,condition,timeout)  ({	 \
	int retimeout = timeout;	\
do{							\
	if(condition){retimeout=1; break;} \
	retimeout = wait_event_timeout(wq,retimeout);	 \
}while(retimeout != 0);\
	(retimeout);							\
} )
#endif//0

#endif
