/* -*-H-*-
*******************************************************************************
* ST-Ericsson
* Reproduction and Communication of this document is strictly prohibited
* unless specifically authorized in writing by ST-Ericsson
******************************************************************************/
/**
* \file
* \ingroup WSM SYS
*
* \brief WSM Device Config
*/
/*
***************************************************************************
* Copyright (c) 2007 STMicroelectronics Ltd
* Copyright ST-Ericsson, 2010 鈥�All rights reserved.
*
* This information, source code and any compilation or derivative thereof are
* the proprietary information of ST-Ericsson and/or its licensors and are
* confidential in nature. Under no circumstances is this software to be exposed
* to or placed under an Open Source License of any type without the expressed
* written permission of ST-Ericsson.
*
* Although care has been taken to ensure the accuracy of the information and the
* software, ST-Ericsson assumes no responsibility therefore.
*
* THE INFORMATION AND SOFTWARE ARE PROVIDED "AS IS" AND "AS AVAILABLE".
*
* ST-Ericsson MAKES NO REPRESENTATIONS OR WARRANTIES OF ANY KIND, EITHER EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO WARRANTIES OR MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS,
*
****************************************************************************/
#ifndef CFG_AB_APOLLO_MOD_FPGA_H
#define CFG_AB_APOLLO_MOD_FPGA_H

//#pragma message("======CW1260_FPGA=====")
/* System Clock */
#define MHz         1000000
#ifndef CFG_B2B_SIMU
#define APB_CLK		(40 * MHz)
#else //CFG_B2B_SIMU
#define APB_CLK		(40 * MHz)
#endif //CFG_B2B_SIMU

#define UARTC_CLOCK  (40 * MHz)		/* in WIFI, 40M Hz*/



//**********************add by wp********************************//

//rf type 
//ATBMWIFI_1A);
//ATBMWIFI_1B);
//ATBMWIFI_2A);
//
//rf type 
//ATBMWIFI_1A);
//ATBMWIFI_1B);
//ATBMWIFI_2A);
//
#define APOLLO_SIMU		0
#define APOLLO_1505		1
#define APOLLO_1601		2
#define APOLLO_1606		3
#define APOLLO_C        4
#define APOLLO_D        5
#define APOLLO_E        6
#define MAXIM_RF		7
#define ATHENA_B        8
#define ATHENA_BX        9
#define ATHENA_BY        10
#define LS				11 //28pin lite chip
#define LS_TC           12 //40pin lite chip
#define Ares_A          13
#define Ares_AX         14
#define Ares_AY         15

//this may define in makefile
#ifndef RF_SUBTYPE_DEFINE
#define RF_SUBTYPE_DEFINE 			    Ares_AX 
#endif

#if (RF_SUBTYPE_DEFINE == APOLLO_1601)
#pragma message("======RF_APOLLO_1601=====")
#elif (RF_SUBTYPE_DEFINE == APOLLO_1606)
#pragma message("======RF_APOLLO_1606=====")
#elif (RF_SUBTYPE_DEFINE == APOLLO_C)
#pragma message("======RF_APOLLO_C=====")
#elif (RF_SUBTYPE_DEFINE == APOLLO_D)
#pragma message("======RF_APOLLO_D=====")
#elif (RF_SUBTYPE_DEFINE == APOLLO_E)
#pragma message("======RF_APOLLO_E=====")
#elif (RF_SUBTYPE_DEFINE == ATHENA_B)
//#pragma message("======RF_ATHENA_B=====")
#elif (RF_SUBTYPE_DEFINE == ATHENA_BX)
#pragma message("======RF_ATHENA_BX=====")
#elif (RF_SUBTYPE_DEFINE == ATHENA_BY)
#pragma message("======RF_ATHENA_BY=====")
#elif (RF_SUBTYPE_DEFINE == Ares_A)
#pragma message("======RF_Ares_A=====")
#elif (RF_SUBTYPE_DEFINE == Ares_AX)
//#pragma message("======RF_Ares_AX=====")
#elif (RF_SUBTYPE_DEFINE == Ares_AY)
#pragma message("======RF_Ares_AY=====")
#elif (RF_SUBTYPE_DEFINE == APOLLO_SIMU)
#pragma message("======RF_APOLLO_SIMU=====")
#elif (RF_SUBTYPE_DEFINE == MAXIM_RF)
#pragma message("======RF_MAXIM_RF=====")
#else
#pragma message("======RF_Other=====")
#endif


//chip type define in makefile
//#define CHIP_ATHENAB     1
//#define CHIP_ARES     	 2
//#define CHIP_ARES_B     	 3

//this may define in makefile
#ifndef CHIP_TYPE
#define CHIP_TYPE 			    CHIP_ATHENAB  
#endif


#if (CHIP_TYPE == CHIP_ATHENAB)
#pragma message("======CHIP_TYPE == CHIP_ATHENAB=====")
#elif (CHIP_TYPE == CHIP_ARES)
#pragma message("======CHIP_TYPE == CHIP_ARES=====")
#elif (CHIP_TYPE == CHIP_ARES_B)
//#pragma message("======CHIP_TYPE == CHIP_ARES_B=====")
#else
#pragma message("======CHIP_TYPE err=====")
#endif

//
///
///memory map
//
/* Base address of the BRC */
#define APB_BASE_ADDRESS                (0x0A000000)

#if (USE_HI_USB)
#define HIF_BASE_ADDRESS                (0x0b000000)
#else
#define HIF_BASE_ADDRESS                (APB_BASE_ADDRESS + 0xB00000)
#endif

#define PHY_REG_BASE_ADDR               (APB_BASE_ADDRESS + 0xc00000)
#define WGT_BASE_ADDRESS                (APB_BASE_ADDRESS + 0xD00000)


#if (CHIP_TYPE==CHIP_ATHENAB)
#define DEFAULT_IVB_ADDRESS                	(0x00000000)
#define ITCM_ADDRESS                		(0x00000000)
#define CACHE_ADDRESS                		(0x00000000)

#else
#define ITCM_ADDRESS                		(0x00010000)
#if (CACHE_EILM== 1)
#define CACHE	1
#define CACHE_ADDRESS                		(0x00090000)
#define DEFAULT_IVB_ADDRESS                	CACHE_ADDRESS
#elif (CACHE_AHB == 1)
#define CACHE	1
//#define CACHE_64K	1
#define CACHE_ADDRESS                		(0xc10000)
#define DEFAULT_IVB_ADDRESS                	(CACHE_ADDRESS)
#else //NO CACHE
#define DEFAULT_IVB_ADDRESS                	ITCM_ADDRESS
#define CACHE_ADDRESS                		ITCM_ADDRESS //no cache so set addr to ITCM
#endif
#endif //#if (CHIP_TYPE==CHIP_ATHENAB)
#define ITCM_LEN                		(128*1024)
#define DTCM_ADDRESS                	(0x00800000)
#define DTCM_LEN                		(40*1024)

#define APB_BRG_ADDRESS                	(0x16000000)
#define APB_SMU_REGS        		   	(APB_BRG_ADDRESS+0x100000)
#define APB_UART_REGS                  	(APB_BRG_ADDRESS+0x200000)
#define APB_UART_REGS_2          		(APB_BRG_ADDRESS+0x300000)
#define PIT_BASE_ADDRESS             	(APB_BRG_ADDRESS+0x400000) //(APB_BASE_ADDRESS + 0xE00000)
#define PIT2_BASE_ADDRESS             	(APB_BRG_ADDRESS+0x500000) //
#define PIT3_BASE_ADDRESS           	(0x17000000)
#define ADC_BASE_ADDRESS                (APB_BRG_ADDRESS+0x101000)
#define CPE_WDT_BASE					(APB_BRG_ADDRESS+0x600000)
#define APB_PWR_RTC_BASE				(APB_BRG_ADDRESS+0x700000)
#define APB_PWR_RTC2_BASE				 0x17100000
#define GPIO_BASE_ADDRESS  				(APB_BRG_ADDRESS+0x800000)
#define I2C_BASE_ADDRESS        		(APB_BRG_ADDRESS+0x900000) //(APB_BASE_ADDRESS + 0x900000)
#define SPI_BASE_ADDRESS           		(APB_BRG_ADDRESS+0xa00000)
#define SPI_BASE_ADDRESS_2           	(APB_BRG_ADDRESS+0xc00000)
#define SPI_BASE_ADDRESS_3           	(APB_BRG_ADDRESS+0xd00000)

#define DMA_BASE_ADDRESS                (0x14000000)
#define EFUSE_BASE_ADDR                 (APB_BRG_ADDRESS+0xb00000)

/* Base address of the PAS */
#define PAC_BASE_ADDRESS           		(0x09000000)
#define PAC_BASE_VADDRESS           	(0x09400000)
#define SRAM_ADDRESS                	PAC_BASE_ADDRESS
#define SRAM_LEN                		(128*1024)




#define APB_PWR_CTRL_REGS               (APB_BASE_ADDRESS + 0xF00000) /*FIXME just test*/
#define APB_MIR_REGS                    (APB_BASE_ADDRESS + 0xF00000) /*FIXME just test*/
//#define APB_INT_REGS                    (APB_BASE_ADDRESS + 0x880000)
//#define APB_GPIO_REGS                   (APB_BASE_ADDRESS + 0xF00000)


/* Wired equivalent privacy registers */
#define PAC_REGS                        (PAC_BASE_ADDRESS + 0xC00000)
#define WEP_REGS                        (PAC_BASE_ADDRESS + 0xC10000)
#define RAB_REGS                        (PAC_BASE_ADDRESS + 0xC20000)
#define MIC_REGS                        (PAC_BASE_ADDRESS + 0xC40000)
#define AES_REGS                        (PAC_BASE_ADDRESS + 0xC50000)
#define PAC_SEQ_REGS                    (PAC_BASE_ADDRESS + 0xC60000)
#define PAS_SHARED_MEMORY               (PAC_BASE_ADDRESS  + 0)

/*
	This is automatically generated register base addresses
*/
	
//
//
//PHY register memory map
//
// Nmodem Receive RDT (NR)
#define PHY_RDT_BASE_ADDR 			PHY_REG_BASE_ADDR
#define PHY_RDF_BASE_ADDR 			PHY_REG_BASE_ADDR
#define PHY_RDC_BASE_ADDR 			PHY_REG_BASE_ADDR
#define PHY_TMP_BASE_ADDR 			PHY_REG_BASE_ADDR //0x0ab98000
#define PHY_TSG_BASE_ADDR 			PHY_REG_BASE_ADDR //0x0aba0000 
#define PHY_BRX_BASE_ADDR   		PHY_REG_BASE_ADDR //0x0aba8000
#define PHY_BTX_BASE_ADDR   		PHY_REG_BASE_ADDR //0x0abb0000
#define PHY_BBDIG_BASE_ADDR 		PHY_REG_BASE_ADDR //0x0abb8000
#define PHY_PRM_BASE_ADDR   		PHY_REG_BASE_ADDR //0x0abc8000 
#define PHY_PHY_BASE_ADDR 			PHY_REG_BASE_ADDR //0x0abd0000
#define PHY_RRM_BASE_ADDR			PHY_REG_BASE_ADDR //0x0abc8000
#define PHY_RFIP_BASE_ADDR          PHY_REG_BASE_ADDR //0x0ABC0000 


#define RDT_BASE_ADDR 				(PHY_REG_BASE_ADDR+0x0000000)
#define RDF_BASE_ADDR 				(PHY_REG_BASE_ADDR+0x0008000) //PHY_RDF_BASE_ADDR
#define RDC_BASE_ADDR 				(PHY_REG_BASE_ADDR+0x0010000) //PHY_RDC_BASE_ADDR
#define TMP_BASE_ADDR 				(PHY_REG_BASE_ADDR+0x0018000)
#define TSG_BASE_ADDR 				(PHY_REG_BASE_ADDR+0x0020000)
#define BRX_BASE_ADDR 				(PHY_REG_BASE_ADDR+0x0028000)
#define BTX_BASE_ADDR 				(PHY_REG_BASE_ADDR+0x0030000)
// BB Digital (RI)
#define BBDIG_BASE_ADDR             (PHY_REG_BASE_ADDR+0x0038000) //ac380000
// RFIP
#define RFIP_BASE_ADDR              (PHY_REG_BASE_ADDR+0x00c0000) //ac400000
// RRM (RM)
#define RRM_BASE_ADDR               (PHY_REG_BASE_ADDR+0x0048000)//ac480000
// PHY General Purpose
#define PHY_BASE_ADDR               (PHY_REG_BASE_ADDR+0x0050000)//ac500000

//*************add by wp***********************************************//


//************************************************************//
//Platform selection
/*add by wp*/
#define PLATFORM_1250_CUT_11            1
#define PLATFORM_1260_CUT_11            1

//ASIC selection
#define ASIC_1200_CUT_2                 1 // for EPTA/PHY, keep ASIC_1200_CUT_2=1 at the moment 
#define ASIC_1250_CUT_1                 1
#define ASIC_1260_CUT_1                 1
#define ENABLE_AGG_END_ECO              1
#define ENABLE_A4_FORMAT_ECO            1
#define ENABLE_BAB_ECO                  1

#if (RF_SUBTYPE_DEFINE == ATHENA_BY) //ATHENA_BY have no efuse
#define ENABLE_EFUSE					0		
#else
#define ENABLE_EFUSE					1
#endif


//FPGA use maxim
#define SUPPORT_MAXIM_CUT_1_RADIO       0


//****************************************************************************************
// Specific Platform configurations for feature support
//****************************************************************************************
#define USE_PRIVACY_HARDWARE            1

#if defined(CONFIG_OS_FREERTOS)
#define CFG_DBG_UART_IMMEDIATE_PRINT    1
#define CFG_DBG_UART_BUFFERED_PRINT     0
#define USE_CMD_LINE                    1
#else//CONFIG_OS_FREERTOS
#define CFG_DBG_UART_IMMEDIATE_PRINT    1
#define CFG_DBG_UART_BUFFERED_PRINT     0
#define USE_CMD_LINE     				1
#endif //CONFIG_OS_FREERTOS

#if (CHIP_TYPE == CHIP_ATHENAB)
#define USE_POWER_SAVING                1
#define USE_POWER_SAVING_RF             0
#else //(CHIP_TYPE == CHIP_ARES)
#define USE_POWER_SAVING                1
#define USE_POWER_SAVING_RF             0
#endif //#if (CHIP_TYPE == CHIP_ATHENAB)

#define ENABLE_WAPI                     0
#define ENABLE_5GHZ                     0
#define USE_FIXED_TRACE_MEMORY          0
#define TRACE_RECORD_BUF_SIZE           0x200  /* must be power of 2 */


//****************************************************************************************
// Specific PHY configurations for feature support
//****************************************************************************************
#define CLOSED_LOOP_POWER_CTRL          1
#define PHY_ENABLE_POWER_CONTROL        0
#define PHY_ENABLE_TEMP_CONTROL         0
#ifndef CFG_B2B_SIMU
#define PHY_LITE_NO_CAL                 0
#define	PHY_TX_CAL                      1
#else //CFG_B2B_SIMU
#define PHY_LITE_NO_CAL                 1
#define	PHY_TX_CAL                      0
#endif  //CFG_B2B_SIMU
#define	PHY_RX_CAL						1
#define RXDC_DIFSTRACKING_MODE 
#define USE_WCDMA_NOTCH_FILTER          0   /* SoC Related */

#define RFIP_ENABLE						(1)
//****************************************************************************************
// Specific EPTA configurations for feature support
//****************************************************************************************
#define USE_EPTA                        0  /*not Use PAC PTA Functions   int IOT        */
#define USE_CUT_EPTA					0
#define DISABLE_EPTA_PS_POLL_FALLBACK        1
//****************************************************************************************
// Specific PAS configurations for feature support
//****************************************************************************************
#define SUPPORT_MULTI_MAC_ADDR          1
#define NO_RXC_ON_OFF                   0
#define USE_BAP_STATUS                  1  /* Append BAP Status in Rx FIFO */
#define USE_CM00021770_DUAL_CTS         0 /* SWC - Dual CTS Protection */
#define SUPPORT_BAB                     1

//****************************************************************************************
// Specific LMC configurations for feature support
//****************************************************************************************
#define ENABLE_DATA_FRAME_FILTER        0 /* INROM, CM00013965: Support of Data Frame Filters */
#define USE_MULTI_TX_CONF               1
#define ENABLE_MINIAP                   1
#define ENABLE_11W                      0
#define ENABLE_11K                      0
#define ENABLE_P2P                      0
#define ENABLE_11K_QUIET_INTERVAL       0
#define USE_CM00030124_SCAN_THRESHOLD   1
#define USE_CM00030054_ARP_REPLY        0
#define USE_CM00030123_ARP_KEEPALIVE    0


//****************************************************************************************
// Specific WSC System level configuations
//****************************************************************************************
#define ENABLE_LMAC_DEBUG               1

#define USE_CR429501_ROAMING_OFFLOAD       0
#define USE_CR429501_ROAMING_OFFLOAD_5G    0
//******************************************************************
//******add by wp,need to check late************************************************************a
#define ENABLE_WEP_SHARE_KEY 		0
#define IBSS_SUPPORT 				0
#if (FPGA==1)
#define ENABLE_WDT 					0  //watch dog switch
#define WDT_USE_PIT_TIMER			0  //0:use watch timer feed wdt;   1:use pit timer feed wdt;
#else
#define ENABLE_WDT 					1  //watch dog switch
#define WDT_USE_PIT_TIMER			1  //0:use watch timer feed wdt;   1:use pit timer feed wdt;
#endif  //
#define ENABLE_BEACON_FILTER 		0 //add by wp
#define ENABLE_FILTER_RX_DUP        1 //filter rx dupplicate frame  //add by wp 
#define TDLS_SUPPORT				0
#define SRAM_POWER_SAVE_ZERO		0//power save ,sram data will not zero ,inorder to  reduction DCCM, TODO: if set SRAM_POWER_SAVE_ZERO=0, save xxx byte DCCM
#define HOST_RX_DEFRAG              0

#define LMAC_DBG_UPLOAD_HMAC		0
#define RXDC_DIFSTRACKING_MODE 
#define USE_HW_CHCKSUM				1



#undef ROM_PATCH
#define USE_NO_SSD_FILE

#define TEMP_P2P_FIND_FIX       0
/*just support 1 STA in AP*/

//****************************************************************************************
// Common configuations in WSC system level
//****************************************************************************************
#define SEND_DEBUG_INFO_IND_TO_HOST     0

/***********************
*   CONFIG VALIDATION
************************/
/*because we support 1 vmac */
#define MAX_TEAMP_FRAME_SETS   			1

// MINIAP -> forward
#define ENABLE_MCBC_FORWARDING          0

#define SYS_RX_BUFFER_SIZE_4K       	3 // 8 - 32K
#define WSM_MAX_NUM_LNK_AP             (4) //number of peer stations


#define COUNT_RX_DESCRIPTORS    		(8)
#define CFG_HI_MAX_RX_INDS_IN_PROGRESS (COUNT_RX_DESCRIPTORS)

#define COUNT_MIC_DESCRIPTORS			4


#define SYS_HOST_TX_LIMIT          		4 /*== HIF_INP_NUM_DESCS */

#define CFG_HI_MAX_IN_ITEMS        		(8)  /* must be power of 2  ,must > SYS_HOST_TX_LIMIT*/

/* Maximum number of Transmit buffers (requests) supported by the Host interface */
#define CFG_HI_NUM_REQS_BIG				(SYS_HOST_TX_LIMIT)


#define SINGLE_CHANNEL_MULTI_RX 0
/*
*add by zhouzhanchao
*/
//#define USE_WTD_CNT_TO_CANCLE_WTD_TIMER
#define LMAC_RATE_CONTROL  0
/* Host interface Queue depth for handling HI requests */
/* must be >= SYS_HOST_TX_LIMIT + CFG_HI_NUM_INDS + CFG_HI_MAX_RX_INDS_IN_PROGRESS */
#define CFG_HI_MAX_OUT_ITEMS        	(32)  /* must be power of 2 */


#if (USE_HI_AHB_CTRL)
#define CFG_HI_AHB_BUFFS_SIZE			256
#define CFG_HI_AHB_NUM_BUFFS			4
#endif //USE_HI_AHB_CTRL
 
#define OLD_RATE_POLICY 		1

#define USB_MIB_CMD_USE_EP0 	0
#define USE_PHY_DEBUG 			0
#define NO_PHYREG 				0
#define NO_RFREG 				0
#define FPGA_D2D 				0
#if (FPGA==1)
#undef  NO_RFREG
#define NO_RFREG 				1
#define	COMBUS					1
#endif

#define TEST_AGC_TYPE 
#define ATHENAB_ECO 			0
#define ENABLE_DEBUG_TARG		0

#define MONITOR_MODE 			1
#if MONITOR_MODE
#define USE_PHY_DEBUG 			0
#endif //MONITOR_MODE

#define ENABLE_CALI_CFO 		0

#define CUSTOM_VERSION			1


//#if     CUSTOM_VERSION
//#define FORCE_PM_2_ACTIVE 		1
//#else
#define FORCE_PM_2_ACTIVE 		0
//#endif //CUSTOM_VERSION
#define MODEM_TX_RX_ETF  		1
#define IPC_USED_FW   			0

#define IOT_PLATFORM 1
#define SMARTCONFIG_SUPPORT 1
#define JOYLINK_SUPPORT			1

#define RX_QOSPKG_RESTART_BEACONLOST_TIMER		(0)
#if RX_QOSPKG_RESTART_BEACONLOST_TIMER
#define BEACON_LOST_MAX  (5000000)	//5s
#endif


#define CONFIG_ATBM_PRODUCT_TEST		(1) //need  be 1 always
#define CONFIG_ATBM_PRODUCT_TEST_GOLDEN (0) // golden board  only

/* keep it off under normal conditions */
#define CONFIG_ATBM_PRODUCT_TEST_NO_UART	(0)//not use uart



#define WTD_DROP_ALL_TX_PKG				0


#include <config/app_config.h>

//#define MINI_MEMORY_MODE  			1

#if MINI_MEMORY_MODE
#undef SYS_RX_BUFFER_SIZE_4K
#undef COUNT_RX_DESCRIPTORS
#undef CFG_HI_MAX_RX_INDS_IN_PROGRESS
#undef COUNT_MIC_DESCRIPTORS
#undef SYS_HOST_TX_LIMIT
#undef CFG_HI_MAX_IN_ITEMS
#undef CFG_HI_NUM_REQS_BIG
#undef CFG_HI_MAX_OUT_ITEMS
#undef SUPPORT_BAB

#define SUPPORT_BAB 					0
#ifdef CONFIG_EWLINK_SMARTCONFIG
#define SYS_RX_BUFFER_SIZE_4K       	3 // 8 - 32K
#define COUNT_RX_DESCRIPTORS    		(12)
#define CFG_HI_MAX_RX_INDS_IN_PROGRESS (COUNT_RX_DESCRIPTORS)
#define COUNT_MIC_DESCRIPTORS			12
/* Host interface Queue depth for handling HI requests */
/* must be >= SYS_HOST_TX_LIMIT + CFG_HI_NUM_INDS + CFG_HI_MAX_RX_INDS_IN_PROGRESS */
#define CFG_HI_MAX_OUT_ITEMS        	(32)  /* must be power of 2 */
#else
#define SYS_RX_BUFFER_SIZE_4K       	2 // 8 - 32K
#define COUNT_RX_DESCRIPTORS    		(8)
#define CFG_HI_MAX_RX_INDS_IN_PROGRESS (COUNT_RX_DESCRIPTORS)
#define COUNT_MIC_DESCRIPTORS			8
/* Host interface Queue depth for handling HI requests */
/* must be >= SYS_HOST_TX_LIMIT + CFG_HI_NUM_INDS + CFG_HI_MAX_RX_INDS_IN_PROGRESS */
#define CFG_HI_MAX_OUT_ITEMS        	(32)  /* must be power of 2 */
#endif

#define SYS_HOST_TX_LIMIT          		2 /*== HIF_INP_NUM_DESCS */
#define CFG_HI_MAX_IN_ITEMS        		(4)  /* must be power of 2  ,must > SYS_HOST_TX_LIMIT*/
/* Maximum number of Transmit buffers (requests) supported by the Host interface */
#define CFG_HI_NUM_REQS_BIG				(SYS_HOST_TX_LIMIT)


#endif //MINI_MEMORY_MODE
#define CFG_HI_MAX_RX_BUFF_LEN			(1024*4-100)

#if(CHIP_TYPE == CHIP_ARES_B)
#undef CFG_DPLL_CLOCK
#define CFG_DPLL_CLOCK 24
#else
#define CFG_DPLL_CLOCK 40
#endif

#define RTOS_FIRMWARE			0
#define USE_CCA_DEBUG 			0
#define FULLMASK_SDIO 			0
#define SCAN_WDT 				1

#define AMPDU_DBG 					0
/*
rtl bug will cause cts-self never send done,so we not send cts-self 
*/
#define FIX_TX_CTS_PENDING_BUG 		1
#define SMARTCONFIG_FIFO 1


#define NOT_NEED_TXCONFIRM      1

#define SLEEP_USE_TEST_MODE  1 

#define REDUCE_POWER_CONSUMPTION 1

#endif //CFG_CW1250_C12_FPGA_H
