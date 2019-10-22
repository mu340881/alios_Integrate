#ifndef _API_TYPE_H_
#define _API_TYPE_H_

#include <stddef.h>
#include <inttypes.h>
#include <stdbool.h>
#include "type.h"
#if 0
/*
;*****************************************************************************************************
;* 描	 述 : 定义系统的数据类型。
;*****************************************************************************************************
;*/
typedef unsigned char boolean;
typedef unsigned char uint8; /* Unsigned  8 bit quantity							*/
typedef signed char int8; /* Signed    8 bit quantity							 */
typedef unsigned short uint16; /* Unsigned 16 bit quantity							  */
typedef signed short int16; /* Signed	16 bit quantity 						   */
typedef unsigned int uint32; /* Unsigned 32 bit quantity							*/
typedef signed int int32; /* Signed   32 bit quantity							 */
typedef float fp32; /* Single precision floating point					   */
typedef signed int sint32;
typedef signed short sint16;
typedef signed char sint8;



typedef unsigned char uint8_t; /* Unsigned  8 bit quantity							*/
typedef signed char int8_t; /* Signed    8 bit quantity							 */
typedef unsigned short uint16_t; /* Unsigned 16 bit quantity							  */
typedef signed short int16_t; /* Signed	16 bit quantity 						   */
//typedef unsigned int uint32_t; /* Unsigned 32 bit quantity							*/
typedef int int32_t; /* Signed   32 bit quantity							 */


typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef unsigned int BOOL;



typedef unsigned short u_short;


typedef unsigned char u_char;



typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef uint32_t u32_t;
typedef uint16_t u16_t;
typedef uint8_t u8_t;

typedef int32_t s32_t;
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

#define size_t unsigned int

typedef unsigned char UCHAR;
typedef signed char CHAR;
typedef unsigned char* PUCHAR;
typedef signed char* PCHAR;

typedef char           		INT8;
typedef unsigned char  		UINT8;
typedef signed short   		INT16;
typedef unsigned short 		UINT16;
typedef signed int    		INT32;
typedef unsigned int  		UINT32;
typedef unsigned long long	UINT64;
typedef long long			INT64;

#ifndef __off_t_defined
typedef long _off_t;
#endif
typedef int _ssize_t;

#ifndef __PTRDIFF_TYPE__
#define __PTRDIFF_TYPE__ long int
#endif
typedef __PTRDIFF_TYPE__ ptrdiff_t;

#define	_CLOCK_T_	unsigned long		/* clock() */
#define	_TIME_T_	long			/* time() */
#define _CLOCKID_T_ 	unsigned long
#define _TIMER_T_   	unsigned long

#define __bitwise
#define __force

typedef u16 __bitwise be16;
typedef u16 __bitwise le16;
typedef u32 __bitwise be32;
typedef u32 __bitwise le32;
typedef u64 __bitwise be64;
typedef u64 __bitwise le64;



#ifndef FALSE
#define FALSE	0
#define TRUE	1
#endif /*FALSE*/

#define BIT(x)	(1 << (x))
#define NETDEV_ALIGN		32
#define ETHTOOL_BUSINFO_LEN 32

typedef unsigned gfp_t;
typedef unsigned fmode_t;

#define __packed	__attribute__((packed))

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

#define cpu_to_be16(a) htons(a)
#define cpu_to_be32(a) htonl(a)
#define be16_to_cpu(a) ntohs(a)
#define be32_to_cpu(a) ntohl(a)
#endif //0
#endif /*_API_TYPE_H_*/
