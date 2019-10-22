#ifndef _HMAC_TIMER_H
#define _HMAC_TIMER_H
//#include <type.h>
typedef void (*callback)(void *data);
struct system_timer{
	 struct system_timer  *pNext;
	  struct system_timer  *pLast;
	 uint32			 	TimeAtTimeout;
	 callback		 	cb;
	 void 			 	*arg;
};

extern void System_timer_IRQ(void);
extern void System_timer_task(void);
extern struct system_timer *System_timer_malloc(callback cb,void *arg);
extern bool System_timer_start(struct system_timer * pTimer,int Interval);
extern bool System_timer_free(struct system_timer * pTimer);
extern struct system_timer * System_timer_setup(u32 timeout, callback cb,void *arg);
extern void System_timer_init_timer( struct system_timer *timer, callback cb,void *arg);
extern  bool  System_timer_cancel(struct system_timer *pTimer);

#endif //_HMAC_TIMER_H
