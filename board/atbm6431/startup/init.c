#include "n12_def.h"
#if (CHIP_TYPE==CHIP_ARES_B)
#include "cfg_cw1260_iot_fpga.h"
//#include "app_AT_cmd.h"
#endif
#include "atbm_hal.h"
#include "config/app_config.h"


#define REG32(x)	(*(volatile unsigned long*)(x))


#ifdef XIP_MODE
/* This must be a leaf function, no child function */
void _nds32_init_mem(void) __attribute__((no_prologue, optimize("Os")));

NO_FLASH_FUNC void _nds32_init_mem()
{
	__nds32__mtsr(EDLM_BASE|0x01,NDS32_SR_DLMB);
	__nds32__dsb();
}
#endif

//copy data section, clean bss
NO_FLASH_FUNC NORELOC inline void  __c_init()
{

#ifdef CFG_B2B_SIMU
#define CPU_PRINTF    DBG_Printf
#else
#define CPU_PRINTF(...)    
#endif //CFG_B2B_SIMU


//#define os_memcpy 			memcpy

/* Use compiler builtin memcpy and memset */
//#define MEMCPY(des, src, n) __builtin_memcpy ((des), (src), (n))
//#define MEMSET(s, c, n) __builtin_memset ((s), (c), (n))
/* Use compiler builtin memcpy and memset */
#define MEMCPY(des, src, n) memcpy ((des), (src), (n))
#define MEMSET(s, c, n) 	memset ((s), (c), (n))


	extern char __zi_sram_start;
	extern char __zi_sram_end;
	int size;

	/* If we load code to RAM we don't need to copy
	 * data section and lma will be equal to vma.
	 * */
	/* data section will be copied before we remap.
	 * We don't need to copy data section here. */
#if (CHIP_TYPE==CHIP_ARES_B)
#if  (CACHE == 1)
	extern char __code_sram_end;
	extern char __code_sram_start;
	extern char __code_sram_bin_start;
#endif
#if  (CACHE_64K == 1)
	extern char __code_itcm_end;
	extern char __code_itcm_start;
	extern char __code_itcm_bin_start;
#endif
#endif //(CHIP_TYPE==CHIP_ARES_B)

	/* Clear bss section */
	size = &__zi_sram_end - &__zi_sram_start;

#ifndef CFG_B2B_SIMU 
	MEMSET(&__zi_sram_start, 0, size);
#endif //CFG_B2B_SIMU

#if (CHIP_TYPE==CHIP_ARES_B)
#if  (CACHE_64K == 1)
//                0x0002a000                FLASH_ROM_BASE = (_DLM_SIZE + _ILM_SIZE)
//	        0x00094828	       __code_itcm_bin_start = LOADADDR (.EILM_REGION_CODE)
	//size = &__code_itcm_end - &__code_itcm_start;
	//MEMCPY(&__code_itcm_start, (&__code_itcm_bin_start-&FLASH_ROM_BASE+FLASH_CODE2_SECTION_ADDR), size);
#elif (CACHE)//(CACHE_64K == 0)
	/* copy sram code section */
	size = &__code_sram_end - &__code_sram_start;
	MEMCPY(&__code_sram_start, (&__code_sram_bin_start-&FLASH_ROM_BASE+FLASH_CODE2_SECTION_ADDR), size);
#endif  //(CACHE_64K == 0)
#endif	//(CHIP_TYPE==CHIP_ARES_B)

	return;
}


NO_FLASH_FUNC NORELOC void __attribute__ ((optimize("Os"))) __cpu_init()
{
	unsigned int tmp;

	/* turn on BTB */
	tmp = 0x0;
	__nds32__mtsr(tmp, NDS32_SR_MISC_CTL);

	/* disable all hardware interrupts */
	__nds32__mtsr(0x0, NDS32_SR_INT_MASK);
#if (defined(NDS32_BASELINE_V3M) || defined(NDS32_BASELINE_V3))
	if (__nds32__mfsr(NDS32_SR_IVB) & 0x01)
		__nds32__mtsr(0x0, NDS32_SR_INT_MASK);
#endif

	/* set IVIC, vector size: 4 bytes, base: 0x0
	 * If we use v3/v3m toolchain and want to use
	 * assembly version please don't use USE_C_EXT
	 * in CFLAGS */
	__nds32__mtsr(0x0|DEFAULT_IVB_ADDRESS, NDS32_SR_IVB);
	
	/* Set PSW INTL to 0 */
	tmp = __nds32__mfsr(NDS32_SR_PSW);
	tmp = tmp & 0xfffffff9;
#if (defined(NDS32_BASELINE_V3M) || defined(NDS32_BASELINE_V3))
	/* Set PSW CPL to 7 to allow any priority */
	tmp = tmp | 0x70008;
#endif
	__nds32__mtsr(tmp, NDS32_SR_PSW);
	__nds32__dsb();
#if (defined(NDS32_BASELINE_V3M) || defined(NDS32_BASELINE_V3))
	/* Check interrupt priority programmable*
	* IVB.PROG_PRI_LVL
	*      0: Fixed priority       -- no exist ir18 1r19
	*      1: Programmable priority
	*/
	if (__nds32__mfsr(NDS32_SR_IVB) & 0x01) {
		/* Set PPL2FIX_EN to 0 to enable Programmable
	 	* Priority Level */
		__nds32__mtsr(0x0, NDS32_SR_INT_CTRL);
		/* Check IVIC numbers (IVB.NIVIC) */
		if ((__nds32__mfsr(NDS32_SR_IVB) & 0x0E)>>1 == 5) {        // 32IVIC
			/* set priority HW9: 0, HW13: 1, HW19: 2,
			* HW#-: 0 */
			__nds32__mtsr(0x04000000, NDS32_SR_INT_PRI);
			__nds32__mtsr(0x00000080, NDS32_SR_INT_PRI2);
		} else {
			/* set priority HW0: 0, HW1: 1, HW2: 2, HW3: 3
			* HW4-: 0 */
			__nds32__mtsr(0x000000e4, NDS32_SR_INT_PRI);
		}
	}
#endif
	/* enable FPU if the CPU support FPU */
#if defined(NDS32_EXT_FPU_DP) || defined(NDS32_EXT_FPU_SP)
	tmp = __nds32__mfsr(NDS32_SR_FUCOP_EXIST);
	if ((tmp & 0x80000001) == 0x80000001) {
		tmp = __nds32__mfsr(NDS32_SR_FUCOP_CTL);
		__nds32__mtsr((tmp | 0x1), NDS32_SR_FUCOP_CTL);
	}
#endif
	return;
}


NORELOC  void ITCM lmac_Interrupt_Init(void)
{
	//ulCriticalNesting = 0;
   /* set IRQ_WIFI_IRQ priority to level 1 low */
	__nds32__mtsr(0x55555555, NDS32_SR_INT_PRI);
	__nds32__mtsr(0x55555555, NDS32_SR_INT_PRI2);

	/* enable*/
	__nds32__mtsr(0, NDS32_SR_INT_MASK2);
	/* Interrupt pending register, write 1 to clear */
	__nds32__mtsr(0xFFFFFFFF, NDS32_SR_INT_PEND2);

}
#if (FPGA == 0)
NO_FLASH_FUNC void change_cpu_freq(UINT8 cpu_frq)
{
	UINT8 cur_cpu_frq;

	cur_cpu_frq = inw(0x16100010) & 0x01;

	if(cpu_frq != cur_cpu_frq)
	{
	//ahb to apb en phase
#if((CHIP_TYPE == CHIP_ARES) || ((CHIP_TYPE == CHIP_ARES_B)))
	#ifdef CFG_B2B_SIMU
		outw(0x18e00014,0x300);
	#else
		outw(0x18e00014,0x100);
	#endif

#else //#if(CHIP_TYPE == CHIP_ATHENAB)
	#ifdef CFG_B2B_SIMU
		outw(0x18e00014,0x300);
	#else
		outw(0x18e00014,0x300);
	#endif
#endif //
		GIE_DISABLE();
		outw(0x16100010,cpu_frq);
		__nds32__standby_wait_done();
		GIE_ENABLE();
	}
}
#endif

NO_FLASH_FUNC void BSP_InitIntCtrl(void)
{
	/* use compiler builtin memcpy */
//#define MEMCPY(des, src, n)     __builtin_memcpy ((des), (src), (n))
	/* Copy RW From LMA to VMA */
//	extern char __rw_lma_start;
//	extern char __rw_lma_end;
//	extern char __rw_vma_start;
//	unsigned int size = &__rw_lma_end - &__rw_lma_start;
//	iot_printf("mem copy start %x:%x:%x len %x \n",&__rw_vma_start,&__rw_lma_start,&__rw_lma_end,size);
//	MEMCPY(&__rw_vma_start, &__rw_lma_start, size);
	//__cpu_init();
	//change_cpu_freq(1/*CPU_FRQ_TO_160MHZ*/);
	//PWR_Enable_DPLL(); //Enable Dpll

	__c_init(); 
	lmac_Interrupt_Init();
}

/*
 *********************************************************************************************************
 *					TICKER INITIALIZATION
 *
 * Description : This function is called to initialize tick source (typically a timer generating
 *		 interrupts every 1 to 100 mS).
 *
 *		 We decided to use Timer #0 as the tick interrupt source.
 *
 * Arguments   : none
 *
 * Notes       :
 *********************************************************************************************************
 */

void Tmr_Init(void)
{
	//unsigned int NumCh = REG32(PIT_CFG) & PIT_CH_NUM_MASK;
	//if ( NumCh > 0 )
	//{
	//	REG32(PIT_INT_EN) = 0x0;   	 /* disable all timer interrupt */
	//	REG32(PIT_CH_EN)  = 0x0;	 /* disable all timer */
	//	REG32(PIT_INT_ST) = 0x7;	 /* clear pending events */
	//}
	//else
	//	iot_printf("Tmr_Init Fail, no avalible channel\n");

}



extern void  TI_Cnt1IrqHandler(unsigned Priority);
extern void  APB_CNT_Init(void);
extern void IRQ_RegisterHandler(unsigned IrqNum, IRQ_HANDLER IrqHandler)

NO_FLASH_FUNC void Tmr_TickInit(void)
{
	/* FIXME add trigger mode & MASK2 */
	APB_CNT_Init();
	
    IRQ_RegisterHandler(PIT_DEV_INT_NUM, TI_Cnt1IrqHandler);
    IRQ_RegisterHandler(DEV_INT_NUM_PIT_US, TI_Cnt1IrqHandler);
}



