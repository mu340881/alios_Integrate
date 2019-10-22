/*
 * spi.h
 *
 *  Created on: 2018-11-23
 *      Author: NANXIAOFENG
 */

#ifndef SPI_H_
#define SPI_H_

#include "hal.h"


enum SPI_NO{
   spi2 = 2,
   spi3 = 3
};

enum SPI_MODE{
   Master = 0,
   Slave
};

enum SPI_CLOCK_SOURCE{
	CLOCK_10M = 0,
	CLOCK_20M = 1,
	CLOCK_40M = 4,
	CLOCK_80M = 6
};
enum SPI_Dir{
   LINE_4 = 0,
   LINE_3
};

enum SPI_FirstBit{
   SPI_FirstBit_MSB = 0,
   SPI_FirstBit_LSB
};

enum SPI_CPOL{
	SPI_CPOL_Low = 0,
	SPI_CPOL_High
};

enum SPI_CPHA{
	SPI_CPHA_Edge = 0,
	SPI_CPHA_2Edge
};

enum SPI_Status{
    Tx_Status,
    Rx_Status,
    SPIActive
};

enum SPI_CS_Config{
   hardware,
   sofeware
};

enum TransMode{
	write_and_read_same_time,
	write_only,
	read_only,
	write_read,
	read_write,
	write_Dummy_Read,
	read_Dummy_write,
	none_data,
	Dummy_write,
	Dummy_Read,
};


typedef struct
{
   //ox10

   enum SPI_MODE SPI_MODE;             //SPI mode slave/master
   enum SPI_Dir SPI_Dir;               //
   u32 SPI_DataSize;                   //用于设置SPI的数据大小
   enum SPI_CPOL SPI_CPOL;             //用于设置串行时钟的隐态
   enum SPI_CPHA SPI_CPHA;             //用于设置位捕捉的时钟沿；
   enum SPI_FirstBit SPI_FirstBit;     //用于指定数据传输从MSB位还是LSB
   u32 SPI_Prescaler;                  //用于设置发送接收的分频系数
   enum SPI_CS_Config SPI_CS_Config;   //CS/NSS 用软件控制或用硬件控制
   BOOL  SPI_DMA;                      //是否使能DMA
   BOOL  SPI_ADDR_EN;                  //是否使能SPI ADDR
   u32 SPI_ADDR_LEN;                   //地址长度
   BOOL  SPI_CMD_EN;                   //是否使能SPI CMD
}SPI_InitTpyeDef;

#define ATCSPI200_GET_FIELD(var, reg, field)		GET_FIELD(var, ATCSPI200_##reg##_##field##_##MASK, ATCSPI200_##reg##_##field##_##OFFSET)
#define ATCSPI200_DEFAULT(reg, field)				PREPARE_FIELD(ATCSPI200_##reg##_##field##_##DEFAULT, ATCSPI200_##reg##_##field##_##MASK, ATCSPI200_##reg##_##field##_##OFFSET )
#define ATCSPI200_PREPARE(reg, field, value)		PREPARE_FIELD(value, ATCSPI200_##reg##_##field##_##MASK, ATCSPI200_##reg##_##field##_##OFFSET )

void HAL_SPI_Init(enum SPI_NO spi_no,SPI_InitTpyeDef *SPI_InitStruct);
int HAL_SPI_Line_Mode(enum SPI_NO spi_no,enum SPI_Dir spi_dir);
int HAL_SPI_Addr_Enable(enum SPI_NO spi_no,u32 addr_len);
int HAL_SPI_Clock_Set(enum SPI_NO spi_no,UINT32 timing);
u32 HAL_SPI_Set_SCLK_Period(enum SPI_NO spi_no,enum SPI_CLOCK_SOURCE SCLK_TYPE);

u32 HAL_SPI_GET_Status(enum SPI_NO spi_no,enum SPI_Status);
void HAL_SPI_Write_Data(enum SPI_NO spi_no,uint16 data);
u32 HAL_SPI_Read_Data(enum SPI_NO spi_no);
void HAL_SPI_Write(enum SPI_NO spi_no,uint8 *pdata,int len);
void HAL_SPI_Read(enum SPI_NO spi_no,uint8 *pdata,int len);

#endif /* SPI_H_ */
