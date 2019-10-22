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
* \brief Standard Definitions
*/
/*
***************************************************************************
* Copyright altobeam, 2009 �All rights reserved.
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

#ifndef _STDDEFS_H_
#define _STDDEFS_H_
#include "type.h"
#if 0
#define _API_TYPE_H_
/*
;*****************************************************************************************************
;* 描	 述 : 定义系统的数据类型。
;*****************************************************************************************************
;*/
typedef unsigned char boolean;
typedef unsigned char uint8; /* Unsigned  8 bit quantity							*/
typedef signed char int8; /* Signed    8 bit quantity							 */
typedef signed char sint8; /* Signed    8 bit quantity							 */
typedef unsigned short uint16; /* Unsigned 16 bit quantity							  */
typedef signed short int16; /* Signed	16 bit quantity 						   */
typedef signed short sint16; /* Signed	16 bit quantity 						   */
typedef unsigned int uint32; /* Unsigned 32 bit quantity							*/
typedef signed int int32; /* Signed   32 bit quantity							 */
typedef float fp32; /* Single precision floating point					   */
typedef signed int sint32;

typedef unsigned char uint8_t; /* Unsigned  8 bit quantity							*/
typedef signed char int8_t; /* Signed    8 bit quantity							 */
typedef unsigned short uint16_t; /* Unsigned 16 bit quantity							  */
typedef signed short int16_t; /* Signed	16 bit quantity 	*/

//typedef unsigned int uint32_t;	 /* Unsigned 32 bit quantity							*/
//typedef signed int int32_t; /* Signed   32 bit quantity							 */





typedef unsigned short u_short;


typedef unsigned char u_char;



typedef unsigned int u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef u32 u32_t;
typedef uint16_t u16_t;
typedef uint8_t u8_t;

//typedef int32_t s32_t;
typedef int16_t s16_t;
typedef int8_t s8_t;

typedef unsigned short __le16;
typedef unsigned int __le32;
typedef unsigned long long __le64;

typedef unsigned short __be16;
typedef unsigned int __be32;
//typedef unsigned __int64	__be64;
typedef unsigned short __u16;
typedef unsigned char __u8;
typedef unsigned long long u64; //mark 无符号64位
typedef unsigned int __u32;


typedef signed char s8;
typedef signed short s16;
typedef signed int s32;

//#define size_t unsigned int

typedef unsigned char UCHAR;
typedef signed char CHAR;
typedef unsigned char* PUCHAR;
typedef signed char* PCHAR;


#define bool	unsigned char
#ifndef FALSE
#define FALSE	0
#define TRUE	1
#endif /*FALSE*/

#define BIT(x)	(1 << (x))
#define NETDEV_ALIGN		32
#define ETHTOOL_BUSINFO_LEN 32

typedef unsigned gfp_t;
typedef unsigned fmode_t;
#ifndef __packed
#define __packed	__attribute__((packed))
#endif //__packed

#define false 0
#define true  1

#ifndef min
#define min(_a, _b) 	(((_a) < (_b)) ? (_a) : (_b))
#endif

#ifndef max
#define max(_a, _b) 	(((_a) > (_b)) ? (_a) : (_b))
#endif


#ifndef NULL
#define NULL	((void *)0)
#endif

#define cpu_to_le16(v16) (v16)
#define cpu_to_le32(v32) (v32)
#define cpu_to_le64(v64) (v64)
#define le16_to_cpu(v16) (v16)
#define le32_to_cpu(v32) (v32)
#define le64_to_cpu(v64) (v64)

#define __cpu_to_le16(v16) (v16)
#define __cpu_to_le32(v32) (v32)
#define __cpu_to_le64(v64) (v64)
#define __le16_to_cpu(v16) (v16)
#define __le32_to_cpu(v32) (v32)
#define __le64_to_cpu(v64) (v64)

typedef u16  be16;
typedef u16  le16;
typedef u32  be32;
typedef u32  le32;
typedef u64  be64;
typedef u64  le64;


typedef u64 UINT64;
typedef int32 INT32;
typedef uint32 UINT32;
typedef uint16 UINT16;
typedef uint8 UINT8;


#endif /*_API_TYPE_H_*/



#define PUBLIC
#define PRIVATE	    static

//typedef unsigned char uint8;
//typedef signed char sint8;
//typedef unsigned short uint16;
//typedef signed short sint16;
//typedef unsigned long uint32;
//typedef signed long sint32;


//#ifndef __cplusplus
//typedef int bool;
//#endif

#ifndef offsetof
#define offsetof(_s, _f)  ((uint8 *)&((_s*)0)->_f - (uint8 *)0)
#endif

#if defined(_WIN32)
typedef unsigned __int64 uint64;
typedef signed __int64 sint64;
typedef uint32 UINTPTR;
#define  PACKED
#define GCC_PACKED
#define ALIGN4
#else
#define ALIGN4 __attribute__ ((aligned (4)))
#endif

#if defined(__GNUC__)
//typedef __u64 uint64;
//typedef __s64 sint64;
typedef unsigned long long uint64;
typedef signed long long sint64;
typedef uint32 UINTPTR;
#define PACKED
#define GCC_PACKED __attribute__((packed))
#else
#if defined(__arm)
typedef unsigned long long uint64;
typedef signed long long sint64;
typedef uint32 UINTPTR;
#define  PACKED  __packed
#define GCC_PACKED
#ifndef FIELD_OFFSET
	/*#define FIELD_OFFSET(type, field) ((UINTPTR)(&((type *)0)->field))*/
#define FIELD_OFFSET(type, field) offsetof(_s, _f)
#endif
#endif // __arm

#if defined(ANDES)
typedef unsigned long long uint64;
typedef signed long long sint64;
typedef uint32 UINTPTR;
#define PACKED 
#define GCC_PACKED __attribute__((packed))
#ifndef FIELD_OFFSET
	/*#define FIELD_OFFSET(type, field) ((UINTPTR)(&((type *)0)->field))*/
#define FIELD_OFFSET(type, field) offsetof(_s, _f)
#endif
#endif // ANDES

#endif // __GNUC__

#if !defined(__GNUC__)
#undef NULL
#if !defined(NULL)
#define NULL	    0
#endif
#endif

#define TRUE	    1
#define FALSE	   0

#define forever()   for (;;)

#ifndef ENTRIES
#define ENTRIES(x)  (sizeof(x)/sizeof((x)[0]))
#endif

#define ALIGN_DOWN_16BIT(_x)      (((UINTPTR)(_x))&~(sizeof(uint16)-1))
#define ALIGN_DOWN_32BIT(_x)      (((UINTPTR)(_x))&~(sizeof(uint32)-1))

#define ALIGN_UP_16BIT(_x)      ALIGN_DOWN_16BIT(_x+sizeof(uint16)-1)
#define ALIGN_UP_32BIT(_x)      ALIGN_DOWN_32BIT(_x+sizeof(uint32)-1)


#define ALIGN(a,b)			(((a) + ((b) - 1)) & (~((b)-1)))

#ifndef INITIALISE_TO_FIELD
/*
 * Do not be tempted by the Windows-style "CONTAINING_RECORD()" macro.  It is
 * quite clever, but doesn't check that the address you give it is a pointer
 * to the correct type.  This can hide some nasty programming errors.
 *
 * Also, there is a "feature" in the MS compiler that doesn't properly check
 * the types of pointer that are cast from constants.
 *   e.g.
 *      STRUCT_A *myAPtr = NULL;
 *      STRUCT_B *myBPtr = NULL;
 *      Warning as expected
 *      int Test1 = (myAPtr == myBPtr);
 *      No warning!
 *      int Test2 = (&((STRUCT_A *)0) == myBPtr);
 *
 * Hence, we have to have a slightly different macro to get around the above
 *problem, perform the casts AND check the type.
 */
#define INITIALISE_TO_FIELD(var, address, type, field) \
    { var = ((type *)((char *)(address) - (UINTPTR)(&((type *)0)->field))), (&var->field == address); }
#endif

/************************
* BITFIELD_VALUE(_bitmap)
*************************
* Converts a uint32 bitfield structure to/from a uint32 value
*/
#define BITFIELD_VALUE(_bitfield)  *((uint32 *) (&(_bitfield)))

#if 0
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) (((a) < (b)) ? (b) : (a))
#endif
#endif
#define MIN(a,b) ( ((a)>(b)) ? (b):(a) )
#define MAX(a,b) ( ((a)>(b)) ? (a):(b) )
#define ABS(a) (((a) < 0) ? (-(a)) : (a))

typedef struct REGVAL_S {
	uint32 addr;
	uint32 val;
} REGVAL;
// Register bit operation macro
typedef struct REGVAL_Bit
{
	 uint32 addr;
	 uint16 endBit;
	 uint16 startBit;
	 uint32 data;
}REGVAL_Bit;


#define BIT_MASK(bit_h, bit_l)	((UINT32)((((UINT64)0x1<<(1+bit_h-bit_l))-(UINT32)0x1)<<bit_l))
#define SET_BIT(var, bit)			do { var |= (0x1 << (bit)); } while(0) 
#define CLR_BIT(var, bit)			do { var &= (~(0x1 << (bit))); } while(0)

#define SET_FIELD(var, mask, offset, value)	do {\
        						var = ((var) & (~mask)) | (((value) << (offset)) & (mask)); \
        					} while (0)
#define GET_FIELD(var, mask, offset)		(((var) & (mask)) >> (offset))

#define TEST_FIELD(var, mask)			((var) & (mask))

#define CHECK_FIELD(value, mask)		((value) & (mask))
#define EXTRACT_FIELD(value, mask, offset)	(((value) & (mask)) >> (offset))
#define PREPARE_FIELD(value, mask, offset)	(((value) << (offset)) & (mask))

// Variable bit operation macro
#define VAR_TEST_BIT(var, sig)			((var) & (sig))
#define VAR_SET_BIT(var, sig)			((var) = (var) | (sig))
#define VAR_CLR_BIT(var, sig)			((var) = (var) & (~(sig)))

#endif	/* Sentry */			/* #ifndef _STDDEFS_H_*/
