#ifndef _API_H_
#define _API_H_

#include "hal.h"
#include "atomic.h"
#include "type.h"
#include "mutex.h"
#include "apidebug.h"
#include "skbuf.h"
#include "linked-list.h"
#include "hmac_timer.h"
//#include "api_work.h"

/*
 * ..and if you can't take the strict
 * types, you can specify one yourself.
 *
 * Or not use min/max/clamp at all, of course.
 */
#define min_t(type, x, y) ({			\
	type __min1 = (x);			\
	type __min2 = (y);			\
	__min1 < __min2 ? __min1: __min2; })

#define max_t(type, x, y) ({			\
	type __max1 = (x);			\
	type __max2 = (y);			\
	__max1 > __max2 ? __max1: __max2; })

#define BITMAP_LAST_WORD_MASK(nbits)					\
(									\
	((nbits) % BITS_PER_LONG) ?					\
		(1UL<<((nbits) % BITS_PER_LONG))-1 : ~0UL		\
)

#define small_const_nbits(nbits) \
	((nbits) && (nbits) <= BITS_PER_LONG)

#define sleep hal_sleep
#define msleep hal_sleep
#define mdelay hal_sleep
#define OS_SLEEP hal_sleep
#define printk iot_printf
#define likely(a)  a
#define mb()
#define ASSERT_RTNL()

#define BITS_PER_LONG 32
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))
//#define BIT_MASK(nr)		(1UL << ((nr) % BITS_PER_LONG))
//#define BIT_WORD(nr)		((nr) / BITS_PER_LONG)
#define BITS_PER_BYTE		8
#define BITS_TO_LONGS(nr)	DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))
#define ALIGN(a,b)			(((a) + ((b) - 1)) & (~((b)-1)))
#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))




struct __una_u16 { u16 x; } __packed ;
struct __una_u32 { u32 x; } __packed ;

static inline u16 __get_unaligned_cpu16(const void *p)
{
 	const struct __una_u16 *ptr = (const struct __una_u16 *)p;
 	return ptr->x;
}
 
static inline  u32 __get_unaligned_cpu32(const void *p)
{
	 const struct __una_u32 *ptr = (const struct __una_u32 *)p;
	 return ptr->x;
}

static inline u16 get_unaligned_le16(const void *p)
{
 	return __get_unaligned_cpu16((const u8 *)p);
}
 
static inline u32 get_unaligned_le32(const void *p)
{
	return __get_unaligned_cpu32((const u8 *)p);
}

static inline int __bitmap_empty(const unsigned long *bitmap, int bits)
{
	int k, lim = bits/BITS_PER_LONG;
	for (k = 0; k < lim; ++k)
		if (bitmap[k])
			return 0;

	if (bits % BITS_PER_LONG)
		if (bitmap[k] & BITMAP_LAST_WORD_MASK(bits))
			return 0;

	return 1;
}

static inline int bitmap_empty(const unsigned long *src, int nbits)
{
	if (small_const_nbits(nbits))
		return ! (*src & BITMAP_LAST_WORD_MASK(nbits));
	else
		return __bitmap_empty(src, nbits);
}

u32 random32();
u32 crc32_be(u32 crc, unsigned char const *p, size_t len);
#define  jiffies  hal_time_get() /*ms*/


#endif
