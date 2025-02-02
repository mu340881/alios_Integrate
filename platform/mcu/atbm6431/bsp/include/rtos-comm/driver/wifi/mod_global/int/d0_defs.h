/* -*-H-*-
*******************************************************************************
* altobeam
* Reproduction and Communication of this document is strictly prohibited
* unless specifically authorized in writing by altobeam
******************************************************************************/
/**
* \file
* \ingroup GLOBAL
*
* \brief IEEE 802.0 definitions
*/
/*
***************************************************************************
* Copyright (c) 2007 altobeam Ltd
* Copyright altobeam, 2009 � All rights reserved.
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

#ifndef _D0_DEFS_H
#define _D0_DEFS_H

/***********************************************************************/
/***			Include Files			    ***/
/***********************************************************************/

/***********************************************************************/
/***			Data Types and Constants		 ***/
/***********************************************************************/
#define D0_ADDR_SIZE    6
#define D0_MCAST_BIT    0x1
#define D0_LOCAL_BIT    0x2

typedef PACKED struct D0_ADDR_S {
	PACKED union {
		uint8 Byte[D0_ADDR_SIZE];
		uint16 hWord[D0_ADDR_SIZE / 2];
	} GCC_PACKED u;
} GCC_PACKED D0_ADDR;

#define D0_ADDR_EQ(_a, _b) \
     ((*(uint16*)(_a)		     == *(uint16*)(_b)) && \
      (*(((uint16*)(_a))+1) == *(((uint16*)(_b))+1)) && \
      (*(((uint16*)(_a))+2) == *(((uint16*)(_b))+2)))

#define D0_ADDR_EQ5(_a, _b) \
		 ((*(uint16*)(_a)			 == *(uint16*)(_b)) && \
		  (*(((uint16*)(_a))+1) == *(((uint16*)(_b))+1)) && \
		  (*(uint8*)(((uint16*)(_a))+2) == *(uint8*)(((uint16*)(_b))+2)))

#define D0_ADDR_NEQ(_a, _b) \
     ((*(uint16*)(_a)       != *(uint16*)(_b)) || \
      (*(((uint16*)(_a))+1) != *(((uint16*)(_b))+1)) || \
      (*(((uint16*)(_a))+2) != *(((uint16*)(_b))+2)))

#define D0_IS_BROADCAST(_a) \
     ((*(uint16*)(_a)       == 0xFFFF) && \
      (*(((uint16*)(_a))+1) == 0xFFFF) && \
      (*(((uint16*)(_a))+2) == 0xFFFF))

#define D0_IS_MULTICAST(_a) \
      (*((uint8*)(_a)) & ((uint8)0x01))

#define D0_ADDR_COPY(_a, _b) \
     {*((uint16*)(_a)) = *((uint16*)(_b)); \
      *(((uint16*)(_a))+1) = *(((uint16*)(_b))+1); \
      *(((uint16*)(_a))+2) = *(((uint16*)(_b))+2); }

#define D0_SET_BROADCAST(_a) \
    {(*(uint16*)(_a) = 0xFFFF);\
     (*(((uint16*)(_a))+1) = 0xFFFF); \
     (*(((uint16*)(_a))+2) = 0xFFFF); }

#endif				/* _D0_DEFS_H */
