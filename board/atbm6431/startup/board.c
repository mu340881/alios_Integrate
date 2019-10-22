/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "k_config.h"
#include "board.h"
#include "k_api.h"

void flash_partition_init(void);

/**
  * @general board init entry board_init
  * @retval None
  */
void board_init(void)
{
    /*mm heap set*/
    //aos_heap_set();

    /*choosen to init flash partition , for kv¡¢flash¡¢ota and so on*/
    flash_partition_init();
    /*user code begin*/


    /*user code end*/
}

/*user code begin*/



#if (RHINO_CONFIG_MM_TLF > 0)

k_mm_region_t g_mm_region[2];
int           g_region_num = 2;

//os_heap_set()
void sys_mem_init(unsigned char * sram,unsigned int sramlen, unsigned char * heap,unsigned int heaplen)
{
    g_mm_region[0].start = heap;    /* heap defined in assembler file*/
    g_mm_region[0].len   = heaplen;
    g_mm_region[1].start = sram;    /* heap defined in assembler file*/
    g_mm_region[1].len   = sramlen;
	//printf("sys_mem_init %x len %d\n",sram,sramlen);
	
    /*kernel init, malloc can use after this!*/
    krhino_init();
}
#endif  //#if (RHINO_CONFIG_MM_TLF == 0)
#if (RHINO_CONFIG_MM_LEAKCHECK > 0 )

extern uint32_t __bss_start, _end, __data_start, _edata;

void aos_mm_leak_region_init(void)
{
#if (RHINO_CONFIG_MM_DEBUG > 0)
    krhino_mm_leak_region_init(&__bss_start, &_end);
    krhino_mm_leak_region_init(&__data_start, &_edata);
#endif
}

#endif



