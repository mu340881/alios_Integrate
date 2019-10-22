#ifndef _API_DEBUG_H
#define _API_DEBUG_H

#include "stdio.h"
#include "hal.h"

#define SYS_MONITOR    		0

#ifdef API_DEBUG
#define p_info(...) do{iot_printf("[I: %d.%03d] ",  xTaskGetTickCount()/1000, xTaskGetTickCount()%1000);iot_printf(__VA_ARGS__); iot_printf("\r\n");}while(0)
#define p_err(...) do{iot_printf("[E: %d.%03d] ",  xTaskGetTickCount()/1000, xTaskGetTickCount()%1000);iot_printf(__VA_ARGS__); iot_printf("\r\n");}while(0)
#define p_dbg_track do{iot_printf("[D: %d.%03d] ",  xTaskGetTickCount()/1000, xTaskGetTickCount()%1000);iot_printf("%s,%d",  __FUNCTION__, __LINE__, ); iot_printf("\r\n");}while(0)
#define p_dbg(...) do{iot_printf("[D: %d.%03d] ",  xTaskGetTickCount()/1000, xTaskGetTickCount()%1000);iot_printf(__VA_ARGS__); iot_printf("\r\n");}while(0)
#define p_dbg_enter do{iot_printf("[D: %d.%03d] ",  xTaskGetTickCount()/1000, xTaskGetTickCount()%1000);iot_printf("enter %s\n", __FUNCTION__); iot_printf("\r\n");}while(0)
#define p_dbg_exit do{iot_printf("[D: %d.%03d] ",  xTaskGetTickCount()/1000, xTaskGetTickCount()%1000);iot_printf("exit %s\n", __FUNCTION__); iot_printf("\r\n");}while(0)
#define p_dbg_status do{iot_printf("[D: %d.%03d] ",  xTaskGetTickCount()/1000, xTaskGetTickCount()%1000);iot_printf("status %d\n", status); iot_printf("\r\n");}while(0)
#define p_err_miss do{iot_printf("[E: %d.%03d] ",  xTaskGetTickCount()/1000, xTaskGetTickCount()%1000);iot_printf("%s miss\n", __FUNCTION__); iot_printf("\r\n");}while(0)
#define p_err_mem do{iot_printf("[E: %d.%03d] ",  xTaskGetTickCount()/1000, xTaskGetTickCount()%1000);iot_printf("%s mem err\n", __FUNCTION__); iot_printf("\r\n");}while(0)
#define p_err_fun do{iot_printf("[E: %d.%03d] ",  xTaskGetTickCount()/1000, xTaskGetTickCount()%1000);iot_printf("%s err in %d\n", __FUNCTION__, __LINE__); iot_printf("\r\n");}while(0)

#else
#define p_info(...) 	//do{iot_printf("[I: %d.%03d] ",  xTaskGetTickCount()/1000, xTaskGetTickCount()%1000); iot_printf(__VA_ARGS__); iot_printf("\r\n");}while(0)
#define p_err(...) 	do{ iot_printf(__VA_ARGS__); }while(0)
#define p_err_miss do{iot_printf("%s miss\n", __FUNCTION__);}while(0)
#define p_err_mem do{iot_printf("%s mem err\n", __FUNCTION__); }while(0)
#define p_err_fun do{iot_printf("%s err in %d\n", __FUNCTION__, __LINE__); }while(0)

#define p_dbg_track
#define p_dbg(...)
#define p_dbg_exit
#define p_dbg_enter
#define p_dbg_status


#endif


//#define LWIP_NOASSERT	///关闭lwip的assert，调试时注意打开



#define assert(x)                                                               \
do{                                                                              		 \
if (!(x))                                                                   	\
{                                                                           			\
p_err( "%s:%d assert " #x "failed\r\n", __FILE__, __LINE__);	\
while(1);										\
}                                                                           			\
}while(0)


#if SYS_MONITOR

typedef enum
{
	PRINT_TYPE_D, PRINT_TYPE_H, PRINT_TYPE_P_D, PRINT_TYPE_P_H,
} STAT_PRINT_TYPE;

struct dev_monitor_item
{
	const char *name;
	uint32_t value;
	//  STAT_PRINT_TYPE type;
};

#define INC_MONITER_ERR_VALUE(x)    (x->err_cnt)++
#define ADD_MONITER_READ_VALUE(x, v)    (x->read_total_size) += (v)
#define ADD_MONITER_WRITE_VALUE(x, v)    (x->write_total_size) += (v)

#define INC_MONITOR_VALUE(X) X.value++
#define ADD_MONITOR_VALUE(X, V) X.value += V
#define SET_MONITER_VALUE(X, V)  X.value = V

#define DECLARE_MONITOR_ITEM(TAG, NAME)	 struct dev_monitor_item NAME  \
__attribute__((section("monitor_item_table"))) = {TAG, 0}

#define EXTERN_MONITOR_ITEM(NAME)	 extern struct dev_monitor_item NAME;


#define INC_MONITOR_ITEM_VALUE(X)  	X.value++
#define DEC_MONITOR_ITEM_VALUE(X)  	X.value--
#define SET_MONITOR_ITEM_VALUE(X, V)  	X.value = V
#define ADD_MONITOR_ITEM_VALUE(X, V) 	X.value += V
#define SUB_MONITOR_ITEM_VALUE(X, V) 	X.value -= V
#define GET_MONITOR_ITEM_VALUE(X) 	X.value

#else
#define DECLARE_MONITOR_ITEM(TAG, NAME)
#define INC_MONITOR_ITEM_VALUE(X)
#define DEC_MONITOR_ITEM_VALUE(X)
#define SET_MONITOR_ITEM_VALUE(X, V)
#define ADD_MONITOR_ITEM_VALUE(X, V)
#define SUB_MONITOR_ITEM_VALUE(X, V)

#define INC_MONITER_ERR_VALUE(x)
#define ADD_MONITER_READ_VALUE(x, v)
#define ADD_MONITER_WRITE_VALUE(x, v)

#define INC_MONITOR_VALUE(X)
#define ADD_MONITOR_VALUE(X, V)
#define SET_MONITER_VALUE(X, V)
#define GET_MONITOR_ITEM_VALUE(X)
#define EXTERN_MONITOR_ITEM(NAME)

#endif


#endif
