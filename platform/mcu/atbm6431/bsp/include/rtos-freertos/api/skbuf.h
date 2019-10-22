#ifndef __SKBUF_H_
#define __SKBUF_H_
#include "stdlib.h"

//#include "hal.h"
#include "pbuf.h"

#include "lwip/sys.h"
#include "lwip/netif.h"
#include "atomic.h"
#define net_device  netif
#define sk_buff pbuf

void *netdev_drv_priv (struct netif * netdev);

struct tcpip_opt{
	void (*net_init)(struct net_device *dev);
	void (*net_enable)(struct net_device *dev);
	void (*net_disable)(struct net_device *dev);
	void (*net_rx)(struct net_device *dev, struct sk_buff * skb);
	void (*net_tx_done)(struct net_device *dev);
	void (*net_start_queue)(struct net_device *dev,int);
	void (*net_stop_queue)(struct net_device *dev,int);
	void (*net_task_event)(struct net_device *dev);	
};

struct net_device_ops{
	int	(*ndo_open)(struct net_device *dev);
	int (*ndo_stop)(struct net_device *dev);
	int  (*ndo_start_xmit)(struct net_device *dev,struct sk_buff *skb );
	int (*ndo_set_mac_address)(struct net_device *dev,u8 *addr);
};

struct sk_buff_head {
	/* These two members must be first. */
	struct sk_buff	*next;
	struct sk_buff	*prev;
	u16				qlen;
	spinlock_t	lock;
};

struct netif * alloc_netdev(int size);
void free_netdev(struct netif * netdev);



//#define DEBUG_MALLOC_SKB
#ifdef DEBUG_MALLOC_SKB

inline struct pbuf * dev_alloc_skb2(int len);
int dev_kfree_skb2(struct pbuf * skb);

#define dev_alloc_skb(size) ( {\
	char * buf = (char *)dev_alloc_skb2(size); \
	iot_printf("<DEBUG_SKB>alloc_skb %s %d:%x\n",__func__,__LINE__,(u32)buf);\
	(void * )buf;\
})

#define dev_kfree_skb(addr)	dev_kfree_skb2(addr)

#else /*DEBUG_MALLOC_SKB*/

struct pbuf * dev_alloc_skb(int len);
int dev_kfree_skb(struct pbuf * skb);

#endif /*DEBUG_MALLOC_SKB*/

void skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk);
struct sk_buff *skb_dequeue(struct sk_buff_head *list);
void skb_queue_purge(struct sk_buff_head *list);
struct sk_buff *skb_copy(const struct sk_buff *skb, gfp_t gfp_mask);
void netdev_attach_ops (struct netif * netdev, struct net_device_ops * net_ops);

#define alloc_skb(len,a) dev_alloc_skb(len)

#define kfree_skb dev_kfree_skb

#define compare_ether_addr(a,b) (!((((char *)(a))[0] == ((char *)(b))[0]) && \
								(((char *)(a))[1] == ((char *)(b))[1]) && \
								(((char *)(a))[2] == ((char *)(b))[2]) && \
								(((char *)(a))[3] == ((char *)(b))[3]) && \
								(((char *)(a))[4] == ((char *)(b))[4]) && \
								(((char *)(a))[5] == ((char *)(b))[5])))

//#define compare_ether_addr(a,b) memcmp(a,b,6)
#define is_multicast_ether_addr(addr) ((((unsigned char *)addr)[0])& 0x1)
/**
 * is_broadcast_ether_addr - Determine if the Ethernet address is broadcast
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Return true if the address is the broadcast address.
 */
static inline int is_broadcast_ether_addr(const u8 *addr)
{
	return (addr[0] & addr[1] & addr[2] & addr[3] & addr[4] & addr[5]) == 0xff;
}

#define broadcast_ether_addr (const u8 *) "\xff\xff\xff\xff\xff\xff"


#define netif_wake_subqueue(a,b)
#define netif_stop_subqueue(a,b)
#define synchronize_net()


#define OS_SKB_LEN(skb)   (skb)->data_len
#define OS_SKB_DATA(skb)   (((skb)->payload))


#define  IEEE80211_SKB_CB(skb) ((struct atbmwifi_ieee80211_tx_info *)((skb)->cb))
#define  IEEE80211_SKB_RXCB(skb) ((struct atbmwifi_ieee80211_rx_status *)((skb)->cb))

/**
 *	__skb_queue_head_init - initialize non-spinlock portions of sk_buff_head
 *	@list: queue to initialize
 *
 *	This initializes only the list and queue length aspects of
 *	an sk_buff_head object.  This allows to initialize the list
 *	aspects of an sk_buff_head without reinitializing things like
 *	the spinlock.  It can also be used for on-stack sk_buff_head
 *	objects where the spinlock is known to not be used.
 */
static inline void __skb_queue_head_init(struct sk_buff_head *list)
{
	list->prev = list->next = (struct sk_buff *)list;
	list->qlen = 0;
}

/*
 * This function creates a split out lock class for each invocation;
 * this is needed for now since a whole lot of users of the skb-queue
 * infrastructure in drivers have different locking usage (in hardirq)
 * than the networking core (in softirq only). In the long run either the
 * network layer or drivers should need annotation to consolidate the
 * main types of usage into 3 classes.
 */
static inline void skb_queue_head_init(struct sk_buff_head *list)
{
	spin_lock_init(&list->lock);
	__skb_queue_head_init(list);
}


/**
 *	skb_peek_tail - peek at the tail of an &sk_buff_head
 *	@list_: list to peek at
 *
 *	Peek an &sk_buff. Unlike most other operations you _MUST_
 *	be careful with this one. A peek leaves the buffer on the
 *	list and someone else may run off with it. You must hold
 *	the appropriate locks or have a private queue to do this.
 *
 *	Returns %NULL for an empty list or a pointer to the tail element.
 *	The reference count is not incremented and the reference is therefore
 *	volatile. Use with caution.
 */
static inline struct sk_buff *skb_peek_tail(struct sk_buff_head *list_)
{
	struct sk_buff *list = ((struct sk_buff *)list_)->prev;
	if (list == (struct sk_buff *)list_)
		list = NULL;
	return list;
}

/**
 *	skb_queue_len	- get queue length
 *	@list_: list to measure
 *
 *	Return the length of an &sk_buff queue.
 */
static inline __u32 skb_queue_len(const struct sk_buff_head *list_)
{
	return list_->qlen;
}
/**
 *	skb_queue_empty - check if a queue is empty
 *	@list: queue head
 *
 *	Returns true if the queue is empty, false otherwise.
 */
static inline int skb_queue_empty(const struct sk_buff_head *list)
{
	return list->next == (struct sk_buff *)list;
}

/**
 *	skb_queue_is_last - check if skb is the last entry in the queue
 *	@list: queue head
 *	@skb: buffer
 *
 *	Returns true if @skb is the last buffer on the list.
 */
static inline bool skb_queue_is_last(const struct sk_buff_head *list,
				     const struct sk_buff *skb)
{
	return (skb->next == (struct sk_buff *) list);
}

/**
 *	skb_queue_is_first - check if skb is the first entry in the queue
 *	@list: queue head
 *	@skb: buffer
 *
 *	Returns true if @skb is the first buffer on the list.
 */
static inline bool skb_queue_is_first(const struct sk_buff_head *list,
				      const struct sk_buff *skb)
{
	return (skb->prev == (struct sk_buff *) list);
}

/**
 *	skb_queue_next - return the next packet in the queue
 *	@list: queue head
 *	@skb: current buffer
 *
 *	Return the next packet in @list after @skb.  It is only valid to
 *	call this if skb_queue_is_last() evaluates to false.
 */
static inline struct sk_buff *skb_queue_next(const struct sk_buff_head *list,
					     const struct sk_buff *skb)
{
	/* This BUG_ON may seem severe, but if we just return then we
	 * are going to dereference garbage.
	 */
	BUG_ON(skb_queue_is_last(list, skb));
	return skb->next;
}

static inline void __skb_queue_splice(const struct sk_buff_head *list,
				      struct sk_buff *prev,
				      struct sk_buff *next)
{
	struct sk_buff *first = list->next;
	struct sk_buff *last = list->prev;

	first->prev = prev;
	prev->next = first;

	last->next = next;
	next->prev = last;
}

static inline void skb_queue_splice_tail_init(struct sk_buff_head *list,
					      struct sk_buff_head *head)
{
	if (!skb_queue_empty(list)) {
		__skb_queue_splice(list, head->prev, (struct sk_buff *) head);
		head->qlen += list->qlen;
		__skb_queue_head_init(list);
	}
}



/*
 *	Add data to an sk_buff
 */
static inline unsigned char *skb_put(struct sk_buff *skb, unsigned int len)
{
	unsigned char *tmp = skb->tail;
	skb->tail += len;
	OS_SKB_LEN(skb)  += len;
	return tmp;
}

static inline unsigned char *skb_push(struct pbuf *skb, unsigned int len)
{
 	OS_SKB_DATA(skb) -= len;
 	OS_SKB_LEN(skb)  += len;
	return OS_SKB_DATA(skb);
}

static inline unsigned char * skb_pull(struct sk_buff *skb, unsigned int len)
{
	OS_SKB_LEN(skb) -= len;
	return OS_SKB_DATA(skb) += len;
}

/**
 *	skb_headroom - bytes at buffer head
 *	@skb: buffer to check
 *
 *	Return the number of bytes of free space at the head of an &sk_buff.
 */
static inline unsigned int skb_headroom(const struct sk_buff *skb)
{
	return (u32)OS_SKB_DATA(skb) - (u32)(skb + 1);
}

/**
 *	skb_tailroom - bytes at buffer end
 *	@skb: buffer to check
 *
 *	Return the number of bytes of free space at the tail of an sk_buff
 */
static inline int skb_tailroom(const struct sk_buff *skb)
{
	//return (int)(((char *)(u32)(skb + 1)+ skb->len + ) - (char *)skb->tail);
	return (((u32)skb + skb->buffer_len) - (u32)skb->tail);
}

/**
 *	skb_reserve - adjust headroom
 *	@skb: buffer to alter
 *	@len: bytes to move
 *
 *	Increase the headroom of an empty &sk_buff by reducing the tail
 *	room. This is only allowed for an empty buffer.
 */
static inline int skb_reserve(struct sk_buff *skb, int len)
{
	//char * buf = OS_SKB_DATA(skb);
	WARN_ON(skb->data_len > 0);
	if(skb->len < len){
		return -1;
	}
	OS_SKB_DATA(skb) += len;
	skb->tail = OS_SKB_DATA(skb);
	skb->tot_len -= len;
	skb->len -= len;	
	return 0;
}


static inline void skb_set_tail_pointer(struct sk_buff *skb, const int offset)
{
	skb->tail = OS_SKB_DATA(skb) + offset;
}

static inline void skb_trim(struct sk_buff *skb, unsigned int len)
{

	OS_SKB_LEN(skb) = len;
	skb_set_tail_pointer(skb, len);
}

static inline void __skb_insert(struct sk_buff *newsk,
				struct sk_buff *prev, struct sk_buff *next,
				struct sk_buff_head *list)
{
	newsk->next = next;
	newsk->prev = prev;
	next->prev  = prev->next = newsk;
	list->qlen++;
}
/**
 *	__skb_queue_after - queue a buffer at the list head
 *	@list: list to use
 *	@prev: place after this buffer
 *	@newsk: buffer to queue
 *
 *	Queue a buffer int the middle of a list. This function takes no locks
 *	and you must therefore hold required locks before calling it.
 *
 *	A buffer cannot be placed on two lists at the same time.
 */
static inline void __skb_queue_after(struct sk_buff_head *list,
				     struct sk_buff *prev,
				     struct sk_buff *newsk)
{
	__skb_insert(newsk, prev, prev->next, list);
}

 static inline void __skb_queue_before(struct sk_buff_head *list,
				      struct sk_buff *next,
				      struct sk_buff *newsk)
{
	__skb_insert(newsk, next->prev, next, list);
}
static inline void __skb_queue_head(struct sk_buff_head *list,
				    struct sk_buff *newsk)
{
	__skb_queue_after(list, (struct sk_buff *)list, newsk);
}

/**
 *	__skb_queue_tail - queue a buffer at the list tail
 *	@list: list to use
 *	@newsk: buffer to queue
 *
 *	Queue a buffer at the end of a list. This function takes no locks
 *	and you must therefore hold required locks before calling it.
 *
 *	A buffer cannot be placed on two lists at the same time.
 */
static inline void __skb_queue_tail(struct sk_buff_head *list,
				   struct sk_buff *newsk)
{
	__skb_queue_before(list, (struct sk_buff *)list, newsk);
}
static inline void __skb_unlink(struct sk_buff *skb, struct sk_buff_head *list)
{
	struct sk_buff *next, *prev;

	list->qlen--;
	next	   = skb->next;
	prev	   = skb->prev;
	skb->next  = skb->prev = NULL;
	next->prev = prev;
	prev->next = next;
}
/**
 *	skb_peek - peek at the head of an &sk_buff_head
 *	@list_: list to peek at
 *
 *	Peek an &sk_buff. Unlike most other operations you _MUST_
 *	be careful with this one. A peek leaves the buffer on the
 *	list and someone else may run off with it. You must hold
 *	the appropriate locks or have a private queue to do this.
 *
 *	Returns %NULL for an empty list or a pointer to the head element.
 *	The reference count is not incremented and the reference is therefore
 *	volatile. Use with caution.
 */
static inline struct sk_buff *skb_peek(struct sk_buff_head *list_)
{
	struct sk_buff *list = ((struct sk_buff *)list_)->next;
	if (list == (struct sk_buff *)list_)
		list = NULL;
	return list;
}
/**
 *	__skb_dequeue_tail - remove from the tail of the queue
 *	@list: list to dequeue from
 *
 *	Remove the tail of the list. This function does not take any locks
 *	so must be used with appropriate locks held only. The tail item is
 *	returned or %NULL if the list is empty.
 */
static inline struct sk_buff *__skb_dequeue_tail(struct sk_buff_head *list)
{
	struct sk_buff *skb = skb_peek_tail(list);
	if (skb)
		__skb_unlink(skb, list);
	return skb;
}

/**
 *	__skb_dequeue - remove from the head of the queue
 *	@list: list to dequeue from
 *
 *	Remove the head of the list. This function does not take any locks
 *	so must be used with appropriate locks held only. The head item is
 *	returned or %NULL if the list is empty.
 */
static inline struct sk_buff *__skb_dequeue(struct sk_buff_head *list)
{
	struct sk_buff *skb = skb_peek(list);
	if (skb)
		__skb_unlink(skb, list);
	return skb;
}

int rx_skb_cnt();


#endif /*__SKBUF_H_*/
