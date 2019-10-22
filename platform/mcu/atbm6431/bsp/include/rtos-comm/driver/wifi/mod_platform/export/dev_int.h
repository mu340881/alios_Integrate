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
* \brief Defines device interrupt mapping
*/
/*
***************************************************************************
* Copyright (c) 2001-2004 Synad Technologies LTD
* Copyright (c) 2004    STMicroelectroincs
* Copyright altobeam, 2009 锟紸ll rights reserved.
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
* 16/04/2009 - WBF00003856 - http://cqweb.zav.st.com/cqweb/url/default.asp?db=WBF&id=WBF00003856
****************************************************************************/

#ifndef _DEV_INT_H
#define _DEV_INT_H

/***********************************
* Interrupt Mapping
************************************
*
* IRQ   
* Source    Interrupt signal            Type (Sensitivity)    Remark
* 0         Uart Tx FIFO empty          Level    
* 1         Uart Rx Error               Edge                  Only pulses high 1-cycle for error.
* 2         Uart Rx trigger             Level    
* 3         Uart Rx FIFO Overflow       Level    
* 4         Power Controller            Level                 SWCounter, wakeup (int & ext) and DPLL reconfiguration
* 5         Counter Timer (0)           Level     
* 6         Counter Timer (1)           Level     
* 7         Phy Radar Detected          Pulse    
* 8         Phy Report Completed        Pulse    
* 9         Reserved (tied to 0)                              May be used by Phy in the future
* 10        Reserved (tied to 0)                              May be used by Phy in the future
* 11        Reserved (tied to 0)                              May be used by Phy in the future
* 12        Reserved (tied to 0)                              May be used by Phy in the future
* 13        Host Interface              Level                 SPI/SDIO data transfer related (start, complete, error)
* 14        Reserved (tied to 0)        
* 15        PTA                         Level                 COEX Serial interface status
* 16        Reserved (tied to 0)                              May be used by PTA in the future
* 17        Reserved (tied to 0)                              May be used by PTA in the future
* 18        WEP Completed               Pulse    
* 19        WEP CRC Fail                Pulse    
* 20        AES Done                    Pulse    
* 21        MIC/MMH Completed           Pulse    
* 22        Reserved (tied to 0)                              May be used by WAPI in the future
* 23        Reserved (tied to 0)                              May be used by WAPI in the future
* 24        PAC Rx Error                Level                 rxc_rx_buffer_over_flow OR rxc_rx_fifo_overflow
* 25        PAC Tick Timer              Pulse    
* 26        PAC TSF Timer               Level    
* 27        PAC Tx Error                Pulse                 (del_state /= air_wait)  AND phy_ready_
* 28        PAC Interrupt (NTD fifo)    Level    
* 29        Reserved (tied to 0)        
* 30        Reserved (tied to 0)        
* 31        Reserved (tied to 0)        
*/
//#define DEV_INT_NUM_UART_TX_EMPTY       0
//#define DEV_INT_NUM_UART_RX_ERROR       1
//#define DEV_INT_NUM_UART_RX_TRIGGER     2
//#define DEV_INT_NUM_UART_RX_OVERFLOW    3
//25	Wifi_rtc_int_period	RTC	半秒、秒、分、小时、天
//26	Wifi_rtc_int_ alarm	RTC	警报
#define DEV_INT_NUM_PMU					30			// power up from light sleep
#define DEV_INT_NUM_POWER_CONTROL       28
#define DEV_INT_NUM_I2C_SW       		27
#define DEV_INT_NUM_RTC_ALARM           26
#define DEV_INT_NUM_RTC_PERIOD          25
//24	Wifi_bmc_int	BMC	AHB Matrix地址选择错误
#define DEV_INT_NUM_BMC          		24
#define DEV_INT_NUM_GPIO                23
#define DEV_INT_NUM_I2C                 22
#define DEV_INT_NUM_UART2            	20
#define DEV_INT_NUM_UART       		    19
#define DEV_INT_NUM_PIT              	18
#define DEV_INT_NUM_PIT_US             	17
#if (CHIP_TYPE==CHIP_ATHENAB)
//#define DEV_INT_NUM_WIFI_DMA            16
#define DEV_INT_NUM_WIFI_SW             15
#else
#define DEV_INT_NUM_RTC_ALARM2           16
#define DEV_INT_NUM_RTC_PERIOD2          15	
#define DEV_INT_NUM_WIFI_SW             14
#endif 
//#define DEV_INT_NUM_APP_CPU             14
//#define DEV_INT_NUM_CNT_T1              6
#define DEV_INT_NUM_SPI3  				13
#define DEV_INT_NUM_SPI2  				12
#define DEV_INT_NUM_SPI1  				11
#define DEV_INT_NUM_HIF                 10
#define DEV_INT_NUM_EPTA                9
//#define DEV_INT_NUM_WEP_COMPLETE        8
#define DEV_INT_NUM_CIPHER_COMPLETE     8
#define DEV_INT_NUM_DMA                7
#define DEV_INT_NUM_PHY					6
#define DEV_INT_NUM_USB					5
#define DEV_INT_NUM_PAC_RX_ERROR        4
#define DEV_INT_NUM_PAC_TICK_TIMER      3
#define DEV_INT_NUM_PAC_TSF_TIMER       2
#define DEV_INT_NUM_PAC_TX_ERROR        1
#define DEV_INT_NUM_PAC_NTD             0
//#define DEV_INT_NUM_WDT            		NMI
//#define APB_INT_UART_TX_EMPTY        (1<<DEV_INT_NUM_UART_TX_EMPTY)
//#define APB_INT_UART_RX_ERROR        (1<<DEV_INT_NUM_UART_RX_ERROR)
//#define APB_INT_UART_RX_TRIGGER      (1<<DEV_INT_NUM_UART_RX_TRIGGER)
//#define APB_INT_UART_RX_OVERFLOW     (1<<DEV_INT_NUM_UART_RX_OVERFLOW)
#define APB_INT_POWER_CONTROL        (1<<DEV_INT_NUM_POWER_CONTROL)
//#define APB_INT_CNT_T0               (1<<DEV_INT_NUM_CNT_T0)
//#define APB_INT_CNT_T1               (1<<DEV_INT_NUM_CNT_T1)
//#define APB_INT_PHY_RADAR_DETECTED   (1<<DEV_INT_NUM_PHY_RADAR_DETECTED)
//#define APB_INT_PHY_REPORT_COMPLETE  (1<<DEV_INT_NUM_PHY_REPORT_COMPLETE)
//#define APB_INT_PIT_US                  (1<<DEV_INT_NUM_PIT_US)
#define APB_INT_PIT                  (1<<DEV_INT_NUM_PIT)
#define APB_INT_HIF                  (1<<DEV_INT_NUM_HIF)
#define APB_INT_EPTA                 (1<<DEV_INT_NUM_EPTA)
//#define APB_INT_WEP_COMPLETE         (1<<DEV_INT_NUM_WEP_COMPLETE)
//#define APB_INT_WEP_CRC_FAIL         (1<<DEV_INT_NUM_WEP_CRC_FAIL)
//#define APB_INT_AES_COMPLETE         (1<<DEV_INT_NUM_AES_COMPLETE)
//#define APB_INT_MIC_COMPLETE         (1<<DEV_INT_NUM_MIC_COMPLETE)
#define APB_INT_PAC_RX_ERROR         (1<<DEV_INT_NUM_PAC_RX_ERROR)
//#define APB_INT_PAC_TICK_TIMER       (1<<DEV_INT_NUM_PAC_TICK_TIMER)
#define APB_INT_PAC_TSF_TIMER        (1<<DEV_INT_NUM_PAC_TSF_TIMER)
#define APB_INT_PAC_TX_ERROR         (1<<DEV_INT_NUM_PAC_TX_ERROR)
#define APB_INT_PAC_NTD              (1<<DEV_INT_NUM_PAC_NTD)
#define APB_INT_CIPHER_COMPLETE      (1<<DEV_INT_NUM_CIPHER_COMPLETE)
#define APB_INT_USB                  (1<<DEV_INT_NUM_USB)


#define APB_FIQ_CAUSES               APB_INT_PAC_NTD



#define APB_INT_ACTIVE_HIGH         0xFFFFFFFF

#define APB_INT_LEVEL              (BIT(DEV_INT_NUM_WIFI_SW)|APB_INT_HIF | APB_INT_PAC_NTD | APB_INT_PAC_TSF_TIMER | \
                                      APB_INT_PIT | APB_INT_CIPHER_COMPLETE | BIT(DEV_INT_NUM_RTC_ALARM)|BIT(DEV_INT_NUM_UART) |BIT(DEV_INT_NUM_UART2) | \
                                     BIT(DEV_INT_NUM_I2C)|BIT(DEV_INT_NUM_GPIO)| BIT(DEV_INT_NUM_I2C_SW))
                                    /* APB_INT_CNT_T1 is deliberately programmed as edge interrupt */ 
                                    /* APB_INT_PAC_RX_ERROR is deliberately programmed as edge interrupt */

#endif /* Sentry */

