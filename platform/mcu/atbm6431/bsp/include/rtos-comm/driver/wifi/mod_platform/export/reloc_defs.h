/* -*-H-*-
*******************************************************************************
* altobeam
* Reproduction and Communication of this document is strictly prohibited
* unless specifically authorized in writing by altobeam
******************************************************************************/
/**
* \file
* \ingroup PLATFORM
*
* \brief Relocation constant definitions
*/
/*
***************************************************************************
* Copyright (c) 2007 altobeam Ltd
* Copyright altobeam, 2009 ?All rights reserved.
*
* This information, source code and any compilation or derivative thereof are
* the proprietary information of altobeam and/or its licensors and are
* confidential in nature. Under no circumstances is this software to be exposed
* to or placed under an Open Source License of any type without the expressed
* written permission of altobeam.
*
* Although care has been taken to ensure the accuracy of the information and the
* software, altobeam assumes no responsibility therefore.
*
* THE INFORMATION AND SOFTWARE ARE PROVIDED "AS IS" AND "AS AVAILABLE".
*
* altobeam MAKES NO REPRESENTATIONS OR WARRANTIES OF ANY KIND, EITHER EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO WARRANTIES OR MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS,
*
****************************
* Updates
****************************************************************************/

#ifndef RELOC_DEFS_H
#define RELOC_DEFS_H

//#define INDIRECT_ROM_PATCH 0
//#define JUMPTABLE_ROM_PATCH 1

// name of all functions and variables must be made global for initialising reloc table when used

#define QUOTEME(x) #x

//NOTE: can not used this PASRAM or PASRAM_SHARE
#define PASRAM 			__attribute__ ((section ("SM_REGION_PACKET"))) //NOTE: can not used this
#define PASRAM_SHARE 	__attribute__ ((section ("SM_REGION_SHARE")))  //NOTE:can not used this

//
#define PASRAM_ZI 		__attribute__ ((section ("SM_REGION_ZI")))		//can used this
#define PASRAM_DATA		__attribute__ ((section ("PASRAM_DATA"))) 		//can used this
#define FLASH_FUNC	    __attribute__ ((section (".FLASH_CODE")))
#define FLASH_FUNC2	    __attribute__ ((section (".FLASH_CODE2")))
#define FLASH_CONFIG	__attribute__ ((section (".FLASH_CONFIG")))
#ifdef CONFIG_ALI_OS
#define RECORD_RAM 		__attribute__ ((section ("SM_REGION_ZI")))
#else
#define RECORD_RAM 		
#endif //
   

#if (CHIP_TYPE==CHIP_ARES_B)
#if (CACHE_EILM||CACHE_AHB)
#if CACHE_64K
#define NOCACHE_NOFLASH_FUNC	__attribute__ ((section (".EILM_REGION_CODE")))  __attribute__ ((optimize("Os")))
#if CONFIG_MORE_MEMORY
#define NO_FLASH_FUNC1	 	  FLASH_FUNC2
#define NO_FLASH_FUNC_OS	  __attribute__ ((section (".EILM_REGION_CODE")))  __attribute__ ((optimize("Os")))
#define NO_FLASH_FUNC	   	  FLASH_FUNC2
#define NO_FLASH_FUNC2	      FLASH_FUNC2//__attribute__ ((section(".itcm_text")))
#define NO_FLASH_LWIP_FUNC	  FLASH_FUNC2  
#define NO_FLASH_HMAC_FUNC	  FLASH_FUNC2  
#define NO_FLASH_TX_FUNC	  FLASH_FUNC2  
#define NO_FLASH_RX_FUNC	  FLASH_FUNC2  
#define NO_FLASH_PS_FUNC	  FLASH_FUNC2  
#define NO_FLASH_ENC_FUNC	  FLASH_FUNC2  
#define NO_FLASH_SMARTCONFIG_FUNC	  FLASH_FUNC2 
#else //CONFIG_MORE_MEMORY
#define NO_FLASH_FUNC1	 	  NOCACHE_NOFLASH_FUNC
#define NO_FLASH_FUNC_OS	  __attribute__ ((section (".EILM_REGION_CODE")))  __attribute__ ((optimize("Os")))
#define NO_FLASH_FUNC	   	  FLASH_FUNC
#define NO_FLASH_FUNC2	      FLASH_FUNC2//__attribute__ ((section(".itcm_text")))
#define NO_FLASH_LWIP_FUNC	  FLASH_FUNC  
#define NO_FLASH_HMAC_FUNC	  NOCACHE_NOFLASH_FUNC  
#define NO_FLASH_TX_FUNC	  NOCACHE_NOFLASH_FUNC  
#define NO_FLASH_RX_FUNC	  NOCACHE_NOFLASH_FUNC  
#define NO_FLASH_PS_FUNC	  NOCACHE_NOFLASH_FUNC  
#define NO_FLASH_ENC_FUNC	  NOCACHE_NOFLASH_FUNC  
#define NO_FLASH_SMARTCONFIG_FUNC	  NOCACHE_NOFLASH_FUNC 
#endif //CONFIG_MORE_MEMORY
#else //CACHE_64K
#define NOCACHE_NOFLASH_FUNC	__attribute__ ((section (".SM_REGION_CODE"))) __attribute__ ((optimize("Os"))) //__attribute__ ((section (".flash_text")))
#define NO_FLASH_FUNC	   FLASH_FUNC
#define NO_FLASH_FUNC1	   FLASH_FUNC
#define NO_FLASH_FUNC_OS   FLASH_FUNC
#define NO_FLASH_FUNC2	   FLASH_FUNC//__attribute__ ((section(".itcm_text")))
#define NO_FLASH_LWIP_FUNC	  FLASH_FUNC
#define NO_FLASH_HMAC_FUNC	  FLASH_FUNC  
#endif //CACHE_64K
#else //no CACHE
#define NO_FLASH_FUNC	   		 __attribute__ ((section(".itcm_text")))
#define NOCACHE_NOFLASH_FUNC	 NO_FLASH_FUNC //__attribute__ ((section (".flash_text")))
#define NO_FLASH_FUNC1	   		 FLASH_FUNC
#define NO_FLASH_FUNC_OS   		 NO_FLASH_FUNC
#define NO_FLASH_FUNC2	   		 FLASH_FUNC//__attribute__ ((section(".itcm_text")))
#define NO_FLASH_LWIP_FUNC	  	 FLASH_FUNC
#define NO_FLASH_HMAC_FUNC	  	 FLASH_FUNC  
#define NO_FLASH_TX_FUNC	 	 FLASH_FUNC  
#define NO_FLASH_RX_FUNC	  		NO_FLASH_FUNC  
#define NO_FLASH_PS_FUNC	  		NO_FLASH_FUNC  
#define NO_FLASH_ENC_FUNC	  		FLASH_FUNC  
#define NO_FLASH_SMARTCONFIG_FUNC	  NO_FLASH_FUNC 

#endif //CACHE
#else //(CHIP_TYPE==CHIP_ARES_A / ATHENAB)
#define NOCACHE_NOFLASH_FUNC	 NO_FLASH_FUNC //__attribute__ ((section (".flash_text")))
#define NO_FLASH_FUNC	   		__attribute__ ((section(".itcm_text")))
#define NO_FLASH_FUNC1	   		NO_FLASH_FUNC
#define NO_FLASH_FUNC_OS   		NO_FLASH_FUNC
#define NO_FLASH_FUNC2	   		FLASH_FUNC//__attribute__ ((section(".itcm_text")))
#define NO_FLASH_LWIP_FUNC	  	NO_FLASH_FUNC
#define NO_FLASH_HMAC_FUNC	  	NO_FLASH_FUNC  
#endif

#define NO_FLASH_RDATA	
#define FLASH_FUNC_NET	 FLASH_FUNC //__attribute__ ((section (".flash_text")))
#define FLASH_RDATA		__attribute__ ((section (".flash_rodata")))

	// ITCM is intended for the code to be placed in 32K ITCM
	// ITCM + ITCM are the code that to be placed in the direct TX/RX data path
	// ITCM2 is for the code that is regularly used - impact on overall CPU load
	// ITCM3 is for the code used every 100ms and takes big load, creating jitter  
	// AHB_CODE is for code that must be placed in the NORELOC AHBRAM
#define ITCM           NOCACHE_NOFLASH_FUNC  // __attribute__ ((section(".itcm_text")))
#define ITCM1          FLASH_FUNC //  __attribute__ ((section(".itcm_text")))
#define ITCM2          FLASH_FUNC// __attribute__ ((section(".itcm_text")))
#define ITCM3          FLASH_FUNC// __attribute__ ((section(".itcm_text")))
#define ITCM4          NO_FLASH_PS_FUNC// __attribute__ ((section(".itcm_text"))) //power save function

#define AHBRAM_CODE    FLASH_FUNC // __attribute__ ((section(".itcm_text")))


	
#define DTCM_START     NORELOC  //__attribute__ ((section (".debugaddr")))

	// NORELOC DTCM is intended for the data to be placed in 32K NORELOC DTCM
	// DTCM_2 is for the data that can be placed in the extra NORELOC DTCM
	// ITCM3 is for the code used every 100ms and takes big load, creating jitter  

#define DTCM_CONST      NORELOC  __attribute__ ((section (".dtcm_const")))
#define DTCM            NORELOC __attribute__ ((section (".dtcm_data")))
#define DTCM_ZI         NORELOC __attribute__ ((section (".dtcm_zi")))
#define DTCM2_CONST     NORELOC __attribute__ ((section (".dtcm_const")))
#define DTCM2           NORELOC __attribute__ ((section (".dtcm_data")))
#define DTCM2_ZI        PASRAM_ZI //NORELOC __attribute__ ((section (".dtcm_zi")))	
#define AHBRAM          NORELOC __attribute__ ((section (".ahbram_data")))
#define DTCM_ALLOC      NORELOC __attribute__ ((section (".dtcm_alloc")))	
#define VTRACEMEM       NORELOC  __attribute__ ((section ("AHB_SRAM_BEGIN")))
#define KEEP_FUNC     //__attribute__ ((section(".keep_text"))) 
#if INDIRECT_ROM_PATCH
#define ROM_PATCH_FUNC 	__attribute__((indirect_call)) 
#else
#define ROM_PATCH_FUNC 	//__attribute__((indirect_call))
#endif
#define RAM_PATCH  
#define ROM 





//#define USE_RELOC  1
#if JUMPTABLE_ROM_PATCH
#define USE_RELOC  1
#endif




#if USE_RELOC
#define PATCH __attribute__ ((section (".patch_text")))
#define FNROM(_f)  _f##_def 
#define FNNROM(_f)  _f##_def
#define FNNRAM(_f)  _f##_RAM
#define FNNTCM(_f)  _f##_TCM
#define VROM(_v)   _v##_def
#define VRAM(_v)   _v##_RAM
#define VTCM(_v)   _v##_TCM
#define VNROM(_v)   _v##_def
#define VNRAM(_v)   _v##_RAM
#define VNTCM(_v)   _v##_TCM
//#include "full/wsm_rltab_variables.h"
//#include "full/wsm_rltab_instinit.h"
#include "wsm_rltab_macros.h"
//#include "wsm_rltab_prototypes.h"
//#include "wsm_rltab_instinit.h"

#else // USE_RELOC
#define PATCH 
#define FNROM(_f)  _f
#define FNNROM(_f) _f
#define FNNRAM(_f) _f
#define FNNTCM(_f) _f
#define FNTCM(_f) _f
#define VROM(_v)  _v
#define VRAM(_v)  _v
#define VTCM(_v)  _v
#endif


#ifndef NORELOC
#define NORELOC
#endif //NORELOC


#ifndef UNUSED_ARG
#define UNUSED_ARG(x) (void)x
#endif /* LWIP_UNUSED_ARG */

#endif


