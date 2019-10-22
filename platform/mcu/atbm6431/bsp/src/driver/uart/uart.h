/**************************************************************************************************************
 * altobeam iot at cmd file 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 * Permission is hereby granted for use on Altobeam IOT 6421 only,
 * in which case ,it is free to  modify it
 *
*****************************************************************************************************************/

#ifndef __APP_UART_H__
#define  __APP_UART_H__

NORELOC void Console_Putc(uint8 Data);
NORELOC unsigned Console_TxBufferSpace(void);
NORELOC int Console_ReadRxFifo(char *pChar);
PUBLIC int Console_ClearRxFifo();
PUBLIC  void Console_SetPolling(int Polling);
int Console_RxFifoFull();
void Console_WriteRxFifo(char Data);
int Console_PeekRxFifo(char *pChar);


void Command_Putc(uint8 Data);
unsigned Command_TxBufferSpace(void);
int Command_ReadRxFifo(char *pChar);
int Command_ClearRxFifo();
void Command_SetPolling(int Polling);
int Command_RxFifoFull();
void Command_WriteRxFifo(char Data);



void UART1_Init();
int UART1_Send(uint8 Data);
int UART1_Recv(uint8 * pData);
int UART1_set_baudrate(unsigned long baudrate);
void UART1_PowerSave_Later();
int UART1_is_have_rx(void);
int UART1_is_can_tx(void);
int UART1_is_tx_empty(void);
int UART1_get_char(void);
void UART1_put_char(int ch);


void UART2_Init();
int UART2_Send(uint8 Data);
int UART2_Recv(uint8 * pData);
int UART2_set_baudrate(unsigned long baudrate);
void UART2_PowerSave_Later();
int UART2_is_have_rx(void);
int UART2_is_can_tx(void);
int UART2_is_tx_empty(void);
int UART2_get_char(void);
void UART2_put_char(int ch);



#if (CHIP_TYPE == CHIP_ATHENAB)
#define CONSOLE_UART(func) UART1_##func
#define CMD_UART(func)     UART1_##func
#define CMD_UART_INTR_NUM  DEV_INT_NUM_UART
#define CONSOLE_UART_INTR_NUM  DEV_INT_NUM_UART
#else //(CHIP_TYPE == CHIP_ARES)

#ifndef CONFIG_UART_CMD_DEBUG //cmd debug used the same uart
#ifdef UART1
#define CONSOLE_UART(func) UART1_##func
#define CMD_UART(func)     UART1_##func
#define CMD_UART_INTR_NUM  DEV_INT_NUM_UART
#define CONSOLE_UART_INTR_NUM  DEV_INT_NUM_UART
#else
#define CONSOLE_UART(func) UART2_##func
#define CMD_UART(func)     UART2_##func
#define CMD_UART_INTR_NUM  DEV_INT_NUM_UART2
#define CONSOLE_UART_INTR_NUM  DEV_INT_NUM_UART2
#endif//#define UART1
#else //CONFIG_UART_CMD_DEBUG ////cmd debug used the diff uart
#define CONSOLE_UART(func) UART2_##func
#define CONSOLE_UART_INTR_NUM  DEV_INT_NUM_UART2
#define CMD_UART(func)     UART1_##func
#define CMD_UART_INTR_NUM  DEV_INT_NUM_UART
#endif//#define CONFIG_UART_CMD_DEBUG

#endif //(CHIP_TYPE == CHIP_ARES)

void CMD_UART_Init();
int  CMD_UART_Recv(uint8 * pData);
int  CMD_UART_Send(uint8 Data);
int  CMD_UART_set_baudrate(unsigned long baudrate);
void CMD_UART_change_params(unsigned long rate, unsigned char data_len,  unsigned char stopbits, unsigned char parity);
void CMD_UART_INTR_En(uint8 en);


void CONSOLE_UART_Init();
int  CONSOLE_UART_Recv(uint8 * pData);
int  CONSOLE_UART_Send(uint8 Data);
void CONSOLE_UART_change_params(unsigned long rate, unsigned char data_len,  unsigned char stopbits, unsigned char parity);

//define the default uart port
#define      UART_Init                     CONSOLE_UART_Init 
#define      UART_Send                     CONSOLE_UART_Send                 
#define      UART_Recv                     CONSOLE_UART_Recv                 
#define      UART_change_params            CONSOLE_UART_change_params



#endif //__APP_UART_H__
