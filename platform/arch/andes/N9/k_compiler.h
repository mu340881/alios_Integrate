/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_COMPILER_H
#define K_COMPILER_H

#define RHINO_INLINE                static inline
/* get the return address of the current function
   void * __builtin_return_address (unsigned int level) */
#define RHINO_GET_RA()              __builtin_return_address(0)
/* get the return address of the current function */
__attribute__((always_inline)) RHINO_INLINE void *RHINO_GET_SP(void)
{
    void *stack;	
     __asm__ __volatile__ ("\tori\t%0, $sp, #0\n" : "=r" (stack));
    return stack;
}
/* get the number of leading 0-bits in x
   int __builtin_clz (unsigned int x) */
//#define RHINO_BIT_CLZ(x)            __builtin_clz(x)

#endif /* K_COMPILER_H */

