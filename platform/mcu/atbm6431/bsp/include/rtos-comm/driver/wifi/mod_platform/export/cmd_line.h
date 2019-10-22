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
* \brief Command line support
*/
/*
***************************************************************************
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

#ifndef CMD_LINE_H
#define CMD_LINE_H


#if (USE_CMD_LINE)



/**************************************************************************
**
** NAME        CmdLine_GetPlatformVar
** 
** PARAMETERS:  *buf - the memory to save platform.
                           len  - the size of memory
**
** RETURNS:        none
**            
** DESCRIPTION    Read platform information, save in arg memory.
**
**************************************************************************/

NORELOC void CmdLine_GetPlatformVar(char *buf, int len);


extern 	void CmdLine_DelayPowerSave();




NORELOC  void CmdLine_PollOnException(void);

#endif //USE_CMD_LINE

#if (USE_PHY_DEBUG)
NORELOC void CmdLine_PhyDbgCntUpdata(RX_INDICATE *pRxIndicate);
#endif //#if (USE_PHY_DEBUG)

#endif /* Sentry */

