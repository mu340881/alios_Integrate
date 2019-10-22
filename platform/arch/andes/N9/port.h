/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef PORT_H
#define PORT_H

#include "k_types.h"
#include "k_api.h"
#include <stddef.h>


void  cpu_intrpt_switch(void);
void  cpu_task_switch(void);
void  cpu_first_task_start(void);
void *cpu_task_stack_init(cpu_stack_t *base, size_t size, void *arg,
                          task_entry_t entry);

extern void  OS_RestoreFiqIrq(unsigned int IState);
extern unsigned int	 OS_DisableFiqIrq();

/* int lock for spinlock */
#define cpu_intrpt_save()           OS_DisableFiqIrq()
#define cpu_intrpt_restore(cpsr)    OS_RestoreFiqIrq(cpsr)

/* normal int lock (can not lock the NMI) */
#define CPSR_ALLOC()                cpu_cpsr_t cpsr
#define RHINO_CPU_INTRPT_DISABLE()  \
        do{cpsr = cpu_intrpt_save();}while(0)
#define RHINO_CPU_INTRPT_ENABLE()   \
        do{cpu_intrpt_restore(cpsr);}while(0)




RHINO_INLINE uint8_t cpu_cur_get(void)
{
    return 0;
}

#endif /* PORT_H */
