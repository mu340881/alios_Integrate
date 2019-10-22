#ifndef _VECTOR_H_
#define _VECTOR_H_
#include "ae210p_regs.h"
#include "ae210p_defs.h"

//#define OS_Trap_Interrupt_HW2   IRQ_PIT
#if (CHIP_TYPE==CHIP_ATHENAB)
#define OS_Trap_Interrupt_HW15	OS_Trap_Interrupt_SWI
#else
#define OS_Trap_Interrupt_HW14	OS_Trap_Interrupt_SWI
#endif //


#define REG32(x)        (*(volatile unsigned long*)(x))
//#define REGISTER_TICK1_ISR()	register_isr(IRQ_PIT_VECTOR, tick_isr_fp, (void*)0);


//#define REGISTER_TICK2_ISR()


#endif

