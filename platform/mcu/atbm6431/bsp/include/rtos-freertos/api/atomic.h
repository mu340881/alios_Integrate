#ifndef _ATOMIC_H_
#define _ATOMIC_H_

#define MAX_ERRNO 4096
#define in_interrupt() (OSIntNesting)
#define IS_ERR_VALUE(x) unlikely((x) >= (unsigned long)-MAX_ERRNO)
#define DIV_ROUND_CLOSEST(x, divisor)(			\
{							\
	typeof(divisor) __divisor = divisor;		\
	(((x) + ((__divisor) / 2)) / (__divisor));	\
}							\
)

typedef struct _atomic_t
{
	  int val;
} atomic_t;

/*
 * 64-bit atomic ops
 */
typedef struct {
	volatile long long counter;
} atomic64_t;

struct ewma {
	unsigned long internal;
	unsigned long factor;
	unsigned long weight;
};


#define enter_critical()	vPortEnterCritical()
#define  exit_critical()	vPortExitCritical()


int atomic_test_set(atomic_t *at, int val);
int atomic_add_return(int val,atomic_t *at);
int atomic_xchg(atomic_t * v, int val);

void atomic_set(atomic_t *at, int val);
int atomic_read(atomic_t *at);
int set_bit(int nr,unsigned long * addr);
int clear_bit(int nr, unsigned long * addr);
#define test_and_clear_bit clear_bit

int test_bit(int nr,unsigned long * addr);
u32 crc32_be(u32 crc, unsigned char const *p, size_t len);

struct kref {
	atomic_t refcount;
};

void kref_set(struct kref *kref, int num);
void kref_init(struct kref *kref);
void kref_get(struct kref *kref);
int kref_put(struct kref *kref, void (*release) (struct kref *kref));
void * ERR_PTR(long error);
long  PTR_ERR(const void *ptr);
long  IS_ERR(const void *ptr);
void ewma_init(struct ewma *avg, unsigned long factor, unsigned long weight);
unsigned long ewma_read(const struct ewma *avg);
struct ewma *ewma_add(struct ewma *avg, unsigned long val);

#if 0

/**
 * ffs - find first bit set
 * @x: the word to search
 *
 * This is defined the same way as
 * the libc and compiler builtin ffs routines, therefore
 * differs in spirit from the above ffz (man ffs).
 */
static inline int ffs(int x)
{
	int r = 1;

	if (!x)
		return 0;
	if (!(x & 0xffff)) {
		x >>= 16;
		r += 16;
	}
	if (!(x & 0xff)) {
		x >>= 8;
		r += 8;
	}
	if (!(x & 0xf)) {
		x >>= 4;
		r += 4;
	}
	if (!(x & 3)) {
		x >>= 2;
		r += 2;
	}
	if (!(x & 1)) {
		x >>= 1;
		r += 1;
	}
	return r;
}
static inline int fls(int x)
{
	int r = 32;

	if (!x)
		return 0;
	if (!(x & 0xffff0000u)) {
		x <<= 16;
		r -= 16;
	}
	if (!(x & 0xff000000u)) {
		x <<= 8;
		r -= 8;
	}
	if (!(x & 0xf0000000u)) {
		x <<= 4;
		r -= 4;
	}
	if (!(x & 0xc0000000u)) {
		x <<= 2;
		r -= 2;
	}
	if (!(x & 0x80000000u)) {
		x <<= 1;
		r -= 1;
	}
	return r;
}
#endif//0
#define __set_bit set_bit
#define __clear_bit clear_bit

//#define unlikely(a)  (a)
#define spin_lock_init(a)
#define spin_lock_bh(a)		hal_TaskSuspendAll()
#define spin_unlock_bh(a) 	hal_TaskResumeAll()

#define spin_lock(a)
#define spin_unlock(a) 

#define atbm_spin_lock		spin_lock
#define atbm_spin_unlock  	spin_unlock

#define local_bh_disable()
#define local_bh_enable()

struct rcu_head {
	void * data;
};

#define  rtnl_lock()
#define  rtnl_unlock()
#define rcu_dereference_protected(p, c) (p)
#define rcu_dereference(a) (a)
#define rtnl_dereference(p)	(p)			

#define   rcu_read_lock()          vPortEnterCritical()
#define   rcu_read_unlock()          vPortExitCritical()
#define rcu_assign_pointer(p, v) (p) = (v)
#define rcu_access_pointer(a) (a)
#define RCU_INIT_POINTER(p,v) (p) = (v)

#define   spin_lock_irqsave(a,b)			vPortEnterCritical()
#define   spin_unlock_irqrestore(a,b)		vPortExitCritical()

#define lockdep_assert_held(a)
#define lockdep_is_held  0
#define rcu_dereference_check(a,b) (a)
#define  spinlock_t char

#define list_for_each_entry_rcu list_for_each_entry



#define atomic_add(i, v)	((void)atomic_add_return( (int)(i), (v)))
#define atomic_sub(i, v)	((void)atomic_add_return(-(int)(i), (v)))
#define atomic_inc(v)		((void)atomic_add_return(        1, (v)))
#define atomic_dec(v)		((void)atomic_add_return(       -1, (v)))
#define atomic_sub_return(i, v)	(atomic_add_return(-(int)(i), (v)))
#define atomic_dec_and_test(v)		(atomic_sub_return(1, (v)) == 0)
#define atomic_dec_return(v)	(atomic_sub_return(1, (v)))



#endif
