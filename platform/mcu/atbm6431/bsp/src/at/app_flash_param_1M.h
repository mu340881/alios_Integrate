/**************************************************************************************************************
 * altobeam iot at cmd file 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 * Permission is hereby granted for use on Altobeam IOT 6421 only,
 * in which case ,it is free to  redistribute it and/or modify
 *
*****************************************************************************************************************/

#ifndef _FLASH_CONFIG_PARAM_
#define _FLASH_CONFIG_PARAM_

#define FLASH_CODE_UPDATA_FLAG_SECTION_LEN	DEVICE_PAGE_SIZE

#define FLASH_BOOT_CODE_SECTION_ADDR 		(FLASH_BASE+0x000000)  /*struct flash_boot_section*/
#define FLASH_BOOT_PARAM_SECTION_ADDR 		(FLASH_BASE+0x008000)  /*struct flash_ap_config_section*/

#define FLASH_CODE_SECTION_ADDR 			(FLASH_BASE+0x010000)   
#define FLASH_CODE_CONFIG_SECTION_ADDR 		((FLASH_BASE+0x040000)-4096) 
#define FLASH_CODE1_SECTION_ADDR 			(FLASH_BASE+0x010000)  
#define FLASH_CODE2_SECTION_ADDR 			(FLASH_BASE+0x040000)  
#define FLASH_CODE1_SECTION_LEN 			(0x030000)  
#define FLASH_CODE2_SECTION_LEN 			(0x040000-FLASH_CODE_UPDATA_FLAG_SECTION_LEN)  
#define FLASH_CODE_FLAG_SECTION_ADDR		(FLASH_CODE2_SECTION_ADDR + FLASH_CODE2_SECTION_LEN)  /* UPDATE FLAG*/
#define FLASH_BOOT_PARAM_SECTION2_ADDR 		(FLASH_BASE+0x080000)  /*struct flash_ap_config_section*/
#define FLASH_USR_KEY_SECTION2_ADDR 		(FLASH_BASE+0x081000)  /*user key*/
#define FLASH_USR_DATA_SECTION_ADDR 		(FLASH_BASE+0x082000)  /*user data1*/
#define FLASH_USR_DATA_SECTION_LEN           (0xA000)
#define FLASH_USR_DATA_SECTION2_ADDR 		(FLASH_BASE+0x08C000)  /*user data2*/
#define FLASH_USR_DATA_SECTION2_LEN          (0x4000)
#define FLASH_CODE1_UPDATA_SECTION_ADDR		(FLASH_BASE+0x090000)  /* FLASH_CODE1*/
#define FLASH_CODE1_UPDATA_SECTION_LEN		 FLASH_CODE1_SECTION_LEN  /* FLASH_CODE1*/
#define FLASH_CODE2_UPDATA_SECTION_ADDR		(FLASH_BASE+0x0c0000)  /* FLASH_CODE2*/
#define FLASH_CODE2_UPDATA_SECTION_LEN		(FLASH_CODE2_SECTION_LEN)  /* FLASH_CODE2*/
#define FLASH_CODE_UPDATA_FLAG_SECTION_ADDR	(FLASH_CODE2_UPDATA_SECTION_ADDR + FLASH_CODE2_UPDATA_SECTION_LEN)  /* UPDATE FLAG*/
#define FLASH_CODE2_UPDATA_SECTION_END_ADDR (FLASH_CODE_UPDATA_FLAG_SECTION_ADDR + FLASH_CODE_UPDATA_FLAG_SECTION_LEN)

#define BLOCK_BOOT_PARAM_ADDR 			FLASH_BOOT_PARAM_SECTION_ADDR 
#define BLOCK_BOOT_PARAM_LEN 			0x400

#if (CHIP_TYPE == CHIP_ATHENAB)
#define FLASH_OFFSET_MASK               0xffffff
#else
#define FLASH_OFFSET_MASK               0x3fffff //FLASH_BASE-1
#endif //





#define BLOCK_UART_PARAM_ADDR (FLASH_BOOT_PARAM_SECTION_ADDR + BLOCK_BOOT_PARAM_LEN)
#define BLOCK_UART_PARAM_LEN 0x100
#define BLOCK_TCP_PARAM_ADDR (BLOCK_UART_PARAM_ADDR + BLOCK_UART_PARAM_LEN)
#define BLOCK_TCP_PARAM_LEN  0x100


#endif /*_FLASH_CONFIG_PARAM_*/

