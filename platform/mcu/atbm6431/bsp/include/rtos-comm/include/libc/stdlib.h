#ifndef __STDLIB_H__
#define __STDLIB_H__
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#ifndef NULL
#define NULL ((void*)0)
#endif //NULL
//extern NORELOC void *memcpy(void *dest, const void *src, unsigned n);
#ifdef ATBM_PRINT_FLAG
#define iot_printf(...) (printf("[atbm debug]"),printf(__VA_ARGS__))
#define DBG_Printf(...) (printf("[atbm debug]"),printf(__VA_ARGS__))
#define AT_Printf(...) 	(printf("[atbm debug]"),printf(__VA_ARGS__))
#else
#define iot_printf 	printf
#define DBG_Printf  printf
#define AT_Printf 	printf
#endif 
//extern void DBG_Printf(const char * fmt, ...);

extern  void  RTL_ZeroMemory(void *pMem,int Size);
//extern  void  RTL_CopyMemory(void *pDst, void *pSrc, int Size);
extern void RTL_CopyMemory32(void *pDst, void *pSrc, int Size);

#define GFP_KERNEL 	0x0001
#define GFP_DMA		0x0002
#define GFP_SRAM	0x0004
#define GFP_ATOMIC	0x0008
enum MEM_TYPE{
	MEM_HEAP=0,
	MEM_SRAM,
	MEM_MAX
};

extern void *pvPortMalloc( size_t xWantedSize );
extern void vPortFree( void *pv );
extern 	void sys_heapfree( void *pv );
extern void *pvPortHeapMalloc(size_t xWantedSize, enum MEM_TYPE memtype  );
extern void vPortHeapFree( void *pv,enum MEM_TYPE memtype );
extern int vPortHeapBlockSize(void *pv );


//#define DEBUG_MALLOC_LIST
#ifdef DEBUG_MALLOC_LIST
extern void *pvPortMalloc_DEBUG( size_t xWantedSize,unsigned long call_addr );
#define malloc(a)	pvPortMalloc_DEBUG(a,(unsigned long)__builtin_return_address(0))
#define os_malloc malloc
#else
#define os_malloc	malloc
void * malloc(size_t xWantedSize);
void free( void *pv );

#endif //

#define os_free	free


#define os_memset memset
#define os_memzero(a,b,l) RTL_ZeroMemory((a),(l))
#define os_memcpy 			memcpy
#define os_memcpy32(a,b,l) RTL_CopyMemory32(a,b,(l+3)>>2)
#define os_zalloc zalloc
//#define os_malloc malloc
//#define os_free   free
#define os_memcmp  memcmp




//#define DEBUG_MALLOC
#ifdef DEBUG_MALLOC
void *pvPortHeapMalloc_DEBUG(size_t xWantedSize, enum MEM_TYPE memtype ,unsigned long call_addr );
#define smalloc(size) ({\
	char * buf = (char *)pvPortHeapMalloc_DEBUG(size,MEM_SRAM,(unsigned long)__builtin_return_address(0)); \
	/*DBG_Printf("smalloc %s %d:%x:%d\n",__func__,__LINE__,(int)buf,size);*/\
	(void * )buf; \
})

#define kmalloc(size) ( {\
	char * buf = (char *)smalloc(size); \
	/*DBG_Printf("kmalloc %s %d:%x:%d\n",__func__,__LINE__,(u32)buf,size);*/\
	(void * )buf; \
})

#define kfree(addr) do{	 \
	/*DBG_Printf("kfree %s %d:%x\n",__func__,__LINE__,addr);*/\
	sys_heapfree(addr); \
}while(0)

#define kfree_check(addr) do{	 \
	/*DBG_Printf("kchk %s %d:%x\n",__func__,__LINE__,addr);*/\
	sys_heapfree_check(addr); \
}while(0)

#else /*DEBUG_MALLOC*/
static  inline char * smalloc(int  s){
 	return malloc(s);
}

#define kmalloc(size)  	smalloc(size)

#define kfree(addr)		free(addr)

#define kfree_check(addr)

#endif /*DEBUG_MALLOC*/
void sfree(void *pv );
void *zalloc(size_t size); 
void *scalloc(size_t nmemb, size_t size);
void *realloc(void *pointer, size_t size);
void qsort(void *base, size_t nel, size_t width, int (*compar)(const void *, const void *));
extern int	atoi(const char *__nptr);
void sys_heapShow(enum MEM_TYPE memtype);
extern double strtod(const char* s, char** endptr);

#endif /* __STDLIB_H__ */
