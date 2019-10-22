#ifndef __LWIP_LWIPOPTS_H__
#define __LWIP_LWIPOPTS_H__

#include <config/app_config.h>
#include <time.h>

//#define UDP_DEBUG                       LWIP_DBG_ON

/*
	TCP/IP statck may require RAM up to 35K when runing iperfs or receive buffered.
	use define TCP_IP_REDUCE_MEM_SET will reduce RAM to 15K 
*/
#define MINI_MEMORY_MODE 0
#if MINI_MEMORY_MODE
#define TCP_IP_REDUCE_MEM_SET 			1
#endif //#if MINI_MEMORY_MODE

#define MEM_LIBC_MALLOC                 1
#define MEMP_LIBC_MALLOC                1

#define MEMP_MEM_MALLOC                 1

#define LWIP_SOCKET			1
#define LWIP_DHCP			1
#define LWIP_NETCONN        1

/* XXX sys_arch_mbox_tryfetch may need to implement again too 
 * timeout argument can't use 1 because it will be converts to tick */
//#define sys_arch_mbox_tryfetch(mbox,msg) sys_arch_mbox_fetch(mbox,msg,10)
/*
 * MBOX size
 * */
#ifdef TCP_IP_REDUCE_MEM_SET
#define TCPIP_MBOX_SIZE                 20
#define DEFAULT_RAW_RECVMBOX_SIZE       8
#define DEFAULT_UDP_RECVMBOX_SIZE       8
#define DEFAULT_TCP_RECVMBOX_SIZE       8
#else
#define TCPIP_MBOX_SIZE                 32
#define DEFAULT_RAW_RECVMBOX_SIZE       16
#define DEFAULT_UDP_RECVMBOX_SIZE       16
#define DEFAULT_TCP_RECVMBOX_SIZE       16
#endif
#define DEFAULT_ACCEPTMBOX_SIZE         8
#define SYS_LIGHTWEIGHT_PROT            1
/* DNS */
#define LWIP_DNS                        1

#define LWIP_IGMP                       1
#define DUMMY_IGMP_TIMER				1
/* TCP */
#define TCP_MSS                         (1500-40)/*TCP_MSS=(MTU-IP header size - TCP head size)*/
#define TCP_SND_BUF                     (TCP_MSS * 2)
#define TCP_SND_QUEUELEN                (8 * (TCP_SND_BUF/TCP_MSS + 1))
#define MEMP_NUM_TCP_SEG                TCP_SND_QUEUELEN
#define TCP_SNDLOWAT                    ((TCP_SND_BUF / 5) * 2)

#ifdef TCP_IP_REDUCE_MEM_SET
#define TCP_WND                         (6*TCP_MSS) 
#else
#define TCP_WND                         (15*TCP_MSS) 
#endif

#define LWIP_SO_RCVTIMEO                1
#define IP_FORWARD                		1
#define MEMP_NUM_NETCONN                6 /*socket num*/
#define MEMP_NUM_TCP_PCB_LISTEN         7 /*socket num*/
#define MEMP_NUM_TCP_PCB         		8 /*socket num*/
#define MEMP_NUM_UDP_PCB         		4 /*socket num*/
#define MEMP_NUM_RAW_PCB         		4 /*socket num*/


#define TCP_TMR_INTERVAL       			250  /* The TCP timer interval in milliseconds. default is 250*/

#define TCP_MSL 						(TCP_TMR_INTERVAL)


#define WEB_SERVER_SOCKET_NUM			2
#define WEB_CHILD_SOCKET_NUM			2
#define WEB_CHILD_SOCKET_NUM_MAX		2
#define DHCP_SERVER_SOCKET_NUM			14
#define INVALID_SOCKET 					0xffff


#define MEMP_NUM_SYS_TIMEOUT            16

#define MEM_ALIGNMENT                   4

#define TCPIP_THREAD_PRIO               (32 -2-  CONFIG_TCPIP_THREAD_PRIORITY)
#define TCPIP_THREAD_STACKSIZE			TASK_TCPIP_THREAD_STACK_SIZE
#define SLIPIF_THREAD_STACKSIZE         1024
#define DEFAULT_THREAD_STACKSIZE        256
#define DNS_USES_STATIC_BUF				2 /*used mem*/

#define SNMP_SAFE_REQUESTS              0

#define  LWIP_TIMEVAL_PRIVATE 			0

#define LWIP_PRIVATE_FD_SET
#define LWIP_TCPIP_CORE_LOCKING 		1

#define LWIP_IPV4 						1
#define LWIP_IPV6                       0
#define LWIP_RAW                        1

#define LWIP_DONT_PROVIDE_BYTEORDER_FUNCTIONS 1
#define TCP_FAST_REMOVE				1


/*
   ---------- Socket options ----------
*/
#define LWIP_COMPAT_SOCKETS             1
#define LWIP_POSIX_SOCKETS_IO_NAMES     1
#if !defined(FD_SET) && defined(RHINO_CONFIG_VFS_DEV_NODES)
#define LWIP_SOCKET_OFFSET              RHINO_CONFIG_VFS_DEV_NODES
#endif
#define LWIP_SO_SNDTIMEO                1
#define LWIP_SO_RCVTIMEO                1
#define SO_REUSE 						1
#define SO_REUSE_RXTOALL 				1

#define PBUF_WIFI_HEADROOM  			64
#define PBUF_LINK_ENCAPSULATION_HLEN 	PBUF_WIFI_HEADROOM

/**
 * LWIP_NETIF_HOSTNAME==1: use DHCP_OPTION_HOSTNAME with netif's hostname
 * field.
 */
#define LWIP_NETIF_HOSTNAME             1

/**
 * LWIP_NETIF_TX_SINGLE_PBUF: if this is set to 1, lwIP tries to put all data
 * to be sent into one single pbuf. This is for compatibility with DMA-enabled
 * MACs that do not support scatter-gather.
 * Beware that this might involve CPU-memcpy before transmitting that would not
 * be needed without this flag! Use this only if you need to!
 *
 * @todo: TCP and IP-frag do not work with this, yet:
 */
#define LWIP_NETIF_TX_SINGLE_PBUF             1

/**
 * Enable TCP_KEEPALIVE
 */
#define LWIP_TCP_KEEPALIVE              1

//#include <arch/debug.h>
#define LWIP_DEBUG							0
#define TCP_QUEUE_OOSEQ						1
#define IP_REASS_MAX_PBUFS              	0
#define IP_REASSEMBLY                   	0
#define IP_REASS_MAX_PBUFS              	0
#define IP_REASSEMBLY                   	0
#define MEMP_NUM_REASSDATA              	0
#define IP_FRAG                         	0
#define LWIP_RANDOMIZE_INITIAL_LOCAL_PORTS 	1


/**
 * CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets.
 */
#define CHECKSUM_GEN_IP                 1
 
/**
 * CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets.
 */
#define CHECKSUM_GEN_UDP                1
 
/**
 * CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets.
 */
#define CHECKSUM_GEN_TCP                1
 
/**
 * CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.
 */
#define CHECKSUM_CHECK_IP               1
 
/**
 * CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.
 */
#define CHECKSUM_CHECK_UDP              1

/**
 * CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets.
 */
#define CHECKSUM_CHECK_TCP              1

#endif /* __LWIP_LWIPOPTS_H__ */
