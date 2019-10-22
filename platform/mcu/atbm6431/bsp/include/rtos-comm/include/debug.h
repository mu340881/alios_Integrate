#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>

FLASH_FUNC void system_reboot(void);
FLASH_FUNC void OS_Exception(int entry_num);


#define DEBUG(enable, tagged, ...)						\
	do{									\
		if( enable){							\
			if( tagged)						\
				iot_printf("[ %8s() ] ", __func__);	\
			iot_printf(__VA_ARGS__);				\
		}							        \
	} while( 0)

#define ERROR(...)	iot_printf("ERROR:"__VA_ARGS__)
#define WARN(...)	iot_printf("WARNING:"__VA_ARGS__)
#define WARN_P(cond,...)	({ if (cond) DEBUG(1, 1, "WARNING:"__VA_ARGS__); \
	(cond); \
})


#define KASSERT(cond)								\
{										\
	if (!(cond)){								\
		ERROR("ASSERT in %s:\n"				\
				"line %d "					\
				"RA=%lx\n",					\
				__func__,					\
				__LINE__,					\
				(unsigned long)__builtin_return_address(0));	\
		OS_Exception(0xfe);					\
	}									\
}
/*
WARN_ON can't not used to warnning  func,this may cause "call function twice bug"
WARN_ON_FUNC used to warnning  func
*/
#define WARN_ON(cond)  						\
({	int tmpcond=(cond); 				\
	if ((tmpcond)){ 						\
		WARN(" in %s: line:%d\n",			\
				__func__,	\
				__LINE__);	\
	}				\
	(tmpcond); \
})


#define WARN_ON_ONCE  WARN_ON

#define WARN_ON_FUNC(cond)  						\
({											\
	if ((cond)){							\
		WARN(" in %s: line:%d\n",			\
				__func__,	\
				__LINE__);	\
	}				\
})


#define KPANIC(args, ...)		\
{					\
	ERROR(args, __VA_ARGS__);	\
	while (1) printf_task();	\
}



#define BUG_ON(cond)  	KASSERT(!(cond))


#define BUILD_BUG_ON(cond)  						\
{											\
	if ((cond)){							\
		WARN("BUILD_BUG_ON in %s:<%d>\n",					\
				__func__,					\
				__LINE__);	\
	}				\
}


#define WARN_CODE001   0x001
#define WARN_CODE002   0x002
#define WARN_CODE003   0x003
#define WARN_CODE004   0x004
#define WARN_CODE005   0x005
#define WARN_CODE006   0x006
#define WARN_CODE007   0x007
#define WARN_CODE008   0x008
#define WARN_CODE009   0x009
#define WARN_CODE00A   0x00a
#define WARN_CODE00B   0x00b
#define WARN_CODE00C   0x00c





#define WIFI_TX 		BIT(0)
#define WIFI_RX 		BIT(1)
#define WIFI_WSM 		BIT(2)
#define WIFI_PS 		BIT(3)
#define WIFI_QUEUE 		BIT(4)
#define WIFI_IF 		BIT(5)
#define WIFI_DBG_MSG 	BIT(6)
#define WIFI_DBG_ANY 	BIT(7)
#define WIFI_DBG_INIT 	BIT(8)
#define WIFI_DBG_ERROR 	BIT(9)
#define WIFI_WARN_CODE 	BIT(10)
#define WIFI_OS 		BIT(11)
#define WIFI_CONNECT	BIT(12)
#define WIFI_SCAN		BIT(13)
#define WIFI_RATE		BIT(14)
#define WIFI_ATCMD 		BIT(15)
#define WIFI_WPA 		BIT(16)
#define WIFI_TIME 		BIT(17)
#define WIFI_KEY 		BIT(18)
#define WIFI_WPS		BIT(19)



#define WIFI_DEBUG_LEVEL  (WIFI_ATCMD|WIFI_DBG_INIT|WIFI_DBG_ERROR|WIFI_DBG_ANY|WIFI_WARN_CODE) //WIFI_TX|WIFI_RX|WIFI_WSM|WIFI_PS|WIFI_IF


#define atbmwifi_dbg(level, ...)				\
	do {						\
		if ((level) & WIFI_DEBUG_LEVEL)		\
			printk(KERN_DEBUG __VA_ARGS__);	\
	} while (0)




extern unsigned int wifi_debug_mask;
extern char WARN_CODE_DEBUG[];

#define wifi_printk(_level,...) do { if((_level)&WIFI_DEBUG_LEVEL) iot_printf(__VA_ARGS__);} while (0)

#define wifi_printk_time(_level,...) do { if((_level)&wifi_debug_mask){  if(wifi_debug_mask&WIFI_TIME) iot_printf("[%d]",hal_time_get());   iot_printf(__VA_ARGS__);}} while (0)


#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR_HAILIN "%02X%02X%02X%02X%02X%02X"
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"

#define WARN_CODE(wc)	wifi_printk(WIFI_WARN_CODE,"\n%s:%d\n",WARN_CODE_DEBUG,wc)

#define DEBUG_FLAG(c) Console_Putc(c)

void dump_mem(const void *mem, int count);
 void dump_stack(void);
#define wifi_save_ande_printk_Stack() do{ int stack; \
hal_bh_t *pthis_bh = (hal_bh_t *)param;\
__asm__ __volatile__ ("\tori\t%0, $sp, #0\n" : "=r" (stack)); \
if(pthis_bh){pthis_bh->th.ptos	= stack;}\
DBG_Printf("now_sp:0x%x\n",(int )stack); \
}while(0)

#define wifi_printk_Stack()do{ int stack; \
__asm__ __volatile__ ("\tori\t%0, $sp, #0\n" : "=r" (stack)); \
DBG_Printf("now_sp:0x%x\n",(int )stack); \
}while(0)

//#define OS_TASK_DEBUG
#ifdef OS_TASK_DEBUG

#define MAX_TASK_DEBUG_CNT 4
struct task_debug {
	unsigned long task_top[MAX_TASK_DEBUG_CNT];
	unsigned long task_size[MAX_TASK_DEBUG_CNT];
	int cnt;
};
extern struct task_debug os_task_dbg; 

#define  TaskDebug_Put() do{ \
	if(os_task_dbg.cnt < MAX_TASK_DEBUG_CNT){ \
	   int stack=0;	   hal_bh_t *pthis_bh = (hal_bh_t *)param; \
	   __asm__ __volatile__ ("\tori\t%0, $sp, #0\n" : "=r" (stack));  \
	   if(pthis_bh){ \
		  pthis_bh->th.ptos	= stack; \
	   }  \
		if(pthis_bh){ \
		  os_task_dbg.task_size[os_task_dbg.cnt] = pthis_bh->th.stack_size;  \
	   } \
		else { \
			os_task_dbg.task_size[os_task_dbg.cnt] = 1024;  \
		} \
		os_task_dbg.task_top[os_task_dbg.cnt] = stack; \
		os_task_dbg.cnt++; \
	} \
}while(0)
#else
#define  TaskDebug_Put()
#endif //OS_TASK_DEBUG

#endif /* __DEBUG_H__ */
