/*
 * spi.c
 *
 *  Created on: 2018-11-23
 *      Author: NANXIAOFENG
 */
#include "hal.h"
#include "os_api.h"
#include "spi.h"
#include "gpio.h"
#include "flash.h"

PRIVATE atcspi200_reg_p	 spi_dev;

#if (CHIP_TYPE == ARES_B)
static u32 SPI_CLOCK_SOURCE = 10000000;
#else
static u32 SPI_CLOCK_SOURCE = 40000000;
#endif
uint8 SPI = 1;
/* spi_no is only spi2/spi3
 * 3 线全双工同步传输
 * 0 ~ 31位传输帧格式选择
 * 可触发中断发送和接收
 * 可配置DMA
 * 可编程时钟极性和相位
 * 可编程数据传输顺序（MSB,LSB）
 *
 * SPI2 的默认时钟 80MHz
 * SPI3 的默认时钟 10MHz
 */

void HAL_SPI_Init(enum SPI_NO spi_no,SPI_InitTpyeDef *SPI_InitStruct)
{

      if(spi_no == spi2)
      {
	    	iot_printf("spi2\n");
	    	HW_WRITE_REG_BIT(0x17400000,8,8,1);
	    	spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_2;
	  	    gpio_set_pin_func(20,SPI2_FUNC);   //---CLK
	  	    gpio_set_pin_func(21,SPI2_FUNC);   //---MISO
	  	    gpio_set_pin_func(22,SPI2_FUNC);   //---MOSI
	  	    if(SPI_InitStruct->SPI_CS_Config == hardware)
	  	    {
	  	    	iot_printf("SPI_CS_Config == hardware\n");
	  	    	gpio_set_pin_func(23,SPI2_FUNC);   //---CS
	  	    }
	  	    //gpio_set_pin_func(23,SPI2_FUNC);   //---CS
      }
      else if(spi_no == spi3)
      {
    	  iot_printf("spi3\n");
    	  spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_3;
    	  //step1 IO mask    	  		  
		  // 0x17400000[0] 	  SPI3 location_sel
    	  HW_WRITE_REG_BIT(0x17400000,0,0,1);
    	  gpio_set_pin_func(0,SPI3_FUNC);  //---CLK
    	  gpio_set_pin_func(1,SPI3_FUNC);  //---MISO
    	  gpio_set_pin_func(2,SPI3_FUNC);  //---MOSI
    	  if(SPI_InitStruct->SPI_CS_Config == hardware)
		  {
    		  iot_printf("SPI_CS_Config == hardware\n");
    		  gpio_set_pin_func(3,SPI3_FUNC);  //---CS
		  }
      }
      else
      {
    	  return;
      }

      if(SPI_InitStruct->SPI_ADDR_EN)
      {
    	  HAL_SPI_Addr_Enable(spi_no,SPI_InitStruct->SPI_ADDR_LEN);
      }

      //SPI Transfer Format Register (0x10)

      spi_dev->IfSet =
    		ATCSPI200_PREPARE(IFSET, DATA_MERGE,0) |   //If DataLen is 0x7 each unit is a byte
      		ATCSPI200_PREPARE(IFSET, DATA_LEN,7)   |
      		ATCSPI200_PREPARE(IFSET, 3LINE, 	SPI_InitStruct->SPI_Dir) |
      		ATCSPI200_PREPARE(IFSET, LSB,		SPI_InitStruct->SPI_FirstBit) |
      		ATCSPI200_PREPARE(IFSET, MSSL,		SPI_InitStruct->SPI_MODE) |
      		ATCSPI200_PREPARE(IFSET, CPOL,		SPI_InitStruct->SPI_CPOL) |
      		ATCSPI200_PREPARE(IFSET, CPHA,		SPI_InitStruct->SPI_CPHA);

      if(SPI_InitStruct->SPI_CMD_EN)
	  {
    	  HAL_SPI_Cmd_Enable(spi_no,SPI_InitStruct->SPI_CMD_EN);
	  }
      /*write only*/
	  spi_dev->RegDCtrl  =
		 ATCSPI200_PREPARE(REGDCTRL, TRAMODE,   0x01);

     if(SPI_InitStruct->SPI_DMA == 1)
     {
    	  spi_dev->Ctrl =
			ATCSPI200_PREPARE(CTRL, TXFTH,		1) |
			ATCSPI200_PREPARE(CTRL, RXFTH,		1) |
			ATCSPI200_PREPARE(CTRL, TXDMAEN,	1) |
			ATCSPI200_PREPARE(CTRL, RXDMAEN,	1) |
			ATCSPI200_PREPARE(CTRL, RXFRST, 	1) |
			ATCSPI200_PREPARE(CTRL, TXFRST, 	1) ;
     }
     else
     {
    	 iot_printf("DMA Unenable\n");
    	 spi_dev->Ctrl =
			 ATCSPI200_PREPARE(CTRL, TXFTH, 	 2) |		 //Transmit (TX) FIFO Threshold
			 ATCSPI200_PREPARE(CTRL, RXFTH, 	 1) |		 //Receive (RX) FIFO Threshold (must be 1!!!)
			 ATCSPI200_PREPARE(CTRL, RXFRST,	 1) |
			 ATCSPI200_PREPARE(CTRL, TXFRST,	 1) ;
     }

    HAL_SPI_Clock_Set(spi_no,SPI_InitStruct->SPI_Prescaler);
    //iot_printf("0x20 = 0x%x\n",HW_READ_REG(SPI_BASE_ADDRESS_3+0x20));
	iot_printf("spi_init success!\n");
 }

void HAL_SPI_Reset(enum SPI_NO spi_no)
{
	if(spi_no == spi2)
	{
		iot_printf("spi2\n");
		spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_2;


	}
	else if(spi_no == spi3)
	{
		iot_printf("spi3\n");
		spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_3;

	}
	else
	{
	  return;
	}

	spi_dev->Ctrl = ATCSPI200_PREPARE(CTRL, SPIRST, 1);

}

/*
 *   Enable Address
 *   Master mode only
 *   AddrLen 1,2,3 byte
 */
int HAL_SPI_Addr_Enable(enum SPI_NO spi_no,u32 addr_len)
{
	if(spi_no == spi2)
	{
		iot_printf("spi2\n");
		spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_2;


	}
	else if(spi_no == spi3)
	{
		iot_printf("spi3\n");
		spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_3;

	}
	else
	{
	  return 0;
	}
	/*Master mode only*/
	if(ATCSPI200_GET_FIELD( spi_dev->IfSet,IFSET, MSSL)!= Master)
	{
		DBG_Printf("Error Slave mode\n");
		return 0;
	}
	 /* Enable Addr */
	 ATCSPI200_SET_FIELD(spi_dev->RegDCtrl,REGDCTRL, ADDREN, 0x1);
	 /*AddrLen */
	 if(addr_len == 1)
	 {
		 ATCSPI200_SET_FIELD(spi_dev->RegDCtrl,IFSET, ADDR_LEN,0x0);
	 }
	 else if(addr_len == 2)
	 {
		 ATCSPI200_SET_FIELD(spi_dev->RegDCtrl,IFSET, ADDR_LEN,0x1);
	 }

	 else if(addr_len == 3)
	 {
		 ATCSPI200_SET_FIELD(spi_dev->RegDCtrl,IFSET, ADDR_LEN,0x2);
	 }
	 else
	 {
		 return 0;
	 }
	 return 1;
}

/*
 *   Enable/Disable Cmd
 */
int HAL_SPI_Cmd_Enable(enum SPI_NO spi_no,BOOL able)
{
	if(spi_no == spi2)
	{
		iot_printf("spi2\n");
		spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_2;


	}
	else if(spi_no == spi3)
	{
		iot_printf("spi3\n");
		spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_3;

	}
	else
	{
	  return 0;
	}
	/*Master mode only*/
	if(ATCSPI200_GET_FIELD( spi_dev->IfSet,IFSET, MSSL)!= Master)
	{
		DBG_Printf("Error Slave mode\n");
		return 0;
	}
	/* Enable CMDEN */
	if(able == 1)
	{
		 ATCSPI200_SET_FIELD(spi_dev->RegDCtrl,REGDCTRL, CMDEN, 0x1);
	}
	else
	{
		ATCSPI200_SET_FIELD(spi_dev->RegDCtrl,REGDCTRL, CMDEN, 0x0);
	}

	return 1;
}

/*
 *  CS2SCLK Set
 *  cs_clk 0 ~ ox3
 *  SPI CS???SCLK??????????0.5 SPI SCLK,??????CS2SCLK???0.5~2 SPI SCLK
 *  (SCLK period/2) * (CS2SCLK + 1)
 */
int HAL_SPI_CS2SCLK_Set(enum SPI_NO spi_no,u8 cs_clk)
{
	if(spi_no == spi2)
	{
		iot_printf("spi2\n");
		spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_2;


	}
	else if(spi_no == spi3)
	{
		iot_printf("spi3\n");
		spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_3;

	}
	else
	{
	  return 0;
	}
	ATCSPI200_SET_FIELD(spi_dev->RegTiming,REGTIMING, CS_CLK,cs_clk);
	 return 1;
}

/*
 *  CSHT Set
 *  csht 0~0xf
 *  SPI CS?????????????1.5?SPI SCLK,??????CSHT???0.5~8 SPI SCLK
 *  (SCLK period/2) * (CSHT + 1)
 */
int HAL_SPI_CSHT_Set(enum SPI_NO spi_no,u8 csht)
{
	if(spi_no == spi2)
	{
		iot_printf("spi2\n");
		spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_2;


	}
	else if(spi_no == spi3)
	{
		iot_printf("spi3\n");
		spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_3;

	}
	else
	{
	  return 0;
	}
	ATCSPI200_SET_FIELD(spi_dev->RegTiming,REGTIMING, CSHT, csht);
	 return 1;
}

/*
 * SPI set TransMode
 */
int HAL_SPI_TransMode_Set(enum SPI_NO spi_no,enum TransMode mode)
{
	if(spi_no == spi2)
	{
		iot_printf("spi2\n");
		spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_2;


	}
	else if(spi_no == spi3)
	{
		iot_printf("spi3\n");
		spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_3;

	}
	else
	{
	  return 0;
	}
	ATCSPI200_SET_FIELD(spi_dev->RegDCtrl,REGDCTRL, TRAMODE, mode);
	 return 1;
}

NO_FLASH_FUNC int HAL_SPI_Clock_Set(enum SPI_NO spi_no,UINT32 timing)
{
	UINT32 SCLK_DIV;

	if(spi_no == spi2)
	{
		iot_printf("spi2\n");
		spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_2;


	}
	else if(spi_no == spi3)
	{
		iot_printf("spi3\n");
		spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_3;

	}
	else
	{
	  return 0;
	}

	//iot_printf("timing = %d\n",timing);
	if(timing == CPE_SPI_EXT_CLK)
		SCLK_DIV = 0xff;
	else if(timing == 0)
		return 0;
	else
		SCLK_DIV = (SPI_CLOCK_SOURCE/timing)/2 - 1;

	ATCSPI200_SET_FIELD(spi_dev->RegTiming,REGTIMING, SCLK_DIV, SCLK_DIV);
	return 1;
}

/*
 * Set SPI Clock Source
 * only spi3
 * 16100010[21:18]
 * 	CLOCK_10M = 0,
	CLOCK_20M = 1,
	CLOCK_40M = 4,
	CLOCK_80M = 6
 */
u32 HAL_SPI_Set_SCLK_Period(enum SPI_NO spi_no,enum SPI_CLOCK_SOURCE SCLK_TYPE)
{
	if(spi_no == spi2)
	{
        return 0;
	}
	else if(spi_no == spi3)
	{
		HW_WRITE_REG_BIT(0x16100010,21,18,SCLK_TYPE);
	    if(SCLK_TYPE == CLOCK_10M)
	    {
	    	SPI_CLOCK_SOURCE = 10000000;
	    }
	    else if(SCLK_TYPE == CLOCK_20M)
	    {
	    	SPI_CLOCK_SOURCE = 20000000;
	    }
	    else if(SCLK_TYPE == CLOCK_40M)
	    {
	    	SPI_CLOCK_SOURCE = 40000000;
	    }
	    else
	    {
	    	SPI_CLOCK_SOURCE = 80000000;
	    }
		return SPI_CLOCK_SOURCE;
	}
	else
	{
		return 0;
	}
}

u32 HAL_SPI_GET_Status(enum SPI_NO spi_no,enum SPI_Status status)
{
	if(spi_no == spi2)
	{
	   spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_2;
	}
	else if(spi_no == spi3)
	{
	  spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_3;
	}
	else
	{
	  return -1;
	}
    if(status == Tx_Status)
    {
        return (spi_dev->St & (0x1<<23));
    }
    else if(status == Rx_Status)
    {
    	return (spi_dev->St & (0x1<<15));
    }
    else if(status == SPIActive)
    {
    	return (spi_dev->St & (0x1));
    }
    else
    {
    	return -1;
    }
}

void HAL_SPI_Write(enum SPI_NO spi_no,uint8 *pdata,int len)
{
	   //iot_printf("%s %d \n",__func__,__LINE__);
       uint8 * t_ptr_start = pdata;
       uint8 *t_ptr_end;
       u32 data = 0;

       int i=0,tmp=0,j;
       if(spi_no == spi2)
       {
          spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_2;
       }
       else if(spi_no == spi3)
       {
         spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_3;
       }
       else
       {
         return;
       }

       while (ATCSPI200_GET_FIELD(spi_dev->St, ST, SPIBSY));

       ATCSPI200_SET_FIELD(spi_dev->IfSet,IFSET, DATA_LEN,7);
       ATCSPI200_SET_FIELD(spi_dev->IfSet,IFSET, DATA_MERGE,1);

       while(len/512)
       {
           ATCSPI200_SET_FIELD(spi_dev->RegDCtrl,REGDCTRL, WCNT, 511);
    	   //spi_dev->RegDCtrl  = ATCSPI200_PREPARE(REGDCTRL, TRAMODE,   0x01) | ATCSPI200_PREPARE(REGDCTRL, WCNT, 511);
    	   t_ptr_end = t_ptr_start+512;
    	   spi_dev->RegCPort = 7;
    	   while(t_ptr_end - t_ptr_start)
		   {
				 //iot_printf("%s %d \n",__func__,__LINE__);
				 tmp = CPE_SPI_TX_FIFO_SIZE- ATCSPI200_GET_FIELD(spi_dev->St, ST, TXFVE);
				 for (i = 0; i < tmp; i++)
				 {
					//CPE_SPI_TX_FIFO_SIZE - Number of valid entries in the Transmit FIFO
				    //iot_printf("SPI_TX: [0x%x]\n",*t_ptr_start);
					 for (j=0; j < 4; j++)
					 {
						 data |= ((*t_ptr_start++)<<(8*j));
					 }
					 spi_dev->RegDPort = data;
					 //iot_printf("data = 0x%x\n",data);
					 data = 0;
				 }

		   }
    	   while (ATCSPI200_GET_FIELD(spi_dev->St, ST, SPIBSY));
    	   //iot_printf("%s %d \n",__func__,__LINE__);
    	   len = len-512;
    	   //iot_printf(" len %d +++++++\n",len);
       }
       //iot_printf("%s %d \n",__func__,__LINE__);
       if(len != 0)
       {
    	    //iot_printf("%s %d \n",__func__,__LINE__);
    	    ATCSPI200_SET_FIELD(spi_dev->RegDCtrl,REGDCTRL, WCNT, len-1);
			//spi_dev->RegDCtrl  = ATCSPI200_PREPARE(REGDCTRL, TRAMODE,   0x01) | ATCSPI200_PREPARE(REGDCTRL, WCNT, len-1);
			t_ptr_end = t_ptr_start+len;
			spi_dev->RegCPort = 7;
			while(t_ptr_end - t_ptr_start)
			{
				 while(len/4)
				 {
					 tmp = CPE_SPI_TX_FIFO_SIZE- ATCSPI200_GET_FIELD(spi_dev->St, ST, TXFVE);
					 for (i = 0; i < tmp; i++)
					 {
						//CPE_SPI_TX_FIFO_SIZE - Number of valid entries in the Transmit FIFO
						//iot_printf("SPI_TX: [0x%x]\n",*t_ptr_start);
						 for (j=0; j < 4; j++)
						 {
							 data |= ((*t_ptr_start++)<<(8*j));
							 len = len-1;
						 }
						 spi_dev->RegDPort = data;
						 //iot_printf("data = 0x%x\n",data);
						 data = 0;
					 }
				 }
				 if(len != 0)
				 {
						tmp = CPE_SPI_TX_FIFO_SIZE- ATCSPI200_GET_FIELD(spi_dev->St, ST, TXFVE);
						if(tmp != 0)
						{
							//CPE_SPI_TX_FIFO_SIZE - Number of valid entries in the Transmit FIFO
							//iot_printf("SPI_TX: [0x%x]\n",*t_ptr_start);
							 for (j=0; j < len; j++)
							 {
								 data |= ((*t_ptr_start++)<<(8*j));
							 }
							 spi_dev->RegDPort = data;
							 //iot_printf("data = 0x%x\n",data);
							 data = 0;
						}
				 }
			}
			while (ATCSPI200_GET_FIELD(spi_dev->St, ST, SPIBSY));
       }
}

void HAL_SPI_Read(enum SPI_NO spi_no,uint8 *pdata,int len)
{
       if(spi_no == spi2)
       {
          spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_2;
       }
       else if(spi_no == spi3)
       {
         spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_3;
       }
       else
       {
         return;
       }

       while (ATCSPI200_GET_FIELD(spi_dev->St, ST, SPIBSY));

       ATCSPI200_SET_FIELD(spi_dev->IfSet,IFSET, DATA_LEN, 7);
       ATCSPI200_SET_FIELD(spi_dev->RegDCtrl,REGDCTRL, RCNT, len-1);
       //spi_dev->RegDCtrl =  ATCSPI200_PREPARE(REGDCTRL, TRAMODE,   0x02) | ATCSPI200_PREPARE(REGDCTRL, RCNT, len-1);

       spi_dev->RegCPort = 7;

		while(len)
		{
			if ((spi_dev->St & ATCSPI200_ST_RXFEM_MASK)==0)
			{
				*(pdata) = spi_dev->RegDPort;
				pdata++;
				len--;
			}
		}
        while (ATCSPI200_GET_FIELD(spi_dev->St, ST, SPIBSY));
}


u32 HAL_SPI_Read_Data(enum SPI_NO spi_no)
{
	u32 TmpData;
	uint32 RxEmpty = 1;
	if(spi_no == spi2)
	{
	   spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_2;
	}
	else if(spi_no == spi3)
	{
	  spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_3;
	}
	else
	{
	  return -1;
	}

	while(1)
	{
		RxEmpty = ATCSPI200_GET_FIELD(spi_dev->St, ST, RXFEM);
		if(!RxEmpty)
		{
			DBG_Printf("%s,%d. Tx fifo is not empty!\n",__func__,__LINE__);
			continue;
		}
		break;
	}

	TmpData = spi_dev->RegDPort;
	lmac_Wait(200*1000);
	spi_dev->RegCPort = 7;
	return TmpData;
}

void HAL_SPI_Write_Data(enum SPI_NO spi_no,uint32 data)
{
	uint32 TmpData = data;
	uint32 TxEmpty = 1;

	if(spi_no == spi2)
	{
	   spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_2;
	}
	else if(spi_no == spi3)
	{
	   spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_3;
	}
	else
	{
	   return;
	}

	int i=0,n=0;

	//MSB need del high n bit 0
	for (i = 31; i >=0; i--)
	{
		if(TmpData & (0x1<<i)){
			n = i;
			break;
		}
	}

	//iot_printf("%s 0x20 = 0x%x----\n",__func__,HW_READ_REG(SPI_BASE_ADDRESS_3+0x20));
	ATCSPI200_SET_FIELD(spi_dev->IfSet,IFSET, DATA_LEN, n);
	//spi_dev->IfSet = ATCSPI200_PREPARE(IFSET, DATA_LEN,	n);
	//spi_dev->RegDCtrl  = ATCSPI200_PREPARE(REGDCTRL, TRAMODE,   0x01);

	while(1)
	{
		TxEmpty = ATCSPI200_GET_FIELD(spi_dev->St, ST, TXFEM);
		if(!TxEmpty)
		{
			DBG_Printf("%s,%d. Tx fifo is not empty!\n",__func__,__LINE__);
			continue;
		}
		break;
	}

	//DBG_Printf("spi_write:0x%x,len %d\n",TmpData,n);
	spi_dev->RegDPort = TmpData;

	spi_dev->RegCPort = 7;

	while (ATCSPI200_GET_FIELD(spi_dev->St, ST, SPIBSY));
}

#define HT1621 0
#if(HT1621)

//the dot postion of lcd
#define HT1621_DOT_COLON		0
#define HT1621_DOT_LEFT			2
#define HT1621_DOT_MID			4
#define HT1621_DOT_RIGHT		6

//the address of the num postion
#define HT1621_BYTE_ADDR1		0
#define HT1621_BYTE_ADDR2		2
#define HT1621_BYTE_ADDR3		4
#define HT1621_BYTE_ADDR4		6

#define HT_CMD_ID_READ			0x06		//0b110
#define HT_CMD_ID_WRITE			0x05		//0b101
#define HT_CMD_ID_RD_MODIFY_WR	0x05		//0b101
#define HT_CMD_ID_CMD			0x04		//0b100

#define BIAS 	0x50  //0b1000 0101 0010  1/2duty 4com
#define SYSDIS 	0x00  //0b1000 0000 0000  ?…3??ˉ?3????è????¨?’?LCD????????‘?”????
#define LCDOFF	0x04  //0b1000 0000 0100  ?…3LCD??????
#define LCDON 	0x06  //0b1000 0000 0110  ?‰“??�LCD??????
#define SYSEN 	0x02  //0b1000 0000 0010  ?‰“??�?3??????ˉè?????
#define XTAL 	0x28  //0b1000 0010 1000  ?¤–é?¨??￥?—?é’?
#define RC256 	0x30  //0b1000 0011 0000  ??…é?¨?—?é’?
#define TONEON 	0x12  //0b1000 0001 0010  ?‰“??�?￡°é?3è?“??o
#define TONEOFF 0x10  //0b1000 0001 0000  ?…3é—-?￡°é?3è?“??o
#define TONE4K	0x80  //0b1000 1000 0000  ?￡°é?3é￠‘???4kHZ
#define TONE2K	0xc0  //0b1000 1100 0000  ?￡°é?3é￠‘???2KHZ
#define WDTDIS 	0x0A  //0b1000 0000 1010  ?|??-￠???é—¨???

const unsigned char num[10]={0xbe,0xa0,0xda,0xf8,0xe4,0x7c,0x7e,0xa8,0xfe,0xfc};

void HT1621_spi_init(void)
{

	  //step1 IO mask
	   spi_dev = (atcspi200_reg_p) SPI_BASE_ADDRESS_3;
	    //SPI3 channel 1
	    HW_WRITE_REG_BIT(0x17400000,0,0,1);  //????2?í?μ?D?μà                      //D?μà????
		HW_WRITE_REG_BIT(0x17400004,3,0,6);
		HW_WRITE_REG_BIT(0x17400008,19,16,6);
		HW_WRITE_REG_BIT(0x17400008,3,0,6);

		//SPI3 channel 2
		/*
		HW_WRITE_REG_BIT(0x17400030,3,0,6);     //MOSI
		HW_WRITE_REG_BIT(0x17400030,19,16,6);   //CS
		HW_WRITE_REG_BIT(0x1740002c,3,0,6);     //CLK
		*/

	    /*
		HW_WRITE_REG_BIT(0x1740002c,3,0,2);        //CLK
		HW_WRITE_REG_BIT(0x17400030,3,0,2);        //
		HW_WRITE_REG_BIT(0x17400030,19,16,2);      //channel 3  gpio18
        */

//SPI Transfer Format Register (0x10)
	 spi_dev->IfSet =
		 ATCSPI200_PREPARE(IFSET, DATA_MERGE,0) |		 //If DataLen is 0x7 each unit is a byte
		 ATCSPI200_PREPARE(IFSET, 3LINE,	 0) |
		 ATCSPI200_PREPARE(IFSET, LSB,		 0) |
		 ATCSPI200_PREPARE(IFSET, MSSL, 	 0) |
		 ATCSPI200_PREPARE(IFSET, CPOL, 	 1) |
		 ATCSPI200_PREPARE(IFSET, CPHA, 	 1);

	 spi_dev->RegTiming =
		 ATCSPI200_PREPARE(REGTIMING, SCLK_DIV, 0x63);
		 //ATCSPI200_PREPARE(REGTIMING, SCLK_DIV, 0xff);

	 spi_dev->RegDCtrl  =
		 ATCSPI200_PREPARE(REGDCTRL, TRAMODE,   0x01);

		 //SPI Control Register (0x30)

	 spi_dev->Ctrl =
		 ATCSPI200_PREPARE(CTRL, TXFTH, 	 2) |		 //Transmit (TX) FIFO Threshold
		 ATCSPI200_PREPARE(CTRL, RXFTH, 	 1) |		 //Receive (RX) FIFO Threshold (must be 1!!!)
		 ATCSPI200_PREPARE(CTRL, RXFRST,	 1) |
		 ATCSPI200_PREPARE(CTRL, TXFRST,	 1) ;


	 DBG_Printf(" spi_init success!\n");
}

void HT1621_spi_write(uint32 data)
{
	uint32 TmpData = data;
	uint32 TxEmpty = 1;

	int i=0,n=0;

	//MSB need del high n bit 0
	for (i = 31; i >=0; i--)
	{
		if(TmpData & (0x1<<i)){
			n = i;
			break;
		}
	}

	spi_dev->IfSet = ATCSPI200_PREPARE(IFSET, DATA_LEN,	n);

	while(1)
	{
		TxEmpty = ATCSPI200_GET_FIELD(spi_dev->St, ST, TXFEM);
		if(!TxEmpty)
		{
			DBG_Printf("%s,%d. Tx fifo is not empty!\n",__func__,__LINE__);
			continue;
		}
		break;
	}

	DBG_Printf("spi_write:0x%x, %d bit\n",TmpData,n);
	spi_dev->RegDPort = TmpData;
	//spi_dev->RegDPort = 0x80600000;
	lmac_Wait(200*1000);
	spi_dev->RegCPort = 7;

}

void HT1621WrCmd(uint8 ucTmpCmd)
{
	uint8 cmdId = HT_CMD_ID_CMD;
	uint32 cmdstr = ucTmpCmd|(cmdId<<9);
	//iot_printf("cmd 0x%x \n",cmdId<<1);
    if(SPI == 1)
    {
       	HAL_SPI_Write_Data(spi3,cmdstr);
       	sleep(1);

    }
    else
    {
    	HT1621_spi_write(cmdstr);
    }
}

void HT1621WrByte(uint8 addr,uint8 ucTmpData)
{
	uint8 cmdId = HT_CMD_ID_WRITE;
	uint32 cmdstr;
	//uint8 cmd = 0xa0;
	//uint32 cmdstr = ((ucTmpCmd|(cmdId<<8))<<8)|ucTmpData;
	DBG_Printf("HT1621WrByte, addr:0x%x,data:0x%x\n",addr,ucTmpData);
	cmdstr = (cmdId<<10)|((addr&0x3f)<<4)|(ucTmpData&0x0f);
    if(SPI == 1)
    {
    	HAL_SPI_Write_Data(spi3,cmdstr);
      	sleep(1);
    }
    else
    {
    	HT1621_spi_write(cmdstr);
    }

	cmdstr = (cmdId<<10)|(((addr+1)&0x3f)<<4)|((ucTmpData>>4)&0x0f);
    if(SPI == 1)
    {
    	HAL_SPI_Write_Data(spi3,cmdstr);
      	sleep(1);
    }
    else
    {
    	HT1621_spi_write(cmdstr);
    }
	DBG_Printf("HT1621WrByte end\n");

}

void HT1621Show(uint8 add,uint8 num )
{
	//HT1621WrCmd(add);
	//HT1621WrByte(num);
}

void HT1621_Inital(void)
{
	// HT1621_spi_init();

 	 HT1621WrCmd(BIAS);
	 HT1621WrCmd(RC256);
 	 HT1621WrCmd(SYSDIS);
 	 HT1621WrCmd(WDTDIS);
 	 HT1621WrCmd(SYSEN);
	 HT1621WrCmd(LCDON);

	 iot_printf("HT1621_Inital end\n");
}

void HT1621_Sound(uint8 hzCmd,uint8 time)
{

	HT1621WrCmd(TONEON);
	HT1621WrCmd(hzCmd);
	lmac_Wait(time);
	HT1621WrCmd(TONEOFF);

}

#define HT1621_DOT_COLON		0
#define HT1621_DOT_LEFT			2
#define HT1621_DOT_MID			4
#define HT1621_DOT_RIGHT		6

#define HT1621_BYTE_ADDR1		0
#define HT1621_BYTE_ADDR2		2
#define HT1621_BYTE_ADDR3		4
#define HT1621_BYTE_ADDR4		6

void HT1621WrDotDisplay(uint8 DotN, uint8 bEnable)
{
	HT1621WrByte(DotN, bEnable);
}

void HT1621WrByteDisplay(uint8 addr,uint8 disNum)
{
	uint8 cmdId = HT_CMD_ID_WRITE;
	uint32 cmdstr;
    //iot_printf("HT1621WrByteDisplay\n");
	uint8 ucTmpData = num[disNum%10];
	DBG_Printf("HT1621WrByte, addr:0x%x, num:%d, data:0x%x\n",addr,disNum,ucTmpData);
	cmdstr = (cmdId<<10)|((addr&0x3f)<<4)|(ucTmpData&0x0f);
    if(SPI == 1)
    {
       	HAL_SPI_Write_Data(spi3,cmdstr);
      	sleep(1);
    }
    else
    {
    	HT1621_spi_write(cmdstr);
    }

	cmdstr = (cmdId<<10)|(((addr+1)&0x3f)<<4)|((ucTmpData>>4)&0x0f);
    if(SPI == 1)
    {
    	HAL_SPI_Write_Data(spi3,cmdstr);
      	sleep(1);
    }
    else
    {
    	HT1621_spi_write(cmdstr);
    }
	DBG_Printf("HT1621WrByte end\n");

}

void HT1621Wr4ByteDisplay(uint32 disNum)
{
	uint8 byte1 = (disNum%10);
	uint8 byte2 = ((disNum%100)/10);
	uint8 byte3 = ((disNum%1000)/100);
	uint8 byte4 = (disNum/1000);


	HT1621WrByteDisplay(0, byte4);
	HT1621WrByteDisplay(2, byte3);
	HT1621WrByteDisplay(4, byte2);
	HT1621WrByteDisplay(6, byte1);

}

void HT1621_Cmd(void)
{
	 iot_printf("HT1621 Send Cmd :\n");
	 HT1621WrCmd(BIAS);
	 HT1621WrCmd(RC256);
	 HT1621WrCmd(SYSDIS);
	 HT1621WrCmd(WDTDIS);
	 HT1621WrCmd(SYSEN);
	 HT1621WrCmd(LCDON);
	 iot_printf("HT1621 Send Cmd Send\n");
}

#if 0
void HT1621_Test(void)
{
	iot_printf("HT1621_Test~~~~\n");
    int i= 0;
    u32 clock = 200000;
	SPI_InitTpyeDef SPI_InitStruct;

	SPI_InitStruct.SPI_MODE = Master;
	SPI_InitStruct.SPI_ADDR_EN = 1;
	SPI_InitStruct.SPI_ADDR_LEN = 0;
	SPI_InitStruct.SPI_CMD_EN = 1;
	SPI_InitStruct.SPI_DataSize = 0x07;
	SPI_InitStruct.SPI_DMA = 0;
	SPI_InitStruct.SPI_CS_Config = sofeware;
	SPI_InitStruct.SPI_Prescaler = clock;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	iot_printf("SPI_CPHA = %d,SPI_CPOL = %d,SPI_FirstBit = %d mode = %d\n",SPI_InitStruct.SPI_CPHA,SPI_InitStruct.SPI_CPOL,SPI_InitStruct.SPI_FirstBit,SPI_InitStruct.SPI_MODE);

	//3?ê??ˉSPI3 2?ê1?üDMA
	if(SPI == 1)
	{
		//HT1621_spi_init();
		HAL_SPI_Init(spi3,&SPI_InitStruct);
		sleep(200);
	    HT1621_Cmd();
	}
	else
    {
		HT1621_spi_init();
		//HAL_SPI_Init(spi3,&SPI_InitStruct);
    	HT1621_Inital();
    }

	for(i=1; i<=0x20; i++)
	{
		HT1621WrByteDisplay(HT1621_BYTE_ADDR1 ,i);
		HT1621WrByteDisplay(HT1621_BYTE_ADDR2 ,i);
		HT1621WrByteDisplay(HT1621_BYTE_ADDR3 ,i);
		HT1621WrByteDisplay(HT1621_BYTE_ADDR4 ,i);

		HT1621WrDotDisplay(HT1621_DOT_COLON,1);
		HT1621WrDotDisplay(HT1621_DOT_LEFT,1);
		HT1621WrDotDisplay(HT1621_DOT_MID,1);
		HT1621WrDotDisplay(HT1621_DOT_RIGHT,1);

		lmac_Wait(1000*1000);

		HT1621WrDotDisplay(HT1621_DOT_COLON,0);
		HT1621WrDotDisplay(HT1621_DOT_LEFT,0);
		HT1621WrDotDisplay(HT1621_DOT_MID,0);
		HT1621WrDotDisplay(HT1621_DOT_RIGHT,0);

	}


	for(i=1000; i<=2000; i+=4)
	{
		HT1621Wr4ByteDisplay(i);
		HT1621WrDotDisplay(HT1621_DOT_COLON,0);
		HT1621WrDotDisplay(HT1621_DOT_LEFT,0);
		HT1621WrDotDisplay(HT1621_DOT_MID,0);
		HT1621WrDotDisplay(HT1621_DOT_RIGHT,0);

		lmac_Wait(1000*1000);

		HT1621WrDotDisplay(HT1621_DOT_COLON,1);
		HT1621WrDotDisplay(HT1621_DOT_LEFT,1);
		HT1621WrDotDisplay(HT1621_DOT_MID,1);
		HT1621WrDotDisplay(HT1621_DOT_RIGHT,1);

	}
}
#endif

#if 1
void HT1621_Test(void)
{
	iot_printf("HT1621_Test~~~~\n");
    int i= 0;
    u32 clock = 200000;
	SPI_InitTpyeDef SPI_InitStruct;

	SPI_InitStruct.SPI_MODE = Master;
	SPI_InitStruct.SPI_DataSize = 0x07;
	SPI_InitStruct.SPI_DMA = 0;
	SPI_InitStruct.SPI_CS_Config = hardware;
	SPI_InitStruct.SPI_Prescaler = clock;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	iot_printf("SPI_CPHA = %d,SPI_CPOL = %d,SPI_FirstBit = %d mode = %d\n",SPI_InitStruct.SPI_CPHA,SPI_InitStruct.SPI_CPOL,SPI_InitStruct.SPI_FirstBit,SPI_InitStruct.SPI_MODE);

	//3?ê??ˉSPI3 2?ê1?üDMA
	if(SPI == 1)
	{
		//HT1621_spi_init();
		HAL_SPI_Init(spi3,&SPI_InitStruct);
		sleep(200);
	    HT1621_Cmd();
	}
	else
    {
		HT1621_spi_init();
		//HAL_SPI_Init(spi3,&SPI_InitStruct);
    	HT1621_Inital();
    }

	for(i=1; i<=0x20; i++)
	{
		HT1621WrByteDisplay(HT1621_BYTE_ADDR1 ,i);
		HT1621WrByteDisplay(HT1621_BYTE_ADDR2 ,i);
		HT1621WrByteDisplay(HT1621_BYTE_ADDR3 ,i);
		HT1621WrByteDisplay(HT1621_BYTE_ADDR4 ,i);

		HT1621WrDotDisplay(HT1621_DOT_COLON,1);
		HT1621WrDotDisplay(HT1621_DOT_LEFT,1);
		HT1621WrDotDisplay(HT1621_DOT_MID,1);
		HT1621WrDotDisplay(HT1621_DOT_RIGHT,1);

		lmac_Wait(1000*1000);

		HT1621WrDotDisplay(HT1621_DOT_COLON,0);
		HT1621WrDotDisplay(HT1621_DOT_LEFT,0);
		HT1621WrDotDisplay(HT1621_DOT_MID,0);
		HT1621WrDotDisplay(HT1621_DOT_RIGHT,0);

	}


	for(i=1000; i<=2000; i+=4)
	{
		HT1621Wr4ByteDisplay(i);
		HT1621WrDotDisplay(HT1621_DOT_COLON,0);
		HT1621WrDotDisplay(HT1621_DOT_LEFT,0);
		HT1621WrDotDisplay(HT1621_DOT_MID,0);
		HT1621WrDotDisplay(HT1621_DOT_RIGHT,0);

		lmac_Wait(1000*1000);

		HT1621WrDotDisplay(HT1621_DOT_COLON,1);
		HT1621WrDotDisplay(HT1621_DOT_LEFT,1);
		HT1621WrDotDisplay(HT1621_DOT_MID,1);
		HT1621WrDotDisplay(HT1621_DOT_RIGHT,1);

	}
}
#endif




#endif

void SPI_Test(void)
{
	iot_printf("SPI_Test~~~~\n");
	HAL_SPI_Reset(spi3);
    u8 data[4];
	u32 clock = 40000000;
	SPI_InitTpyeDef SPI_InitStruct;

	SPI_InitStruct.SPI_MODE = Master;
	SPI_InitStruct.SPI_Dir = LINE_4;
	SPI_InitStruct.SPI_DMA = 0;
	SPI_InitStruct.SPI_CS_Config = hardware;
	SPI_InitStruct.SPI_Prescaler = clock;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	iot_printf("SPI_CPHA = %d,SPI_CPOL = %d,SPI_FirstBit = %d mode = %d\n",SPI_InitStruct.SPI_CPHA,SPI_InitStruct.SPI_CPOL,SPI_InitStruct.SPI_FirstBit,SPI_InitStruct.SPI_MODE);

	HAL_SPI_Set_SCLK_Period(spi3,CLOCK_40M);
	HAL_SPI_Init(spi3,&SPI_InitStruct);

    data[0] = 0xaa;
    data[1] = 0xbb;
    data[2] = 0xcc;
    data[3] = 0xdd;

    Console_SetPolling(1);
    HAL_SPI_Write(spi3,data,1);
    HAL_SPI_Write(spi3,data,2);
    HAL_SPI_Write(spi3,data,3);
    HAL_SPI_Write(spi3,data,4);

}






