/*
    FreeRTOS V8.0.0 - Copyright (C) 2014 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that has become a de facto standard.             *
     *                                                                       *
     *    Help yourself get started quickly and support the FreeRTOS         *
     *    project by purchasing a FreeRTOS tutorial book, reference          *
     *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
     *                                                                       *
     *    Thank you!                                                         *
     *                                                                       *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    >>! NOTE: The modification to the GPL is included to allow you to distribute
    >>! a combined work that includes FreeRTOS without being obliged to provide
    >>! the source code for proprietary components outside of the FreeRTOS
    >>! kernel.

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available from the following
    link: http://www.freertos.org/a00114.html

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org - Documentation, books, training, latest versions,
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
 * A sample implementation of pvPortMalloc() and vPortFree() that permits
 * allocated blocks to be freed, but does not combine adjacent free blocks
 * into a single larger block (and so will fragment memory).  See heap_4.c for
 * an equivalent that does combine adjacent blocks into single larger blocks.
 *
 * See heap_1.c, heap_3.c and heap_4.c for alternative implementations, and the
 * memory management pages of http://www.FreeRTOS.org for more information.
 */

#include <stdio.h>
#include "stdlib.h"
#include "FreeRTOS.h"
#undef DEBUG_MALLOC


#ifdef DEBUG_MALLOC
#define  MEM_PRINT(...) /*if(memtype == MEM_SRAM) {iot_printf(__VA_ARGS__);}*/
#else
#define  MEM_PRINT(...)
#endif 


extern 	void   OS_EnableFiqIrq(void);

extern void ITCM OS_RestoreFiqIrq(uint32 IState);
extern uint32	ITCM OS_DisableFiqIrq();

#ifndef traceMALLOC
    #define traceMALLOC( pvAddress, uiSize )
#endif
#ifndef traceFREE
    #define traceFREE( pvAddress, uiSize )
#endif



#define SHORT_MEM_LEN 32
#define SHORT_MEM_NUM 32
DTCM   u32 short_mem_map=0xffffffff; 
static PASRAM_ZI u8 short_mem[SHORT_MEM_NUM][SHORT_MEM_LEN];


#define MIDD_MEM_LEN 64
#define MIDD_MEM_NUM 64
DTCM   u32 midd_mem_map=0xffffffff; 
static PASRAM_ZI u8 midd_mem[MIDD_MEM_NUM][MIDD_MEM_LEN];



#define portBYTE_ALIGNMENT 4
#if portBYTE_ALIGNMENT == 4
	#define portBYTE_ALIGNMENT_MASK	( 0x0003 )
#endif

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
all the API functions to use the MPU wrappers.  That should only be done when
task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* Define the linked list structure.  This is used to link free blocks in order
of their size. */
typedef struct A_BLOCK_LINK
{
	struct A_BLOCK_LINK *pxNextFreeBlock;	/*<< The next free block in the list. */
	u32 xBlockSize;						/*<< The size of the free block. include heapSTRUCT_SIZE*/
	u8 ref;	
	u8 type;	
	u8 reserve[2];	
#ifdef DEBUG_MALLOC_LIST
	u32 call_addr;
#endif //DEBUG_MALLOC_LIST
} BlockLink_t __packed;

/*
 * Initialises the heap structures before their first use.
 */
//static void prvHeapInit( void );

#define heapSTRUCT_SIZE	 ( ( sizeof ( BlockLink_t ) + ( portBYTE_ALIGNMENT - 1 ) ) & ~portBYTE_ALIGNMENT_MASK )
#define heapMINIMUM_BLOCK_SIZE	( ( size_t ) ( heapSTRUCT_SIZE * 2 ) )



typedef struct MEM_CONTROL
{
	u8  xHeapHasBeenInitialised;	
	u8	reserved[3];
	/* A few bytes might be lost to byte aligning the heap start address. */
	u32 configADJUSTED_HEAP_SIZE;	
	/* Allocate the memory for the heap. */
	uint8_t * ucHeap ;
	/* Create a couple of list links to mark the start and end of the list. */
	BlockLink_t xStart;
#ifdef DEBUG_MALLOC_LIST
	/* Create a couple of list links to mark the start and end of the list. */
	BlockLink_t malloc_list;
#endif //DEBUG_MALLOC_LIST
	/* Keeps track of the number of free bytes remaining, but says nothing about
	fragmentation. */
    u32 xFreeBytesRemaining;
} MEM_CONTROL;

DTCM_ZI struct MEM_CONTROL memctrl_heap[MEM_MAX];

/* STATIC FUNCTIONS ARE DEFINED AS MACROS TO MINIMIZE THE FUNCTION CALL DEPTH. */

/*-----------------------------------------------------------*/
NO_FLASH_FUNC_OS void *realloc(void *pointer, size_t size)
{
        unsigned char *ret = (unsigned char *)smalloc(size);
		if(ret && pointer){
			memcpy(ret,pointer,size);
			os_free(pointer);
		}
        return ret;
}

NO_FLASH_FUNC_OS  void *zalloc(size_t size)
{
	char * buf =  (char *)smalloc(size);
	if(buf){
		os_memzero(buf,0,size);
	}
	return (void * )buf;
}

 
NO_FLASH_FUNC_OS  void *scalloc(size_t nmemb, size_t size)
{
        int i = nmemb * size;
        unsigned char *ret = (unsigned char *)smalloc(i);

		if(ret){
			//while (i > 0)
            //    ret[--i] = '\0';
			os_memzero(ret,0,i);
		}
		else {
			iot_printf("scalloc err %x\n",size);
		}
		
        return ret;
}
NO_FLASH_FUNC_OS void *calloc(size_t nmemb, size_t size){
	return scalloc(nmemb,size);
}
NO_FLASH_FUNC_OS void *pvPortMalloc(size_t xWantedSize )
{
	void *pvReturn = NULL;
	
	if((xWantedSize < SHORT_MEM_LEN )&&(short_mem_map != 0)){
		int id =0;
		id = CountLeadingZeros(short_mem_map);
		id = 31-id;
		
		short_mem_map &= ~BIT(id);
		pvReturn = (void *)short_mem[id];
		return pvReturn;
	}
	if((xWantedSize < MIDD_MEM_LEN )&&(midd_mem_map != 0)){
		int id =0;
		id = CountLeadingZeros(midd_mem_map);
		id = 31-id;
		
		midd_mem_map &= ~BIT(id);
		pvReturn = (void *)midd_mem[id];
		return pvReturn;
	}
	
	pvReturn=pvPortHeapMalloc_DEBUG(xWantedSize, MEM_HEAP, (unsigned long)__builtin_return_address(0));
	if(pvReturn == NULL){
		pvReturn=pvPortHeapMalloc_DEBUG(xWantedSize, MEM_SRAM, (unsigned long)__builtin_return_address(0));
	}
	return pvReturn;
}


void *pvPortHeapMalloc_DEBUG(size_t xWantedSize, enum MEM_TYPE memtype ,unsigned long call_addr )
{
	BlockLink_t *pxBlock, *pxPreviousBlock, *pxNewBlockLink;
	void *pvReturn = NULL;

	u32 flags  = OS_DisableFiqIrq(); 
	MEM_PRINT("%s memtype[%d],size %d ++ [%d][%x]\n",__func__,memtype,xWantedSize,memctrl_heap[memtype].xHeapHasBeenInitialised,&memctrl_heap[memtype].xHeapHasBeenInitialised);
	{
		/* If this is the first call to malloc then the heap will require
		initialisation to setup the list of free blocks. */
		if( memctrl_heap[memtype].xHeapHasBeenInitialised == FALSE )
		{
			MEM_PRINT("%s memtype[%d],size %d ++ [%d][%x]\n",__func__,memtype,xWantedSize,memctrl_heap[memtype].xHeapHasBeenInitialised,&memctrl_heap[memtype].xHeapHasBeenInitialised);
			//prvHeapInit();
			//xHeapHasBeenInitialised = TRUE;
			KASSERT(0); 
		}

		/* The wanted size is increased so it can contain a BlockLink_t
		structure in addition to the requested amount of bytes. */
		if( xWantedSize > 0 )
		{
			xWantedSize += heapSTRUCT_SIZE;

			/* Ensure that blocks are always aligned to the required number of bytes. */
			if( ( xWantedSize & portBYTE_ALIGNMENT_MASK ) != 0 )
			{
				/* Byte alignment required. */
				xWantedSize += ( portBYTE_ALIGNMENT - ( xWantedSize & portBYTE_ALIGNMENT_MASK ) );
			}
		}

		if( ( xWantedSize > 0 ) && ( xWantedSize < memctrl_heap[memtype].configADJUSTED_HEAP_SIZE ) )
		{
			/* Blocks are stored in byte order - traverse the list from the start
			(smallest) block until one of adequate size is found. */
			pxPreviousBlock = &memctrl_heap[memtype].xStart;
			pxBlock = memctrl_heap[memtype].xStart.pxNextFreeBlock;
			while(( pxBlock != NULL ) && ( pxBlock->xBlockSize < xWantedSize ) )
			{
				pxPreviousBlock = pxBlock;
				pxBlock = pxBlock->pxNextFreeBlock;
			}

			/* If we found the end marker then a block of adequate size was not found. */
			if( pxBlock != NULL )
			{
				/* If the block is larger than required it can be split into two. */
				if( ( pxBlock->xBlockSize - xWantedSize ) > heapMINIMUM_BLOCK_SIZE )
				{
					//pxNewBlockLink is old
					//pxBlock is return
					pxNewBlockLink = ( void * ) ( ( ( uint8_t * ) pxBlock ) + xWantedSize );

					/* Calculate the sizes of two blocks split from the single
					block. */
					pxNewBlockLink->xBlockSize =  pxBlock->xBlockSize - xWantedSize;
					pxBlock->xBlockSize = xWantedSize;
					
					pxNewBlockLink->ref=0;
					pxNewBlockLink->type = memtype;
					//update free list,delete pxBlock,add  pxNewBlockLink
					pxNewBlockLink->pxNextFreeBlock = pxBlock->pxNextFreeBlock;
					pxPreviousBlock->pxNextFreeBlock = pxNewBlockLink;
					
					pxNewBlockLink = pxBlock;
;
				}
				else {
					pxNewBlockLink = pxBlock;
					/* This block is being returned for use so must be taken out of the
					list of free blocks. */
					pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;
				}
				
				pxNewBlockLink->ref=1;
				pxNewBlockLink->type = memtype;
				pvReturn = ( void * ) ( ( ( uint8_t * ) pxNewBlockLink/*pxPreviousBlock->pxNextFreeBlock*/ ) + heapSTRUCT_SIZE );
				memctrl_heap[memtype].xFreeBytesRemaining -= pxNewBlockLink->xBlockSize;
#ifdef DEBUG_MALLOC
				/*if(memtype == MEM_SRAM)*/{
					MEM_PRINT("xFreeBytesRemaining %x \n",memctrl_heap[memtype].xFreeBytesRemaining);
				}
#endif //DEBUG_MALLOC
#ifdef DEBUG_MALLOC_LIST
			memctrl_heap[memtype].malloc_list.xBlockSize++;
			pxPreviousBlock = memctrl_heap[memtype].malloc_list.pxNextFreeBlock;
			pxNewBlockLink->pxNextFreeBlock = pxPreviousBlock;
			memctrl_heap[memtype].malloc_list.pxNextFreeBlock= pxNewBlockLink;
			pxNewBlockLink->call_addr = call_addr;
#endif //DEBUG_MALLOC_LIST
			}
		}

		traceMALLOC( pvReturn, xWantedSize );
	}
	OS_RestoreFiqIrq(flags);
	
#ifdef DEBUG_MALLOC
	if(pvReturn == NULL){
		sys_heapShow(memtype);
	}
#endif /*DEBUG_MALLOC*/
	#if( configUSE_MALLOC_FAILED_HOOK == 1 )
	{
		if( pvReturn == NULL )
		{
			extern void vApplicationMallocFailedHook( void );
			vApplicationMallocFailedHook();
		}
	}
	#endif
	MEM_PRINT("%s memtype[%d],pvReturn 0x%x [%d]\n",__func__,memtype,pvReturn,memctrl_heap[memtype].xHeapHasBeenInitialised);
	//if(memtype == MEM_SRAM){
	//iot_printf("pvReturn 0x%x xWantedSize %d\n",pvReturn,xWantedSize);
	//}
	return pvReturn;
}


/*-----------------------------------------------------------*/
//vport sort in memory address
NO_FLASH_FUNC_OS static void vportPlugHolesInsert(enum MEM_TYPE memtype ,BlockLink_t *pxLink)
{
	BlockLink_t *pxBlock=NULL;
	BlockLink_t	*pxPreviousBlock=NULL;
	BlockLink_t *pxNextBlockLink=NULL;
	uint8_t *nowmem=(uint8_t *)pxLink;
	//uint8_t *premem=NULL;
	uint8_t *nextmem= nowmem  + pxLink->xBlockSize;
	uint8_t find= 0;
	/* Blocks are stored in byte order - traverse the list from the start
	(smallest) block until one of adequate size is found. */
	pxPreviousBlock = &memctrl_heap[memtype].xStart;
	pxBlock = memctrl_heap[memtype].xStart.pxNextFreeBlock;
	while(  ( pxBlock != NULL ) )
	{
		MEM_PRINT("block %x ,next %x,nextmem %x,nowmem %x \n",(uint32_t )pxBlock,(uint32_t )((uint8_t *)pxBlock+heapSTRUCT_SIZE + pxBlock->xBlockSize),nextmem,nowmem);
		if( (uint8_t *)pxBlock == nextmem ){
			find = 1;
			pxPreviousBlock->pxNextFreeBlock = pxLink;
			pxLink->pxNextFreeBlock = pxBlock->pxNextFreeBlock;			
			pxLink->xBlockSize += pxBlock->xBlockSize;
			//pxBlock = pxPreviousBlock;
			MEM_PRINT("===find next=== \n");
			break;
		}
		else if( ((uint8_t *)pxBlock + pxBlock->xBlockSize) == nowmem ){
			find = 1;
			pxBlock->xBlockSize += pxLink->xBlockSize;			
			pxNextBlockLink = pxBlock->pxNextFreeBlock;
			
			//if the free is in the midlle
			if(((uint8_t *)pxBlock + pxBlock->xBlockSize) == (uint8_t *)pxNextBlockLink){
				pxBlock->pxNextFreeBlock = pxNextBlockLink->pxNextFreeBlock;
				pxBlock->xBlockSize += pxNextBlockLink->xBlockSize;
			}
			MEM_PRINT("===find pre=== \n");
			break;
		} 
		else {
			MEM_PRINT("===set Next block === [%x]->[%x]\n",(u32)pxBlock,(u32)pxBlock->pxNextFreeBlock);
			pxPreviousBlock = pxBlock;
			pxBlock = pxBlock->pxNextFreeBlock;

			if(((u32)pxPreviousBlock <(u32)pxLink)
				&& ((u32)pxLink <(u32)pxBlock)
				&& ((u32)nextmem != (u32)pxBlock)){
				
				MEM_PRINT("===find slot=== \n");
				pxPreviousBlock->pxNextFreeBlock = pxLink;
				pxLink->pxNextFreeBlock = pxBlock;
				find = 1;
				break;
			}
		}

	}

	if(!find ){
			//insert first
		if((pxLink<memctrl_heap[memtype].xStart.pxNextFreeBlock)
			|| (memctrl_heap[memtype].xStart.pxNextFreeBlock == NULL)){
			pxLink->pxNextFreeBlock =  memctrl_heap[memtype].xStart.pxNextFreeBlock;
			memctrl_heap[memtype].xStart.pxNextFreeBlock = pxLink;
			
			MEM_PRINT("===insert first === [%x]\n",(u32)pxLink);
		}
		else if(pxPreviousBlock){
			//insert end
			KASSERT(pxPreviousBlock->pxNextFreeBlock == NULL);
			pxPreviousBlock->pxNextFreeBlock = pxLink;
			pxLink->pxNextFreeBlock = NULL;
			MEM_PRINT("===insert end === [%x]->[%x]\n",(u32)pxPreviousBlock,(u32)pxLink);
		}
		else {
			KASSERT(0);
		}
	}
}




NO_FLASH_FUNC_OS void sys_heapfree( void *pv )
{	
	uint8_t *puc = ( uint8_t * ) pv;
	BlockLink_t *pxLink;
	if((short_mem[0] <= (u8 *)pv)&&(short_mem[SHORT_MEM_NUM-1]  >= (u8 *)pv)){
		int id =0;
		for(id=0;id<SHORT_MEM_NUM;id++)
		{
			if(short_mem[id] == (u8 *)pv){
				break;
			}		
		}
		KASSERT(id < SHORT_MEM_NUM);
		short_mem_map |= BIT(id);
		return;
	}
	
	if((midd_mem[0] <= (u8 *)pv)&&(midd_mem[MIDD_MEM_NUM-1]  >= (u8 *)pv)){
		int id =0;
		for(id=0;id<MIDD_MEM_NUM;id++)
		{
			if(midd_mem[id] == (u8 *)pv){
				break;
			}		
		}
		KASSERT(id < MIDD_MEM_NUM);
		midd_mem_map |= BIT(id);
		return;
	}

	if( pv != NULL )
	{
		/* The memory being freed will have an BlockLink_t structure immediately
		before it. */
		puc -= heapSTRUCT_SIZE;

		/* This unexpected casting is to keep some compilers from issuing
		byte alignment warnings. */
		pxLink = ( void * ) puc;
#ifdef DEBUG_MALLOC
		KASSERT((pxLink->type == MEM_SRAM)||(pxLink->type == MEM_HEAP));
		//if((pxLink->type != MEM_SRAM)&&(pxLink->type != MEM_HEAP)){
		//	MEM_PRINT("\n ASSERT %x:%x \n",pxLink->type ,pv);
		//	dump_mem(pv,8);
		//}
		KASSERT(pxLink->ref == 1);
		u8 memtype =pxLink->type;
		MEM_PRINT("===sys_heapfree===type[%d]=[%x] \n",pxLink->type,pv);
#endif /*DEBUG_MALLOC*/
		vPortHeapFree( pv,pxLink->type);

	}
}

NO_FLASH_FUNC_OS void vPortFree( void *pv )
{
	sys_heapfree(pv);
}



#ifdef DEBUG_MALLOC

NO_FLASH_FUNC_OS void sys_heapfree_check( void *pv )
{	
	uint8_t *puc = ( uint8_t * ) pv;
	BlockLink_t *pxLink;
	if( pv != NULL )
	{
		/* The memory being freed will have an BlockLink_t structure immediately
		before it. */
		puc -= heapSTRUCT_SIZE;

		/* This unexpected casting is to keep some compilers from issuing
		byte alignment warnings. */
		pxLink = ( void * ) puc;
		if(((pxLink->type != MEM_SRAM)&&(pxLink->type != MEM_HEAP))
			||(pxLink->ref != 1)){
			DBG_Printf("->WARN:%d:%d\n",pxLink->type,pxLink->ref);
			//dump_mem(pxLink,16);
		}
		
		//vPortHeapFree(pxLink->type , pv);

	}
}
#endif /*DEBUG_MALLOC*/



NO_FLASH_FUNC_OS void vPortHeapFree( void *pv,enum MEM_TYPE memtype )
{
	uint8_t *puc = ( uint8_t * ) pv;
	BlockLink_t *pxLink;
	u32 flag ;
	
#ifdef DEBUG_MALLOC
	/* The memory being freed will have an BlockLink_t structure immediately
	before it. */	
	if(((uint32_t)memctrl_heap[memtype].ucHeap>(uint32_t) pv)
		||((uint32_t)memctrl_heap[memtype].ucHeap+memctrl_heap[memtype].configADJUSTED_HEAP_SIZE <(uint32_t) pv)){
		return;
	}
	
#endif /*DEBUG_MALLOC*/

	if( pv != NULL )
	{
		/* The memory being freed will have an BlockLink_t structure immediately
		before it. */
		puc -= heapSTRUCT_SIZE;

		/* This unexpected casting is to keep some compilers from issuing
		byte alignment warnings. */
		pxLink = ( void * ) puc;
		KASSERT(pxLink->type == memtype);
		KASSERT(pxLink->ref == 1);
		pxLink->ref--;
		
		flag = OS_DisableFiqIrq();
		
#ifdef DEBUG_MALLOC_LIST
		{
			BlockLink_t *pxNowLink,*pxPreviousBlock=NULL;;
			u16 blockcnt =memctrl_heap[memtype].malloc_list.xBlockSize;
			pxNowLink = memctrl_heap[memtype].malloc_list.pxNextFreeBlock;
			while(blockcnt!=0){
				KASSERT(pxNowLink!=NULL);
				if(pxNowLink == pxLink){
					//KASSERT(cond)(_b,__errcode)
					if(pxPreviousBlock==NULL){
						memctrl_heap[memtype].malloc_list.pxNextFreeBlock = pxNowLink->pxNextFreeBlock;
					}
					else {
						pxPreviousBlock->pxNextFreeBlock = pxNowLink->pxNextFreeBlock;
					}
						
					memctrl_heap[memtype].malloc_list.xBlockSize--;
					
					break;		
				}
				pxPreviousBlock= pxNowLink;
				pxNowLink = pxPreviousBlock->pxNextFreeBlock;
				blockcnt--;
			}
			if(blockcnt ==0){
				MEM_PRINT("<vPortHeapFree> can't find free block %x \n",blockcnt);
			}
		}
#endif //DEBUG_MALLOC_LIST
		{
			/* Add this block to the list of free blocks. */
			//prvInsertBlockIntoFreeList( ( ( BlockLink_t * ) pxLink ) );
			memctrl_heap[memtype].xFreeBytesRemaining += pxLink->xBlockSize;
			vportPlugHolesInsert( memtype,( ( BlockLink_t * ) pxLink ) );
			traceFREE( pv, pxLink->xBlockSize );
		}
		OS_RestoreFiqIrq(flag);
	}
#ifdef DEBUG_MALLOC
	if(memtype == MEM_SRAM){
		MEM_PRINT("<DEBUG_SKB> xFreeRemain %x \n",memctrl_heap[memtype].xFreeBytesRemaining);
	}
#endif /*DEBUG_MALLOC*/
}


FLASH_FUNC void sys_heapShow(enum MEM_TYPE memtype)
{
    u32 total_free = 0;
	
	Console_SetPolling(1);
	iot_printf("===========>Type: %d\n",memtype);
	iot_printf("total SIZE: %d\n",memctrl_heap[memtype].configADJUSTED_HEAP_SIZE);
	iot_printf("Free  SIZE: %d\n",memctrl_heap[memtype].xFreeBytesRemaining);

	iot_printf("mstart: 0x%x\n",memctrl_heap[memtype].ucHeap);
	
	{
		BlockLink_t *pxBlock=NULL;
		//BlockLink_t *pxPreviousBlock=NULL;

		/* Blocks are stored in byte order - traverse the list from the start
		(smallest) block until one of adequate size is found. */
		//pxPreviousBlock = &memctrl_heap[memtype].xStart;
		pxBlock = memctrl_heap[memtype].xStart.pxNextFreeBlock;
		while(  ( pxBlock != NULL ) )
		{
			
			iot_printf("bstart: 0x%x\n",(u32)pxBlock);
			iot_printf("bsize: 0x%x\n",(u32)pxBlock->xBlockSize);
			total_free = total_free + (u32)pxBlock->xBlockSize;
			//pxPreviousBlock = pxBlock;
			pxBlock = pxBlock->pxNextFreeBlock;
		}
	}	
	iot_printf("total free: %d\n",total_free);	
	iot_printf("\n");
	
#ifdef DEBUG_MALLOC_LIST

		iot_printf("===>MALLOC_LIST :BlockSize %d\n",memctrl_heap[memtype].malloc_list.xBlockSize);
		{
				BlockLink_t *pxBlock=NULL;
				//BlockLink_t *pxPreviousBlock=NULL;

				/* Blocks are stored in byte order - traverse the list from the start
				(smallest) block until one of adequate size is found. */
				//pxPreviousBlock = &memctrl_heap[memtype].xStart;
				pxBlock = memctrl_heap[memtype].malloc_list.pxNextFreeBlock;
				while(  ( pxBlock != NULL ) )
				{
					
					iot_printf("bstart: 0x%x\n",(u32)pxBlock);
					iot_printf("bsize: 0x%x\n",(u32)pxBlock->xBlockSize);
					iot_printf("call: 0x%x\n",(u32)pxBlock->call_addr);
					//pxPreviousBlock = pxBlock;
					pxBlock = pxBlock->pxNextFreeBlock;
				}
		}
		iot_printf("\n");
#endif //DEBUG_MALLOC_LIST
   Console_SetPolling(0);

}
#ifdef DEBUG_MALLOC
NO_FLASH_FUNC_OS int xPortGetFreeBlockNum(enum MEM_TYPE memtype )
{
	BlockLink_t *pxBlock;
	int blocknum =0;

	/* Blocks are stored in byte order - traverse the list from the start
	(smallest) block until one of adequate size is found. */
	//pxPreviousBlock = &memctrl_heap[memtype].xStart;
	pxBlock = memctrl_heap[memtype].xStart.pxNextFreeBlock;
	while(	( pxBlock != NULL ) )
	{			
		//pxPreviousBlock = pxBlock;
		pxBlock = pxBlock->pxNextFreeBlock;
		blocknum++;
	}
	return blocknum;
}

#endif /*DEBUG_MALLOC*/


static void prvHeapInit( enum MEM_TYPE memtype )
{
	BlockLink_t *pxFirstFreeBlock;
	uint8_t *pucAlignedHeap;

	/* Ensure the heap starts on a correctly aligned boundary. */
	pucAlignedHeap = ( uint8_t * ) ( ( ( u32 ) &memctrl_heap[memtype].ucHeap[ portBYTE_ALIGNMENT ] ) & ( ( u32 ) ~portBYTE_ALIGNMENT_MASK ) );

	/* xStart is used to hold a pointer to the first item in the list of free
	blocks.  The void cast is used to prevent compiler warnings. */
	memctrl_heap[memtype].xStart.pxNextFreeBlock = ( void * ) pucAlignedHeap;
	memctrl_heap[memtype].xStart.xBlockSize = ( size_t ) 0;
#ifdef DEBUG_MALLOC_LIST
	memctrl_heap[memtype].malloc_list.xBlockSize =0;
	memctrl_heap[memtype].malloc_list.pxNextFreeBlock=NULL;
#endif //DEBUG_MALLOC_LIST
	/* To start with there is a single free block that is sized to take up the
	entire heap space. */
	pxFirstFreeBlock = ( void * ) pucAlignedHeap;
	pxFirstFreeBlock->xBlockSize = memctrl_heap[memtype].configADJUSTED_HEAP_SIZE;
	pxFirstFreeBlock->pxNextFreeBlock = NULL;
	memctrl_heap[memtype].xHeapHasBeenInitialised = TRUE;
}

static void sys_heapinit(enum MEM_TYPE memtype ,uint8_t* heap, unsigned int heap_len)
{

	memctrl_heap[memtype].configADJUSTED_HEAP_SIZE = ( heap_len - portBYTE_ALIGNMENT );
	memctrl_heap[memtype].xFreeBytesRemaining =  memctrl_heap[memtype].configADJUSTED_HEAP_SIZE;
	memctrl_heap[memtype].ucHeap = heap;
	memctrl_heap[memtype].xHeapHasBeenInitialised = FALSE;
	//memctrl_heap[memtype].xStart=;
	//memctrl_heap[memtype].xEnd=;
	
	prvHeapInit(memtype);
}

inline void sys_mem_init(unsigned char * sram,unsigned int sramlen, unsigned char * heap,unsigned int heaplen)
{	
	sys_heapinit(MEM_SRAM,sram, sramlen);//used mem_malloc smalloc
	sys_heapinit(MEM_HEAP,heap,heaplen);//used kmalloc
	return;
}

static u32 _sys_mem_free_size_get(enum MEM_TYPE memtype)
{
	u32 total=0;
	BlockLink_t *pxBlock=NULL;

	//point to the first block
	pxBlock = memctrl_heap[memtype].xStart.pxNextFreeBlock;
	while(	( pxBlock != NULL ) )
	{
		//calc block size
		total += (u32)pxBlock->xBlockSize;
		//point to the next block
		pxBlock = pxBlock->pxNextFreeBlock;
	}

	return total;
}

u32 sys_mem_free_size_get(void)
{
	u32 total_free=0;
	
	total_free += memctrl_heap[MEM_HEAP].xFreeBytesRemaining;//_sys_mem_free_size_get(MEM_HEAP);
	total_free += memctrl_heap[MEM_SRAM].xFreeBytesRemaining;//_sys_mem_free_size_get(MEM_SRAM);

	return total_free;
}

#ifdef DEBUG_MALLOC_TEST
void mem_test()
{
	u8 * mtest_buf[100];
	int mtest_len = 0;

 
	MEM_PRINT("===========mem_testkmalloc ============\n");
	mtest_buf[mtest_len++] = kmalloc(0x101); 
	sys_heapShow(MEM_SRAM);
	mtest_buf[mtest_len++] = kmalloc(0x102);
	sys_heapShow(MEM_SRAM);
	mtest_buf[mtest_len++] = kmalloc(0x103);
	sys_heapShow(MEM_SRAM);
	mtest_buf[mtest_len++] = kmalloc(0x104);
	sys_heapShow(MEM_SRAM);
	mtest_buf[mtest_len++] = kmalloc(0x105);
	sys_heapShow(MEM_SRAM);
	mtest_buf[mtest_len++] = kmalloc(0x106);
	sys_heapShow(MEM_SRAM);
	mtest_buf[mtest_len++] = kmalloc(0x107);
	sys_heapShow(MEM_SRAM);
	mtest_buf[mtest_len++] = kmalloc(0x108);
	sys_heapShow(MEM_SRAM);
	mtest_buf[mtest_len++] = kmalloc(0x109);
	sys_heapShow(MEM_SRAM);
	mtest_buf[mtest_len++] = kmalloc(0x10a);
	sys_heapShow(MEM_SRAM);
	MEM_PRINT("===========mem_test kfree ============\n");
	kfree(mtest_buf[0]);
	sys_heapShow(MEM_SRAM);
	kfree(mtest_buf[1]);
	sys_heapShow(MEM_SRAM);
	kfree(mtest_buf[9]);
	sys_heapShow(MEM_SRAM);
	kfree(mtest_buf[8]);
	sys_heapShow(MEM_SRAM);
	kfree(mtest_buf[5]);
	sys_heapShow(MEM_SRAM);
	kfree(mtest_buf[7]);
	sys_heapShow(MEM_SRAM);
	kfree(mtest_buf[2]);
	sys_heapShow(MEM_SRAM);
	kfree(mtest_buf[4]);
	sys_heapShow(MEM_SRAM);
	kfree(mtest_buf[3]);
	sys_heapShow(MEM_SRAM);
	kfree(mtest_buf[6]);
	sys_heapShow(MEM_SRAM);
	
	MEM_PRINT("===========mem_test end ============\n");
}
#endif /*DEBUG_MALLOC*/


//#ifndef DEBUG_MALLOC_LIST
#undef malloc
NO_FLASH_FUNC_OS  __attribute__((used)) void * malloc(size_t xWantedSize)
{
	return pvPortMalloc(xWantedSize);
}
//#endif //#ifdef DEBUG_MALLOC_LIST

#undef free
NO_FLASH_FUNC_OS  __attribute__((used)) void free( void *pv )
{
	vPortFree( pv );
}

NO_FLASH_FUNC_OS void sfree(void *pv )
{
	 vPortHeapFree(pv ,MEM_SRAM );
}
NO_FLASH_FUNC_OS char * smalloc(int  s)
{
 	return pvPortHeapMalloc_DEBUG(s,MEM_SRAM,(unsigned long)__builtin_return_address(0));
}

/*-----------------------------------------------------------*/
