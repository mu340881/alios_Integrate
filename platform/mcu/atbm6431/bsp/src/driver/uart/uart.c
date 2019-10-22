/**************************************************************************************************************
 * altobeam iot uart driver file 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 * Permission is hereby granted for use on Altobeam IOT 6421 only,
 * in which case ,it is free to  modify it
 *
*****************************************************************************************************************/

#include "hal.h"
#include "uart.h"
#include "app_AT_cmd.h"
//#include "console.h"
#ifdef CONFIG_AT_CMD_SUPPORT

u32_t Uart_TSPT_Enable_flag = 0;
char  UART_TSPT_RxStr[TSPT_RX_BUF_SIZE];
u32_t Uart_TSPT_buffer_len = 0;
//if enable uart echo back 
#ifdef CONFIG_UART_ECHO
u32_t Uart_TSPT_Echo_flag = 1;
#else //CONFIG_UART_ECHO
u32_t Uart_TSPT_Echo_flag = 0;
#endif //CONFIG_UART_ECHO

extern BOOL IPMODE;

/*
uart transparent tx,
this function can't be close by hal_uart_dbg_close
*/
FLASH_FUNC void UART_TSPT_Sendbuff(char *str,int num)
{
	int i=0;
	Command_SetPolling(1);
	for(i = 0; i<num;i++) 
	{
		 char ch;
		 ch = *str;
		 
		 if (ch == '\n')
		 {
			 Command_Putc('\r');
		 }
         //iot_printf("ch = %c \n",ch);

		 Command_Putc(ch);

		 str++;
		 if (ch == '\n')
		 {
			 break;
		 }
	}
	Command_SetPolling(0);
}
/*
uart transparent string,
this function can't be close by hal_uart_dbg_close
*/
FLASH_FUNC void UART_TSPT_SendStr(const char *str)
{
	for (;;)
	{
		char ch;

		ch = *str;
		str++;
		
		if (ch == 0)
		{
			break;
		}

		if (ch == '\n')
		{
			Command_Putc('\r');

		}

	    Command_Putc(ch);

	}
}


FLASH_FUNC char * UART_TSPT_NetData_GetLine(void)
{
	unsigned	Count;
	

	Count = Uart_TSPT_buffer_len;;

	while(Count < (TSPT_RX_BUF_SIZE-1))
	{
		char Ch;

		if (!Command_ReadRxFifo(&Ch))
		{
			break;
		}
		UART_TSPT_RxStr[Count] = Ch;
		if ( (Ch == '\r') || (Ch == '\n') || (Count >= (TSPT_RX_BUF_SIZE-2)) )
		{
			//UART_TSPT_RxStr[Count] = 0;
			Uart_TSPT_buffer_len = 0;
			//OS_RestoreFiqIrq( IState);
			return UART_TSPT_RxStr;
		}
		Count++;
	}
	Uart_TSPT_buffer_len = Count;
	/*
	if(Uart_TSPT_buffer_len > 0)
	{
		 iot_printf("UART_TSPT_RxStr %s\n",UART_TSPT_RxStr);
	}
	*/
	return NULL;
}

FLASH_FUNC char * UART_ETF_KeyData_GetLine(void)
{
	unsigned	Count;
	

	Count = Uart_TSPT_buffer_len;;

	while(Count < (TSPT_RX_BUF_SIZE-1))
	{
		char Ch;

		if (!Command_ReadRxFifo(&Ch))
		{
			break;
		}
		UART_TSPT_RxStr[Count] = Ch;
		if ((Ch == '}') || (Ch == '\r') || (Ch == '\n') || (Count >= (TSPT_RX_BUF_SIZE-2)))
		{
			//UART_TSPT_RxStr[Count] = 0;
			Uart_TSPT_buffer_len = 0;
			//OS_RestoreFiqIrq( IState);
			return UART_TSPT_RxStr;
		}
		Count++;
	}
	Uart_TSPT_buffer_len = Count;
	return NULL;
}



/**************************************************************************
**
** NAME     atbm_uart_read
**
** PARAMETERS:  pData
**
** RETURNS:
**
** DESCRIPTION  Read a byte from the UART port.
**
** NOTES:       None.
**************************************************************************/
PRIVATE FLASH_FUNC int atbm_uart_read(uint8 * pData)
{
	if(CMD_UART(is_have_rx)()) 
	{
		*pData = CMD_UART(get_char)();
		if((Uart_TSPT_Echo_flag == 1) && CMD_UART(is_can_tx)())
		{
			CMD_UART(put_char)(*pData);
		}
		//let os not goto sleep  immediately 
		//wakeup 10 second
		CMD_UART(PowerSave_Later)();
		return TRUE;
	}
	return FALSE;
}


PRIVATE NO_FLASH_FUNC2 void atbm_uart_tspt_rx(void)
{
	static uint32 Uart_TSPT_exit =0;;

    while(Command_RxFifoFull()==0)
    {
        uint8 Data;

        if (!atbm_uart_read(&Data))
        {
            break;
        }
        Command_WriteRxFifo(Data);
		//at cmd "+++"  is used to exit uart transparent mode
		if(Data == '+'){
			Uart_TSPT_exit++;
		    if(Uart_TSPT_exit >= 3)
		    {
		    	//exit uart transparent mode
		         UART_TSPT_SendStr("+++a\n");

		         Command_ClearRxFifo();
		         //memset(Console.RxBuffer, 0, sizeof(Console.RxBuffer));
		         Uart_TSPT_Enable_flag = 0;
				 AT_socketmode_end();
				 Uart_TSPT_exit= 0;
		    }
		}
		else {
			Uart_TSPT_exit= 0;
		}
	    if((Data == '\n') || (Data == '\r') || (Data == '\t'))
	    {
	        break;
	    }
    }
}

NO_FLASH_FUNC PRIVATE void atbm_uart_rxhander()
{

   if(Uart_TSPT_Enable_flag == 0)
   {
	   Command_ReadDataInIrq();
   }
   else if(Uart_TSPT_Enable_flag == 1)
   {
  	   atbm_uart_tspt_rx();
   }
}


NO_FLASH_FUNC PRIVATE void atbm_uart_txhander()
{    
    CMD_UART(TxCmdEmptyIrqHandler)();
}
#endif //#ifdef CONFIG_AT_CMD_SUPPORT

NO_FLASH_FUNC NORELOC PRIVATE void atbm_uart_console_hander(void)
{

	if(CONSOLE_UART(is_have_rx)()) 
	{
		CONSOLE_UART(RxTriggerIrqHandler)(0);
	}

	if(CONSOLE_UART(is_can_tx)())
	{
		CONSOLE_UART(TxEmptyIrqHandler)(0);
	}

}

NO_FLASH_FUNC NORELOC PRIVATE void atbm_uart_cmd_hander(int Priority)
{
#ifdef CONFIG_AT_CMD_SUPPORT
	if(CMD_UART(is_have_rx)())
	{
		atbm_uart_rxhander();
	}

	if(CMD_UART(is_can_tx)())
	{
		atbm_uart_txhander();
	}
	if(CMD_UART_INTR_NUM == CONSOLE_UART_INTR_NUM)
#endif //#ifdef CONFIG_AT_CMD_SUPPORT
	{
		if(CONSOLE_UART(is_can_tx)())
		{
			CONSOLE_UART(TxEmptyIrqHandler)(0);
		}
	}
}



PUBLIC FLASH_FUNC void CMD_UART_INTR_En(uint8 en)
{
	//iot_printf("CMD_UART_INTR_En = %d\n",en);
	if(en)
		hal_global_int_en(CMD_UART_INTR_NUM);
	else 
		hal_global_int_dis(CMD_UART_INTR_NUM);
}

FLASH_FUNC PUBLIC void CMD_UART_Init()
{
	CMD_UART(Init)();
}
FLASH_FUNC PUBLIC int CMD_UART_Send(uint8 Data)
{
	return CMD_UART(Send)(Data);
}
PUBLIC FLASH_FUNC int CMD_UART_Recv(uint8 * pData)
{
	return CMD_UART(Recv)(pData);
}
FLASH_FUNC void CMD_UART_change_params(unsigned long rate, unsigned char data_len,  unsigned char stopbits, unsigned char parity)
{
	CMD_UART(change_params)(rate, data_len, stopbits, parity);
}
NORELOC FLASH_FUNC  int CMD_UART_set_baudrate(unsigned long baudrate)
{
	return CMD_UART(set_baudrate)(baudrate);
}
NOCACHE_NOFLASH_FUNC int CMD_UART_GET_INTR_NUM()
{
	return CMD_UART_INTR_NUM;
}

FLASH_FUNC PUBLIC void CONSOLE_UART_Init()
{
	CONSOLE_UART(Init)(); 
}
FLASH_FUNC PUBLIC int CONSOLE_UART_Send(uint8 Data)
{
	return CONSOLE_UART(Send)(Data);
}
PUBLIC FLASH_FUNC int CONSOLE_UART_Recv(uint8 * pData)
{
	return CONSOLE_UART(Recv)(pData);
}

NOCACHE_NOFLASH_FUNC void CONSOLE_UART_change_params(unsigned long rate, unsigned char data_len,  unsigned char stopbits, unsigned char parity)
{
	CONSOLE_UART(change_params)(rate, data_len, stopbits, parity);
}
NORELOC FLASH_FUNC  int CONSOLE_UART_set_baudrate(unsigned long baudrate)
{
	return CONSOLE_UART(set_baudrate)(baudrate);
}

NOCACHE_NOFLASH_FUNC int CONSOLE_UART_GET_INTR_NUM()
{
	return CONSOLE_UART_INTR_NUM;
}


NOCACHE_NOFLASH_FUNC PUBLIC void atbm_uart0_init(void)
{
	//iot_printf("deafult %d \n",CMD_UART_INTR_NUM);
	hal_global_int_dis(CONSOLE_UART_INTR_NUM);
	if(CMD_UART_INTR_NUM != CONSOLE_UART_INTR_NUM){	
#ifdef CONFIG_AT_CMD_SUPPORT
		Command_Init();
		//iot_printf(" CMD_UART_INTR_NUM != CONSOLE_UART_INTR_NUM\n");
		hal_global_int_dis(CMD_UART_INTR_NUM);
	    hal_register_isr(CMD_UART_INTR_NUM,atbm_uart_cmd_hander,NULL);
	    hal_global_int_en(CMD_UART_INTR_NUM);
#endif //#ifdef CONFIG_AT_CMD_SUPPORT
		hal_register_isr(CONSOLE_UART_INTR_NUM,atbm_uart_console_hander,NULL);
		//iot_printf("\n\n# define CONFIG_UART_CMD_DEBUG please used other uart for ATCMD #\n\n");
	}
	else {
		hal_register_isr(CONSOLE_UART_INTR_NUM,atbm_uart_cmd_hander,NULL);
	}
	hal_global_int_en(CONSOLE_UART_INTR_NUM);

	return;
}
