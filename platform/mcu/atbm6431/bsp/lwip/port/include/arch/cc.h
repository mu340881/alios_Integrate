#ifndef __ARCH_CC_H__
#define __ARCH_CC_H__

#include "atbm_hal.h"
#include <stddef.h>
#include <inttypes.h>
#include "type.h"
#include "lwipopts.h"

/*
typedef uint8_t		u8_t;
typedef uint16_t	u16_t;
typedef uint32_t	u32_t;

typedef int8_t		s8_t;
typedef int16_t		s16_t;
typedef int32_t		s32_t;
*/
#define ERRNO
#define LWIP_MAILBOX_QUEUE

typedef intptr_t	mem_ptr_t;
typedef int sys_prot_t;

#define U16_F		"u"
#define U32_F		"u"

#define X16_F		"x"
#define X32_F		"x"

#define S16_F		"d"
#define S32_F		"d"

#ifndef NULL
#define NULL		((void*)0)
#endif

#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif /* BYTE_ORDER */

#define PACK_STRUCT_FIELD(x)	x
#define PACK_STRUCT_STRUCT	__attribute__ ((__packed__))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

/* Platform specific diagnostic output */

#define DEBUG2( ...)						\
	do{							\
		printf(__VA_ARGS__);			\
	} while( 0)

#ifdef LWIP_DEBUG
#define LWIP_PLATFORM_DIAG(x)		DEBUG2 x
#define LWIP_PLATFORM_ASSERT(x)		printf("\n")
#else
#define LWIP_PLATFORM_DIAG(x)		//DEBUG2 x
#define LWIP_PLATFORM_ASSERT(x)		//DEBUG(1,1, x "\n")
#endif

/* "lightweight" synchronization mechanisms */
//#define SYS_ARCH_DECL_PROTECT(x)	/* - declare a protection state variable. */
//#define SYS_ARCH_PROTECT(x)		/* - enter protection mode. */
//#define SYS_ARCH_UNPROTECT(x)		/* - leave protection mode. */


#define LWIP_RAND() ((u32_t)hal_random_get())

#endif /* __ARCH_CC_H__ */
