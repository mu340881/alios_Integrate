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
* \brief Basic Hardware definition
*/
/*
***************************************************************************
* Copyright (c) 2007 altobeam. All rights reserved
* Copyright altobeam, 2009 – All rights reserved.
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

#ifndef HW_DEFS_H
#define HW_DEFS_H

#include "atbm_hal.h"



/* Number of PAS system clocks per us */
#define SYS_PAS_CLOCKS_PER_US           80



#define HW_WRITE_REG(_Reg,_Value)   (*((volatile u32*)(_Reg)) = (u32)(_Value))
#define HW_READ_REG(_Reg)           (*((volatile u32*)(_Reg)))

extern u32 HW_READ_REG_BIT(u32 addr,int endbit, int startbit);
extern void HW_WRITE_REG_BIT(u32 addr,u8 endBit,u8 startBit,u32 data );
#endif /* Sentry */




