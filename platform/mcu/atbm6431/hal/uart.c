
#include "atbm_hal.h"
#include "hal/soc/soc.h"
#include "src/driver/uart/uart.h"
#include <aos/kernel.h>

//#include "sys/dbgcon.h"
//#include "drv_uart.h"
//#include "osal.h"
extern void UART1_Init();
extern int UART1_Send(uint8 Data);
extern int UART1_Recv(uint8 * pData);
extern int UART1_set_baudrate(unsigned long baudrate);
extern int UART1_is_have_rx(void);
extern int UART1_is_can_tx(void);
extern void UART1_PowerSave_Later();
extern int UART1_get_char(void);


uart_dev_t uart_0 = {
    .port = 2,
    .config = {
        .baud_rate = 115200,
        .data_width = DATA_WIDTH_8BIT,
        .parity = NO_PARITY,
    },
    .priv = NULL,
};
uart_dev_t uart_1 = {
    .port = 1,
    .config = {
        .baud_rate = 115200,
        .data_width = DATA_WIDTH_8BIT,
        .parity = NO_PARITY,
    },
    .priv = NULL,
};
#define UART_TX_BUF_SIZE 64
#define UART_RX_BUF_SIZE 128
typedef struct UART_LOCAL_S
{
   // int                 Polling;
   // volatile uint32     TxGet;
   // uint32              TxPut;
   // uint8               TxBuffer[UART_TX_BUF_SIZE];
    volatile uint32     RxPut;
    uint32              RxGet;
  //  uint8               LastChar;
    uint8               RxBuffer[UART_RX_BUF_SIZE];
}UART_LOCAL;

static UART_LOCAL uart1_fifo;
static aos_sem_t uart_sync_sem;
static bool b_uart_init[3] = {0};
int UART1_ReadRxFifo(char *pChar);
void UART1_IrqHandler(int Priority);



int32_t hal_uart_init(uart_dev_t *uart)
{
	if(uart==NULL){
		uart =  &uart_0;
	}
	printf("hal_uart_init uart->port %d!!",uart->port);
	if(b_uart_init[uart->port])
		return 0;
	if(uart->port==2){
		UART2_set_baudrate(uart->config.baud_rate);
		if(aos_sem_new(&uart_sync_sem, 0)!=0){
			printf("hal_uart_init err!!");
			return -1;
		}
	}
	else {
		printf("hal_uart_init intr %d!!",DEV_INT_NUM_UART);
		hal_global_int_dis(DEV_INT_NUM_UART);
		UART1_Init();
		hal_register_isr(DEV_INT_NUM_UART,UART1_IrqHandler,NULL);
		UART1_set_baudrate(uart->config.baud_rate);
		memset(&uart1_fifo,0,sizeof(uart1_fifo));
		hal_global_int_en(DEV_INT_NUM_UART);
	}
	b_uart_init[uart->port]= 1;
	return 0;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
	aos_sem_free(&uart_sync_sem);
    return 0;
}

/*print used command uart*/
int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    uint8_t* pTmp = (uint8_t*)data;

	if(uart== NULL){
		printf("hal_uart_recv_II uart = NULL\n");
		return -1;
	}
	if(uart->port == 2){
		Command_SetPolling(1);
	    for (int i = 0; i < size; i++) {
	        Command_Putc(*pTmp++);
	    }
	}
	else {
		for (int i = 0; i < size; i++) {
			while(UART1_Send(*pTmp)==FALSE){
				hal_sleep(1);
				timeout--;
				if(timeout == 0)
					return i;
			}
			pTmp++;			
		}
	}
    return size;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *pdata, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
{
    char* pTmp = (char*)pdata;
	char data_c;
    uint32_t size = 0;
	int one_time_size = 0;
	
	if(uart== NULL){
		printf("hal_uart_recv_II uart = NULL\n");
		return -1;
	}
	if(uart->port == 2){
	    while (expect_size > 0)
	    {
	        one_time_size = 0;
			if(Command_ReadRxFifo(&data_c)){
				one_time_size=1;
				expect_size--;
				*pTmp=data_c;
				pTmp++;
				size++;
			}
			
	        if (!timeout)
	        {
	            break;
	        }
	        if ((expect_size > 0)&&(one_time_size==0))
	        {
	            //hal_sleep(1);
				aos_sem_wait(&uart_sync_sem, timeout);
				if(timeout != AOS_WAIT_FOREVER)
	           	 	timeout--;			
	        }
	    }
	}
	else {
	    while (expect_size > 0)
	    {
	        one_time_size = 0;
			if(UART1_ReadRxFifo(&data_c)){
				one_time_size=1;
				expect_size--;
				*pTmp=data_c;
				pTmp++;
				size++;
			}
			
	        if (!timeout)
	        {
	            break;
	        }
	        if ((expect_size > 0)&&(one_time_size==0))
	        {
	            hal_sleep(1);
				//aos_sem_wait(&uart_sync_sem, 1);
				if(timeout != AOS_WAIT_FOREVER)
	           	 	timeout--;			
	        }
	    }

	}
    if (recv_size != NULL) {
        *recv_size = size;
    }
    return 0;
}

int UART1_ReadRxFifo(char *pChar)
{
    unsigned RxGet;

    RxGet = uart1_fifo.RxGet;
    if (RxGet == uart1_fifo.RxPut)
    {
        return FALSE;
    }
    *pChar = (char)uart1_fifo.RxBuffer[RxGet & (UART_RX_BUF_SIZE -1)];
	uart1_fifo.RxBuffer[RxGet & (UART_RX_BUF_SIZE -1)] = (uint8)0;
    RxGet++;
    uart1_fifo.RxGet = RxGet;

    return TRUE;
}
PUBLIC FLASH_FUNC int UART1_RX(uint8 * pData)
{
    if (UART1_is_have_rx())
    {
        *pData = UART1_get_char();
		//if(Uart_TSPT_Echo_flag == 1)
		////{
		//	if ( UART1_is_can_tx()){
		//		UART1_put_char(*pData);	
		//	}	
		//}
		UART1_PowerSave_Later();
        return TRUE;
    }
    return FALSE;
}


NOCACHE_NOFLASH_FUNC void UART1_RxIrq()
{
	unsigned RxPut;
	uint8 Data;

	RxPut = uart1_fifo.RxPut;
	while((s32)(RxPut - uart1_fifo.RxGet) < UART_RX_BUF_SIZE)
	{

		if (!UART1_RX(&Data)) 
		{
			break;
		}
		uart1_fifo.RxBuffer[RxPut & (UART_RX_BUF_SIZE -1)] = (uint8)Data;
		RxPut++;
		uart1_fifo.RxPut = RxPut;
	}
	//drop uart if buffer full,if not drop uart intr will not clear
	if((s32)(RxPut - uart1_fifo.RxGet) == UART_RX_BUF_SIZE){
		while(UART1_RX(&Data));
	}
}

#define IN8(reg)			(  *( (volatile uint8_t *) (reg) ) )
#define OUT8(reg, data)			( (*( (volatile uint8_t *) (reg) ) ) = (uint8_t)(data) )

NOCACHE_NOFLASH_FUNC void UART1_TxIrq()
{

	//uint32	TxGet;
	//clear intr
    uint8 regdata = IN8(UART1_BASE + UARTC_IER_OFFSET);
	OUT8(UART1_BASE + UARTC_IER_OFFSET, regdata& (~UARTC_IER_THRE));
	
}



NOCACHE_NOFLASH_FUNC void UART1_IrqHandler(int Priority)
{
	if(UART1_is_have_rx()){		
		UART1_RxIrq();
	}
	if(UART1_is_can_tx()){
		UART1_TxIrq();
	}
}



void CmdLine_SetEvent()
{
	aos_sem_signal(&uart_sync_sem);	
}

void ATCmd_init()
{
	printf("ATCmd_init\n");
	hal_sleep(10);
}
void ATCmd_loop()
{	
	/*
	u8 uart1_test_buffer[20]={0};
	u32 rx_len=0;
	hal_uart_init(&uart_1);
	printf("ATCmd_loop++\n");
	while(1){
		hal_uart_recv_II(&uart_1,uart1_test_buffer,16,&rx_len,1000);	
		hal_uart_send(&uart_1,uart1_test_buffer,rx_len,1000);	
		memset(uart1_test_buffer,0,16);
	}*/
	hal_sleep(10);
	printf("ATCmd_loop--\n");
    hal_destroy_thread(NULL);
}
void AT_socketmode_end()
{
}

