!********************************************************************************************************
!
!                               (c) Copyright 2005-2014, Andes Techonology
!                                          All Rights Reserved
!
!                                           NDS32 Generic Port
!                                            GNU C Compiler
!
!********************************************************************************************************

!********************************************************************************************************
! INCLUDE ASSEMBLY CONSTANTS
!********************************************************************************************************

#include <nds32_init.inc>
#include "n12_def.h"
#include "vectors.h"

!********************************************************************************************************
! 	Interrupt Default Handler Define
!********************************************************************************************************

	.macro WEAK_DEFAULT weak_sym, default_handler
		.weak \weak_sym
		.set \weak_sym ,\default_handler
	.endm

	
	WEAK_DEFAULT OS_Trap_TLB_Fill,		OS_Trap_Default_TLB_Fill
	WEAK_DEFAULT OS_Trap_PTE_Not_Present,	OS_Trap_Default_PTE_Not_Present
	WEAK_DEFAULT OS_Trap_TLB_Misc,		OS_Trap_Default_TLB_Misc
	WEAK_DEFAULT OS_Trap_TLB_VLPT_Miss,	OS_Trap_Default_TLB_VLPT_Miss
	WEAK_DEFAULT OS_Trap_Machine_Error,	OS_Trap_Default_Machine_Error 
	WEAK_DEFAULT OS_Trap_Debug_Related,	OS_Trap_Default_Debug_Related
	WEAK_DEFAULT OS_Trap_General_Exception,	OS_Trap_Default_General_Exception
	WEAK_DEFAULT OS_Trap_Syscall,  		OS_Trap_Default_Syscall


		
	.global main
	.global __pre_c_init
	.global __post_c_init
	.global OS_Init_Nds32
	.global OS_Int_Vectors
	.global OS_Int_Vectors_End

	.macro CallFn fn
		push	$lp
		addi    $sp, $sp, -4 	! keep 8 byte align
		bal	\fn
		addi    $sp, $sp, 4
		pop	$lp
	.endm

! Define standard NDS32 vector table entry point of interruption vectors
	.macro VECTOR handler
	.align 4
		pushm	$r0, $r1
		sethi	$r0, hi20(\handler)
		ori	$r0, $r0, lo12(\handler)
		jral	$r0, $r0
	.endm



!********************************************************************************************************
! 	Interrupt Entry Table
!********************************************************************************************************

	.section .nds32_init, "ax"
	
OS_Int_Vectors:
!	b	OS_Init_Nds32			!  (0) Trap Reset/NMI
	j _start                    !  (0) Trap Reset

	VECTOR	OS_Trap_TLB_Fill		!  (1) Trap TLB fill
	VECTOR	OS_Trap_PTE_Not_Present		!  (2) Trap PTE not present
	VECTOR	OS_Trap_TLB_Misc		!  (3) Trap TLB misc
	VECTOR	OS_Trap_TLB_VLPT_Miss		!  (4) Trap TLB VLPT miss
	VECTOR	OS_Trap_Machine_Error		!  (5) Trap Machine error
	VECTOR	OS_Trap_Debug_Related		!  (6) Trap Debug related
	VECTOR	OS_Trap_General_Exception	!  (7) Trap General exception
	VECTOR	OS_Trap_Syscall			!  (8) Syscall
    VECTOR  OS_Trap_Interrupt_HW0           !  (9) Interrupt HW0    ENTRY(HW0) = VECTOR_BASE + (VECOTR_SIZE * 0x9)
    VECTOR  OS_Trap_Interrupt_HW1           ! (10) Interrupt HW1
    VECTOR  OS_Trap_Interrupt_HW2           ! (11) Interrupt HW2
    VECTOR  OS_Trap_Interrupt_HW3           ! (12) Interrupt HW3
    VECTOR  OS_Trap_Interrupt_HW4           ! (13) Interrupt HW4
    VECTOR  OS_Trap_Interrupt_HW5           ! (14) Interrupt HW5
    VECTOR  OS_Trap_Interrupt_HW6           ! (15) Interrupt HW6    SWI in INTC mode
    VECTOR  OS_Trap_Interrupt_HW7           ! (16) Interrupt HW7
    VECTOR  OS_Trap_Interrupt_HW8           ! (17) Interrupt HW8
    VECTOR  OS_Trap_Interrupt_HW9           ! (18) Interrupt HW9    SWI in 32IVIC
    VECTOR  OS_Trap_Interrupt_HW10          ! (19) Interrupt HW10
    VECTOR  OS_Trap_Interrupt_HW11          ! (20) Interrupt HW11
    VECTOR  OS_Trap_Interrupt_HW12           ! (21) Interrupt HW12
    VECTOR  OS_Trap_Interrupt_HW13          ! (22) Interrupt HW13
    VECTOR  OS_Trap_Interrupt_HW14          ! (23) Interrupt HW14
    VECTOR  OS_Trap_Interrupt_HW15          ! (24) Interrupt HW15
    VECTOR  OS_Trap_Interrupt_HW16          ! (25) Interrupt HW16
    VECTOR  OS_Trap_Interrupt_HW17          ! (26) Interrupt HW17
    VECTOR  OS_Trap_Interrupt_HW18          ! (27) Interrupt HW18
    VECTOR  OS_Trap_Interrupt_HW19          ! (28) Interrupt HW19
    VECTOR  OS_Trap_Interrupt_HW20          ! (29) Interrupt HW20
    VECTOR  OS_Trap_Interrupt_HW21          ! (30) Interrupt HW21
    VECTOR  OS_Trap_Interrupt_HW22          ! (31) Interrupt HW22
    VECTOR  OS_Trap_Interrupt_HW23          ! (32) Interrupt HW23
    VECTOR  OS_Trap_Interrupt_HW24          ! (33) Interrupt HW24
    VECTOR  OS_Trap_Interrupt_HW25          ! (34) Interrupt HW25
    VECTOR  OS_Trap_Interrupt_HW26          ! (35) Interrupt HW26
    VECTOR  OS_Trap_Interrupt_HW27          ! (36) Interrupt HW27
    VECTOR  OS_Trap_Interrupt_HW28          ! (37) Interrupt HW28
    VECTOR  OS_Trap_Interrupt_HW29          ! (38) Interrupt HW29
    VECTOR  OS_Trap_Interrupt_HW30          ! (39) Interrupt HW30
    VECTOR  OS_Trap_Interrupt_HW31          ! (40) Interrupt HW31


OS_Int_Vectors_End:


!******************************************************************************************************
! 	Start Entry
!******************************************************************************************************




!------------------------------------------------------------------------------
! NMI watchdog code
!------------------------------------------------------------------------------
NMI_wdt_handler:
	
	smw.adm $r0, [$sp], $r5, 0x2
	smw.adm $r15, [$sp], $r15, 0x0


	! Check the WDT interrupt
	!li	$r1, CPE_WDT_BASE
	!lwi	$r0, [$r1 + 0x1c]
	!beqz38	$r0, 2f

	! Goto the WDT interrupt service routine
	bal	os_wdt_handler
2:

	lmw.bim	$r15, [$sp], $r15, 0x0
	lmw.bim	$r0, [$sp], $r5, 0x2
	iret

!------------------------------------------------------------------------------
! Startup code
!------------------------------------------------------------------------------
	.section .nds32_init.start
	.global _start
	.weak _call_exit
	.weak _SDA_BASE_
	.weak _FP_BASE_
	.func _start
	.type _start, @function

	.align 2
_start:

Reset_NMI:
	/* NMI (Non Maskable Interrupt handler, Start) */ 
	mfsr $r1, $ITYPE
	andi $r2, $r1, 0xfff
	addi $r2, $r2, -2
	beqz $r2, NMI_wdt_handler






	.global _start
 OS_Init_Nds32:
	!***********use to check if C code point zero error,************add by wp********just for test******
#if DEBUG_C_POINT_ZERO
	!	if(*(u32 *)(ITCM_ADDRESS+ITCM_LEN-4)==0x45671237){
	movi $r1,#0x1fffc
	sethi $r0,#0x45671
	lwi450 $r5,[$r1]
	addi $r2,$r0,#0x237
	bnes38 $r2,__OS_Init_Nds32    !<if *(u32 *)(ITCM_ADDRESS+ITCM_LEN-4)!=0x45671237 goto __OS_Init_Nds32>
	movi $r0,#0xff
	CallFn  OS_Exception !OS_Exception(0xff);
 __OS_Init_Nds32:
	!	else	*(u32 *)(ITCM_ADDRESS+ITCM_LEN-4) = 0x45671237;
	swi450 $r2,[$r1]
#endif 


	!****************************************add by wp end********just for test******

	!************************** Begin of do-not-modify **************************
	! Please don't modify this code
	! Initialize the registers used by the compiler

	nds32_init	! NDS32 startup initial macro in <nds32_init.inc>

	!*************************** End of do-not-modify ***************************


#ifdef XIP_MODE	
	bal _nds32_init_mem 
#endif
#ifdef CONFIG_OSC_SUPPORT
	bal	_osc_init
#endif
	bal	__pre_c_init
	bal	__post_c_init
	bal	main



	.section .nds32_init.start
	.align 4
	! Redirect unhandled exceptions / interrupts

OS_Trap_Default_TLB_Fill:
	li	$r0, 0x01
	b	OS_Exception

	.align 4
OS_Trap_Default_PTE_Not_Present:
	li	$r0, 0x02
	b	OS_Exception

	.align 4
OS_Trap_Default_TLB_Misc:
	li	$r0, 0x03
	b	OS_Exception

	.align 4
OS_Trap_Default_TLB_VLPT_Miss:
	li	$r0, 0x04
	b	OS_Exception

	.align 4
OS_Trap_Default_Machine_Error:
	li	$r0, 0x05
	b	OS_Exception

	.align 4
OS_Trap_Default_Debug_Related:
	li	$r0, 0x06
	b	OS_Exception

	.align 4
OS_Trap_Default_General_Exception:
	li	$r0, 0x07
	b	OS_Exception

	.align 4
OS_Trap_Default_Syscall:
1:	b	1b
	mfsr	$r0, $IPC
	addi	$r0, $r0, #4
	push	$r0
	mfsr	$r0, $IPSW
	push	$r0

	lwi	$r0, [$sp + #8]

!	CallFn	gloss_syscall

	pop	$r1
	mtsr	$r1, $IPSW
	pop	$r1
	mtsr	$r1, $IPC
	addi	$sp, $sp, #4

	iret
	
	.align 4
OS_Trap_Interrupt_HW0:
	li	$r1, 0x00
	j	OS_Trap_Int_Comm

	.align 4
OS_Trap_Interrupt_HW1:
	li	$r1, 0x01
	j	OS_Trap_Int_Comm


	.align 4
OS_Trap_Interrupt_HW2:
	li	$r1, 0x02
	j	OS_Trap_Int_Comm


	.align 4
OS_Trap_Interrupt_HW3:
	li	$r1, 0x03
	j	OS_Trap_Int_Comm


	.align 4
OS_Trap_Interrupt_HW4:
	li	$r1, 0x04
	j	OS_Trap_Int_Comm


	.align 4
OS_Trap_Interrupt_HW5:
	li	$r1, 0x05
	j	OS_Trap_Int_Comm


	.align 4
OS_Trap_Interrupt_HW6:
	li	$r1, 0x06
	j	OS_Trap_Int_Comm

	.align 4
OS_Trap_Interrupt_HW7:
	li	$r1, 0x07
	j	OS_Trap_Int_Comm

	.align 4
OS_Trap_Interrupt_HW8:
	li	$r1, 0x08
	j	OS_Trap_Int_Comm


	.align 4
OS_Trap_Interrupt_HW9:
	li	$r1, 0x09
	j	OS_Trap_Int_Comm


	.align 4
OS_Trap_Interrupt_HW10:
	li	$r1, 10
	j	OS_Trap_Int_Comm


	.align 4
OS_Trap_Interrupt_HW11:
	li	$r1, 11
	j	OS_Trap_Int_Comm

	.align 4
OS_Trap_Interrupt_HW12:
	li	$r1, 12
	j	OS_Trap_Int_Comm


	.align 4
OS_Trap_Interrupt_HW13:
	li	$r1, 13
	j	OS_Trap_Int_Comm

#if (CHIP_TYPE==CHIP_ATHENAB)
	.align 4
OS_Trap_Interrupt_HW14:
	li	$r1, 14
	j	OS_Trap_Int_Comm
#else 
!!//ARES
OS_Trap_Interrupt_HW15:
	li	$r1, 15
	j	OS_Trap_Int_Comm
#endif 
	.align 4
OS_Trap_Interrupt_HW16:
	li	$r1, 16
	j	OS_Trap_Int_Comm
	
	.align 4
OS_Trap_Interrupt_HW17:
	li	$r1, 17
	j	OS_Trap_Int_Comm
	
	.align 4
OS_Trap_Interrupt_HW18:
	li	$r1, 18
	j	OS_Trap_Int_Comm
	
	.align 4
OS_Trap_Interrupt_HW19:
	li	$r1, 19
	j	OS_Trap_Int_Comm
	
	.align 4
OS_Trap_Interrupt_HW20:
	li	$r1, 20
	j	OS_Trap_Int_Comm

	
	.align 4
OS_Trap_Interrupt_HW21:
	li	$r1, 21
	j	OS_Trap_Int_Comm
	
	.align 4
OS_Trap_Interrupt_HW22:
	li	$r1, 22
	j	OS_Trap_Int_Comm
	
	.align 4
OS_Trap_Interrupt_HW23:
	li	$r1, 23
	j	OS_Trap_Int_Comm
	
	.align 4
OS_Trap_Interrupt_HW24:
	li	$r1, 24
	j	OS_Trap_Int_Comm
	
	.align 4
OS_Trap_Interrupt_HW25:
	li	$r1, 25
	j	OS_Trap_Int_Comm
	
	.align 4
OS_Trap_Interrupt_HW26:
	li	$r1, 26
	j	OS_Trap_Int_Comm
	
	.align 4
OS_Trap_Interrupt_HW27:
	li	$r1, 27
	j	OS_Trap_Int_Comm
	
	.align 4
OS_Trap_Interrupt_HW28:
	li	$r1, 28
	j	OS_Trap_Int_Comm
	
	.align 4
OS_Trap_Interrupt_HW29:
	li	$r1, 29
	j	OS_Trap_Int_Comm


	.align 4
OS_Trap_Interrupt_HW30:
	li	$r1, 30
	j	OS_Trap_Int_Comm
	
	.align 4
OS_Trap_Interrupt_HW31:
	li	$r1, 31
	j	OS_Trap_Int_Comm



!OS_Trap_Default_Int_Handler:
!	j	OS_Trap_Default_Int_Handler


!OS_Unused:
!	push	$r1
!#ifdef CONFIG_PLAT_AG101P_16MB
!	li	$r1, 0x00e01ffc
!#else
!	li	$r1, 0x902ffffc
!#endif
!	swi	$r0, [$r1]
!	b	OS_Exception

!	mfsr	$r0, $IPC
!	addi	$r0, $r0, #4
!	mtsr	$r0, $IPC

!	pop	$r1
!	pop	$r0
!	iret

!OS_Exception:
!	push	$r1
!	li	$r1, 0x902ffffc
!	swi	$r0, [$r1]
!die:
!	b	die

!	mfsr	$r0, $IPC
!	addi	$r0, $r0, #4
!	mtsr	$r0, $IPC

!	pop	$r1
!	pop	$r0
!	iret

!********************************************************************************************************
!                   __pre_c_init
!
! Note(s) : 1) __pre_c_init init SP pointer, cache,
!
!********************************************************************************************************
	.align  2
	.type   __pre_c_init, @function
__pre_c_init:

	! Processor should be in Super User mode
	! Disable interrupts, change to interruption level 0

	mfsr	$r0, $PSW
	li	$r1, #~PSW_MSK
	and	$r0, $r0, $r1
	li	$r1, #PSW_INIT
	or	$r0, $r0, $r1
	mtsr	$r0, $PSW
	isb

	mfsr	$r0, $INT_MASK
	li	$r1, #~INT_MASK_MSK
	and	$r0, $r0, $r1
	li	$r1, #INT_MASK_INIT
	or	$r0, $r0, $r1
	mtsr	$r0, $INT_MASK

#if(NO_EXTERNAL_INT_CTL==1)
	li	$r0, (1<<9)
!	li	$r0, #IVIC_MSK
	mtsr	$r0, $INT_MASK2

	#if 0
		li	$r0, #INT_PRIO_MASK1
		mtsr	$r0, $INT_PRI
		li	$r0, #INT_PRIO_MASK2
		mtsr	$r0, $INT_PRI2
		li	$r0, 0
		mtsr    $r0, $INT_CTRL
	#endif
#endif

	mfsr	$r0, $IVB
	li	$r1, #~IVB_MSK
	and	$r0, $r0, $r1
	li	$r1, #IVB_INIT
	or	$r0, $r0, $r1
	mtsr	$r0, $IVB

#if (defined(CONFIG_CPU_ICACHE_ENABLE) || defined(CONFIG_CPU_DCACHE_ENABLE))
	mfsr	$r0, $MMU_CTL
	li	$r1, #~MMU_CTL_MSK
	and	$r0, $r0, $r1
	li	$r1, #MMU_CTL_INIT
	or	$r0, $r0, $r1
	mtsr	$r0, $MMU_CTL

	! check $CACHE_CTL existing or not
	mfsr	$r0, $ICM_CFG		! $r0 = ICM_CFG
	mfsr	$r1, $DCM_CFG		! $r1 = DCM_CFG
	or	$r0, $r0, $r1		! $r0 = $r0 | $r1
	andi	$r0, $r0, 0x1c0		! $r0 = $r0 & 0x1c0 , take (DSZ | ISZ)
	beqz	$r0, 3f

	mfsr	$r0, $CACHE_CTL
	li	$r1, #~CACHE_CTL_MSK
	and	$r0, $r0, $r1
	li	$r1, #CACHE_CTL_INIT
	or	$r0, $r0, $r1
	mtsr	$r0, $CACHE_CTL
#endif

3:
#if defined(__NDS32_EXT_FPU_DP__) || defined(__NDS32_EXT_FPU_SP__)
#if 0
// We don't need to do FPU initial anymore, since it is done in nds32_init macro.
	! check fpu existing or not and enable if existing
	li	$r1, #0x80000001
	mfsr	$r2, $FUCOP_EXIST
	and	$r2, $r2, $r1
	bne	$r1, $r2, 4f
	mfsr	$r0, $FUCOP_CTL
	ori	$r0, $r0, #0x1
	mtsr	$r0, $FUCOP_CTL
	dsb

	! Enable denormalized flush-to-Zero mode
	fmfcsr	$r0
	ori	$r0, $r0, #0x1000
	fmtcsr	$r0
	dsb
#endif
4:
#endif
#if 0
	! The C runtime initialization could have been done
	! or will be done somewhere else. But do it again
	! does not hurt.

	! We do this on a word basis.
	! Currently, the default linker script guarantee
	! the __bss_start/_end boundary word-aligned.

	! Clear bss
	la      $r0, __bss_start
	la      $r1, _end
        sub     $r2, $r1, $r0		! $r2: Size of .bss
	beqz    $r2, clear_end

        andi    $r7, $r2, 0x1f		! $r7 = $r2 mod 32
        movi    $r3, 0
        movi    $r4, 0
        movi    $r5, 0
        movi    $r6, 0
        movi    $r8, 0
        movi    $r9, 0
        movi    $r10, 0
        beqz    $r7, clear_loop         !if $r7 == 0, bss_size%32 == 0
        sub     $r2, $r2, $r7

first_clear:
	swi.bi  $r3, [$r0], #4		!clear each word
        addi    $r7, $r7, -4
        bnez    $r7, first_clear
        li      $r1, 0xffffffe0
        and     $r2, $r2, $r1           !check bss_size/32 == 0 or not
        beqz    $r2, clear_end          !if bss_size/32 == 0 , needless to clear

clear_loop:
        smw.bim $r3, [$r0], $r10        !clear each 8 words
        addi    $r2, $r2, -32
        bgez    $r2, clear_loop

clear_end:

#endif
	! Set-up the stack pointer
	la	$sp, _stack

	ret
	.size   __pre_c_init, .-__pre_c_init

	.align  2
	.type   __post_c_init, @function
__post_c_init:


#if (defined(CONFIG_CPU_ICACHE_ENABLE) || defined(CONFIG_CPU_DCACHE_ENABLE))
	! check $CACHE_CTL existing or not
	mfsr	$r0, $ICM_CFG		! $r0 = ICM_CFG
	mfsr	$r1, $DCM_CFG		! $r1 = DCM_CFG
	or	$r0, $r0, $r1		! $r0 = $r0 | $r1
	andi	$r0, $r0, 0x1c0		! $r0 = $r0 & 0x1c0 , take (DSZ | ISZ)
	beqz	$r0, 1f

	mfsr	$r0, $MMU_CTL
	li	$r1, #~MMU_CTL_MSK
	and	$r0, $r0, $r1
	li	$r1, #MMU_CTL_INIT_CACHE_ON
	or	$r0, $r0, $r1
	mtsr	$r0, $MMU_CTL

	mfsr	$r0, $CACHE_CTL
	li	$r1, #~CACHE_CTL_MSK
	and	$r0, $r0, $r1
	li	$r0, #CACHE_CTL_INIT

#ifdef CONFIG_CPU_ICACHE_ENABLE
	push	$r0
	CallFn	n12_icache_flush;
	pop	$r0
	li	$r1, (0x1 << CACHE_CTL_offIC_EN)
	or	$r0, $r0, $r1
#endif

#ifdef CONFIG_CPU_DCACHE_ENABLE
	push	$r0
	CallFn	n12_dcache_invalidate;
	pop	$r0

	li	$r1, (0x1 << CACHE_CTL_offDC_EN)
	or	$r0, $r0, $r1
#endif

	mtsr	$r0, $CACHE_CTL
#endif
1:
	! Perform BSP initialization
	CallFn	BSP_Init
	ret
	.size   __post_c_init, .-__post_c_init

	.align	2
	.type	main, @function
main:
	! Call aos_main(fisrt_available_memory), and never go back

	la	$r0, _end
	CallFn	aos_main

	! Infinite loop, if returned from APP_Init accidently
1:
	b	1b
	.size   main, .-main

	.end

