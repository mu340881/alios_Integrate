/*****************************************************************************
 *
 *            Copyright Andes Technology Corporation 2014
 *                         All Rights Reserved.
 *
 *  Revision History:
 *
 *    Jan.11.2014     Created.
 ****************************************************************************/

#ifndef __AE210P_REGS_H__ 
#define __AE210P_REGS_H__ 

#ifndef __ASSEMBLER__
#include <inttypes.h>
#include <nds32_intrinsic.h>
#endif

#include "cfg_cw1260_iot_fpga.h"
#include "dev_int.h"


/*****************************************************************************
 * IRQ Vector
 ****************************************************************************/
#define IRQ_RTC_PERIOD_VECTOR	DEV_INT_NUM_RTC_PERIOD	
#define IRQ_RTC_ALARM_VECTOR	DEV_INT_NUM_RTC_ALARM	
#define IRQ_PIT1_VECTOR			DEV_INT_NUM_PIT
#define IRQ_PIT2_VECTOR			DEV_INT_NUM_PIT_US
#define IRQ_SPI1_VECTOR			DEV_INT_NUM_SPI1
#define IRQ_I2C_VECTOR			DEV_INT_NUM_I2C
#define IRQ_GPIO_VECTOR			DEV_INT_NUM_GPIO
#define IRQ_UART1_VECTOR		DEV_INT_NUM_UART
#define IRQ_UART2_VECTOR		DEV_INT_NUM_UART2	
#define IRQ_DMA_VECTOR			DEV_INT_NUM_DMA
#define IRQ_SWI_VECTOR			DEV_INT_NUM_WIFI_SW
#define IRQ_HIF_VECTOR			DEV_INT_NUM_HIF
//#define IRQ_WIFI_CPU_VECTOR		DEV_INT_NUM_APP_CPU


/*****************************************************************************
 * address map
 ****************************************************************************/
#define APBBR_BASE		APB_BRG_ADDRESS 
#define SMU_BASE		APB_SMU_REGS
#define UART1_BASE		APB_UART_REGS
#define UART2_BASE		APB_UART_REGS_2
#define PIT1_BASE		PIT_BASE_ADDRESS
#define WDT_BASE		CPE_WDT_BASE
#define RTC_BASE		APB_PWR_RTC_BASE
#define GPIO_BASE		GPIO_BASE_ADDRESS
#define I2C_BASE		I2C_BASE_ADDRESS
#define SPI1_BASE		SPI_BASE_ADDRESS
#define DMAC_BASE		DMA_BASE_ADDRESS /* Device base address */


//new add
#define SRAM_BASE		      PAC_BASE_ADDRESS		//256k
//#define	EILM_ILC_SPI_BASE	  SPI_AHB_ADDRESS



/*****************************************************************************
 * ExLM - ALTOBEAM AHB
 * **************************************************************************/
#define EILM_BASE			DEFAULT_IVB_ADDRESS
#define EDLM_BASE			DTCM_ADDRESS

/*****************************************************************************
 * AHBC - ALTOBEAM AHB
 ****************************************************************************/


/*****************************************************************************
 * APBBRG - ALTOBEAM APB
 ****************************************************************************/



/*****************************************************************************
 * SMU - AE210P
 ****************************************************************************/

/*****************************************************************************
 * UARTx - AE210P
 ****************************************************************************/
/* Device base address */
#define STUARTC_BASE                    UART1_BASE /* standard/IR UART */
//#define BTUARTC_BASE                    0x00F03000 /* blue tooth UART */

/* UART register offsets (4~8-bit width) */
/* SD_LCR_DLAB == 0 */
#define UARTC_RBR_OFFSET                0x20 /* receiver biffer register */
#define UARTC_THR_OFFSET                0x20 /* transmitter holding register */
#define UARTC_IER_OFFSET                0x24 /* interrupt enable register */
#define UARTC_IIR_OFFSET                0x28 /* interrupt identification register */
#define UARTC_FCR_OFFSET                0x28 /* FIFO control register */
#define UARTC_LCR_OFFSET                0x2c /* line control regitser */
#define UARTC_MCR_OFFSET                0x30 /* modem control register */
#define UARTC_LSR_OFFSET                0x34 /* line status register */
#define UARTC_TST_OFFSET                0x34 /* testing register */
#define UARTC_MSR_OFFSET                0x38 /* modem status register */
#define UARTC_SPR_OFFSET                0x3c /* scratch pad register */

/* SD_LCR_DLAB == 0 */
#define UARTC_DLL_OFFSET                0x20 /* baudrate divisor latch LSB */
#define UARTC_DLM_OFFSET                0x24 /* baudrate divisor latch MSB */
#define UARTC_PSR_OFFSET                0x28 /* prescaler register */

#define UARTC_MDR_OFFSET                0x40 /* mode definition register */
#define UARTC_ACR_OFFSET                0x44 /* auxiliary control register */


/*****************************************************************************
 * PIT - AE210P
 ****************************************************************************/
#ifndef PIT_BASE
#define PIT_BASE			PIT1_BASE /* Device base address */
#endif //PIT_BASE

#define PIT_DEV_INT_NUM 		DEV_INT_NUM_PIT
//#else //(CHIP_TYPE == CHIP_ARES)
/*
UART2_RXD_GPIO20_inout_pin	PIT		pit3_ch0_pwm
UART2_TXD_GPIO21_inout_pin	PIT		pit3_ch1_pwm
UART2_CTS_GPIO22_inout_pin	PIT		pit3_ch2_pwm
UART2_RTS_GPIO23_inout_pin	PIT		pit3_ch3_pwm
because wo default use uart, so we need pit3, for more PWM pin
*/
//#define PIT_BASE                PIT3_BASE_ADDRESS
//#define PIT_DEV_INT_NUM 		DEV_INT_NUM_PIT
//#endif //(CHIP_TYPE == CHIP_ATHENAB)


#define PIT_IDREV_OFFSET        0x00    /* Offset for PIT ID and revision register */
#define PIT_CONFIG_OFFSET       0x10    /* Offset for PIT configure register */
#define PIT_INTE_OFFSET         0x14    /* Offset for PIT interrupt enable counter */
#define PIT_INTS_OFFSET         0x18    /* Offset for PIT interrupt status counter */
#define PIT_CHNEN_OFFSET        0x1C    /* Offset for PIT channel enable register */
#define PIT_CTRL_CHN(N)         (0x20 + 0x10 * (N))
#define PIT_LOAD_CHN(N)         (0x24 + 0x10 * (N))
#define PIT_CNTR_CHN(N)         (0x28 + 0x10 * (N))

/* PIT Channel 0 ~ 3 as Timer 0 ~ 3, ATCPIT100 */
#define TM_IE                   (PIT_BASE + PIT_INTE_OFFSET)
#define TM_STA                  (PIT_BASE + PIT_INTS_OFFSET)
#define TM_CHNEN                (PIT_BASE + PIT_CHNEN_OFFSET)
#define TM_CTRL(N)              (PIT_BASE + PIT_CTRL_CHN(N))
#define TM_LOAD(N)              (PIT_BASE + PIT_LOAD_CHN(N))
#define TM_CNTR(N)              (PIT_BASE + PIT_CNTR_CHN(N))
/*bit 7:0 ,set how much clock is one us*/
#define TM_US_NUM_SET        	(PIT_BASE + 0x60)
/*32 bits counter fot the load of us*/
#define TM_LOAD_US          	(PIT_BASE + 0x64)
/*8 bits counter fot the couting of clock*/
#define TM_US_NUM_CNT        	(PIT_BASE + 0x68)
/*32 bits counter fot the couting of us*/
#define TM_CNTR_US           	(PIT_BASE + 0x6c)
/************************************/


#define PIT1_BASE                PIT_BASE_ADDRESS
#define PIT1_IDREV_OFFSET        0x00    /* Offset for PIT ID and revision register */
#define PIT1_CONFIG_OFFSET       0x10    /* Offset for PIT configure register */
#define PIT1_INTE_OFFSET         0x14    /* Offset for PIT interrupt enable counter */
#define PIT1_INTS_OFFSET         0x18    /* Offset for PIT interrupt status counter */
#define PIT1_CHNEN_OFFSET        0x1C    /* Offset for PIT channel enable register */
#define PIT1_CTRL_CHN(N)         (0x20 + 0x10 * (N))
#define PIT1_LOAD_CHN(N)         (0x24 + 0x10 * (N))
#define PIT1_CNTR_CHN(N)         (0x28 + 0x10 * (N))


/************************************/

#define PIT2_BASE                PIT2_BASE_ADDRESS
#define PIT2_IDREV_OFFSET        0x00    /* Offset for PIT ID and revision register */
#define PIT2_CONFIG_OFFSET       0x10    /* Offset for PIT configure register */
#define PIT2_INTE_OFFSET         0x14    /* Offset for PIT interrupt enable counter */
#define PIT2_INTS_OFFSET         0x18    /* Offset for PIT interrupt status counter */
#define PIT2_CHNEN_OFFSET        0x1C    /* Offset for PIT channel enable register */
#define PIT2_CTRL_CHN(N)         (0x20 + 0x10 * (N))
#define PIT2_LOAD_CHN(N)         (0x24 + 0x10 * (N))
#define PIT2_CNTR_CHN(N)         (0x28 + 0x10 * (N))



/************************************/

#define PIT3_BASE                PIT3_BASE_ADDRESS
#define PIT3_IDREV_OFFSET        0x00    /* Offset for PIT ID and revision register */
#define PIT3_CONFIG_OFFSET       0x10    /* Offset for PIT configure register */
#define PIT3_INTE_OFFSET         0x14    /* Offset for PIT interrupt enable counter */
#define PIT3_INTS_OFFSET         0x18    /* Offset for PIT interrupt status counter */
#define PIT3_CHNEN_OFFSET        0x1C    /* Offset for PIT channel enable register */
#define PIT3_CTRL_CHN(N)         (0x20 + 0x10 * (N))
#define PIT3_LOAD_CHN(N)         (0x24 + 0x10 * (N))
#define PIT3_CNTR_CHN(N)         (0x28 + 0x10 * (N))


/* PIT register (32-bit width) */
#define PIT_ID_REV			(PIT_BASE + 0x00 ) /* (ro)  PIT ID and Revision Register */
#define PIT_CFG				(PIT_BASE + 0x10 ) /* (ro)  PIT Configuration Register	 */
#define PIT_INT_EN			(PIT_BASE + 0x14 ) /* (rw)  PIT Interrupt Enable Register*/
#define PIT_INT_ST			(PIT_BASE + 0x18 ) /* (w1c) PIT Interrupt Status Register*/
#define PIT_CH_EN			(PIT_BASE + 0x1C ) /* (rw)  PIT Channel Enable Register	 */

/* PIT Channel 0 Registers (32-bit width) */
#define PIT_C0_CTL			(PIT_BASE + 0x20 ) /* (rw)  PIT Channel 0 Control Register */
#define PIT_C0_LOAD			(PIT_BASE + 0x24 ) /* (rw)  PIT Channel 0 Reload Register */
#define PIT_C0_COUNT			(PIT_BASE + 0x28 ) /* (ro)  PIT Channel 0 Counter Registe */

/* PIT Channel 1 Registers (32-bit width) */
#define PIT_C1_CTL			(PIT_BASE + 0x30 ) /* (rw)  PIT Channel 1 Control Register */
#define PIT_C1_LOAD			(PIT_BASE + 0x34 ) /* (rw)  PIT Channel 1 Reload Register */
#define PIT_C1_COUNT			(PIT_BASE + 0x38 ) /* (ro)  PIT Channel 1 Counter Registe */

/* PIT Channel 2 Registers (32-bit width) */
#define PIT_C2_CTL			(PIT_BASE + 0x40 ) /* (rw)  PIT Channel 2 Control Register */
#define PIT_C2_LOAD			(PIT_BASE + 0x44 ) /* (rw)  PIT Channel 2 Reload Register */
#define PIT_C2_COUNT			(PIT_BASE + 0x48 ) /* (ro)  PIT Channel 2 Counter Registe */

/* PIT Channel 3 Registers (32-bit width) */
#define PIT_C3_CTL			(PIT_BASE + 0x50 ) /* (rw)  PIT Channel 3 Control Register */
#define PIT_C3_LOAD			(PIT_BASE + 0x54 ) /* (rw)  PIT Channel 3 Reload Register */
#define PIT_C3_COUNT			(PIT_BASE + 0x58 ) /* (ro)  PIT Channel 3 Counter Registe */

/* PIT Channel 0 ~ 3 as Timer 0 ~ 3, ATCPIT100 */
#define TM1_IE                   (PIT1_BASE + PIT1_INTE_OFFSET)
#define TM1_STA                  (PIT1_BASE + PIT1_INTS_OFFSET)
#define TM1_CHNEN                (PIT1_BASE + PIT1_CHNEN_OFFSET)
#define TM1_CTRL(N)              (PIT1_BASE + PIT1_CTRL_CHN(N))
#define TM1_LOAD(N)              (PIT1_BASE + PIT1_LOAD_CHN(N))
#define TM1_CNTR(N)              (PIT1_BASE + PIT1_CNTR_CHN(N))
/*bit 7:0 ,set how much clock is one us*/
#define TM1_US_NUM_SET        (PIT1_BASE + 0x60)
/*31 bits counter fot the load of us*/
#define TM1_LOAD_US           (PIT1_BASE + 0x64)
/*8 bits counter fot the couting of clock*/
#define TM1_US_NUM_CNT        (PIT1_BASE + 0x68)
/*31 bits counter fot the couting of us*/
#define TM1_CNTR_US           (PIT1_BASE + 0x6c)
/* PIT Channel 0 ~ 3 as Timer 0 ~ 3, ATCPIT100 */
#define TM2_IE                   (PIT2_BASE + PIT2_INTE_OFFSET)
#define TM2_STA                  (PIT2_BASE + PIT2_INTS_OFFSET)
#define TM2_CHNEN                (PIT2_BASE + PIT2_CHNEN_OFFSET)
#define TM2_CTRL(N)              (PIT2_BASE + PIT2_CTRL_CHN(N))
#define TM2_LOAD(N)              (PIT2_BASE + PIT2_LOAD_CHN(N))
#define TM2_CNTR(N)              (PIT2_BASE + PIT2_CNTR_CHN(N))
/*bit 7:0 ,set how much clock is one us*/
#define TM2_US_NUM_SET        (PIT2_BASE + 0x60)
/*32 bits counter fot the load of us*/
#define TM2_LOAD_US           (PIT2_BASE + 0x64)
/*8 bits counter fot the couting of clock*/
#define TM2_US_NUM_CNT        (PIT2_BASE + 0x68)
/*32 bits counter fot the couting of us*/
#define TM2_CNTR_US           (PIT2_BASE + 0x6c)


/* PIT Channel 0 ~ 3 as Timer 0 ~ 3, ATCPIT100 */
#define TM3_IE                   (PIT3_BASE + PIT3_INTE_OFFSET)
#define TM3_STA                  (PIT3_BASE + PIT3_INTS_OFFSET)
#define TM3_CHNEN                (PIT3_BASE + PIT3_CHNEN_OFFSET)
#define TM3_CTRL(N)              (PIT3_BASE + PIT3_CTRL_CHN(N))
#define TM3_LOAD(N)              (PIT3_BASE + PIT3_LOAD_CHN(N))
#define TM3_CNTR(N)              (PIT3_BASE + PIT3_CNTR_CHN(N))
/*bit 7:0 ,set how much clock is one us*/
#define TM3_US_NUM_SET        (PIT3_BASE + 0x60)
/*32 bits counter fot the load of us*/
#define TM3_LOAD_US           (PIT3_BASE + 0x64)
/*8 bits counter fot the couting of clock*/
#define TM3_US_NUM_CNT        (PIT3_BASE + 0x68)
/*32 bits counter fot the couting of us*/
#define TM3_CNTR_US           (PIT3_BASE + 0x6c)

/************************************/




#define TM_CTRL_32BIT           (0x1 << 0)
#define TM_CTRL_16BIT           (0x2 << 0)
#define TM_CTRL_8BIT           	(0x3 << 0)
#define TM_CTRL_MODE_PWM		(0x4 << 0)

/*APB clock 40Mhz*/
#define TM_CTRL_PCLK            (0x1 << 3)
/*External clock 32Khz*/
#define TM_CTRL_EXTCLK          (0x0 << 3)


/*****************************************************************************
 * WDT - AE210P
 ****************************************************************************/
#define WDTC_BASE			WDT_BASE/* Device base address */


/*****************************************************************************
 * RTC - AE210P
 ****************************************************************************/

/*****************************************************************************
 * GPIO - AE210P
 ****************************************************************************/
#define GPIOC_BASE			GPIO_BASE/* Device base address */


/*****************************************************************************
 * I2C - AE210P
 ****************************************************************************/


/*****************************************************************************
 * SPI1 - AE210P
 ****************************************************************************/



/*****************************************************************************
 * I2S/AC97 - AE210P (SSP2)
 ****************************************************************************/

/*****************************************************************************
 * APB_SLAVE - AE210P Vender APB Slave 0~4
 ****************************************************************************/
//#define APB_SLAVE_BASE 			0x00F19000


/*****************************************************************************
 * Macros for Register Access
 ****************************************************************************/
#ifdef REG_IO_HACK

/* 8 bit access */
//#define IN8(reg)			(  *( (volatile uint8_t *) (reg) ) )
#define OUT8(reg, data)			( (*( (volatile uint8_t *) (reg) ) ) = (uint8_t)(data) )

#define CLR8(reg)			(  *( (volatile uint8_t *) (reg) ) = (uint8_t)0 )
#define MASK8(reg, mask)		(  *( (volatile uint8_t *) (reg) ) & (uint8_t)(mask) )
#define UMSK8(reg, mask)		(  *( (volatile uint8_t *) (reg) ) & ~( (uint8_t)(mask) ) )

#define SETR8SHL(reg, mask, shift, v)	(  *( (volatile uint8_t *) (reg) ) = \
					( ( *( (volatile uint8_t *) (reg) ) & ~( (uint8_t)(mask) ) ) | \
					( ( (uint8_t)(v) << (shift) ) & (uint8_t)(mask)          ) ) )

#define SETR8(reg, mask)		(  *( (volatile uint8_t *) (reg) ) = \
					( ( *( (volatile uint8_t *) (reg) ) & ~( (uint8_t)(mask) ) ) | (uint8_t)(mask) ) )

#define CLRR8(reg, mask)		(  *( (volatile uint8_t *) (reg) ) &= ~( (uint8_t)(mask) ) )

#define SETB8(reg, bit)			(  *( (volatile uint8_t *) (reg) ) |= (uint8_t)( (uint8_t)1 << (bit) ) )
#define CLRB8(reg, bit)			(  *( (volatile uint8_t *) (reg) ) &= ( ~( (uint8_t) ( (uint8_t)1 << (bit) ) ) ) )
#define GETB8(reg, bit)			(  *( (volatile uint8_t *) (reg) ) & (uint8_t) ( (uint8_t)1 << (bit) ) )
#define GETB8SHR(reg, bit)		( (*( (volatile uint8_t *) (reg) ) & (uint8_t) ( (uint8_t)1 << (bit) )) >> (bit) )

/* 16 bit access */
#define IN16(reg)			(  *( (volatile uint16_t *) (reg) ) )
#define OUT16(reg, data)		( (*( (volatile uint16_t *) (reg) ) ) = (uint16_t)(data) )

#define CLR16(reg)			(  *( (volatile uint16_t *) (reg) ) = (uint16_t)0 )
#define MASK16(reg, mask)		(  *( (volatile uint16_t *) (reg) ) & (uint16_t)(mask) )
#define UMSK16(reg, mask)		(  *( (volatile uint16_t *) (reg) ) & ~( (uint16_t)(mask) ) )

#define SETR16SHL(reg, mask, shift, v)	(  *( (volatile uint16_t *) (reg) ) = \
					( ( *( (volatile uint16_t *) (reg) ) & ~( (uint16_t)(mask) ) ) | \
					( ( (uint16_t)(v) << (shift) ) & (uint16_t)(mask)          ) ) )

#define SETR16(reg, mask)		(  *( (volatile uint16_t *) (reg) ) = \
					( ( *( (volatile uint16_t *) (reg) ) & ~( (uint16_t)(mask) ) ) | (uint16_t)(mask) ) )

#define CLRR16(reg, mask)		(  *( (volatile uint16_t *) (reg) ) &= ~( (uint16_t)(mask) ) )

#define SETB16(reg, bit)		(  *( (volatile uint16_t *) (reg) ) |= (uint16_t)( (uint16_t)1 << (bit) ) )
#define CLRB16(reg, bit)		(  *( (volatile uint16_t *) (reg) ) &= ( ~( (uint16_t) ( (uint16_t)1 << (bit) ) ) ) )
#define GETB16(reg, bit)		(  *( (volatile uint16_t *) (reg) ) & (uint16_t) ( (uint16_t)1 << (bit) ) )
#define GETB16SHR(reg, bit)		( (*( (volatile uint16_t *) (reg) ) & (uint16_t) ( (uint16_t)1 << (bit) )) >> (bit) )

/* 32 bit access */
#define IN32(reg)			_IN32((uint32_t)(reg))
#define OUT32(reg, data)		_OUT32((uint32_t)(reg), (uint32_t)(data))

#define CLR32(reg)			_CLR32((uint32_t)(reg))
#define MASK32(reg, mask)		_MASK32((uint32_t)(reg), (uint32_t)(mask))
#define UMSK32(reg, mask)		_UMSK32((uint32_t)(reg), (uint32_t)(mask))

#define SETR32SHL(reg, mask, shift, v)	_SETR32SHL((uint32_t)(reg), (uint32_t)(mask), (uint32_t)(shift), (uint32_t)(v))
#define SETR32(reg, mask)		_SETR32((uint32_t)(reg), (uint32_t)(mask))
#define CLRR32(reg, mask)		_CLRR32((uint32_t)(reg), (uint32_t)(mask))

#define SETB32(reg, bit)		_SETB32((uint32_t)(reg), (uint32_t)(bit))
#define CLRB32(reg, bit)		_CLRB32((uint32_t)(reg), (uint32_t)(bit))
#define GETB32(reg, bit)		_GETB32((uint32_t)(reg), (uint32_t)(bit))
#define GETB32SHR(reg, bit)		_GETB32SHR((uint32_t)(reg), (uint32_t)(bit))

#else /* REG_IO_HACK */

/* 8 bit access */
//#define IN8(reg)			(  *( (volatile uint8_t *) (reg) ) )
#define OUT8(reg, data)			( (*( (volatile uint8_t *) (reg) ) ) = (uint8_t)(data) )

#define CLR8(reg)			(  *( (volatile uint8_t *) (reg) ) = (uint8_t)0 )
#define MASK8(reg, mask)		(  *( (volatile uint8_t *) (reg) ) & (uint8_t)(mask) )
#define UMSK8(reg, mask)		(  *( (volatile uint8_t *) (reg) ) & ~( (uint8_t)(mask) ) )

#define SETR8SHL(reg, mask, shift, v)	(  *( (volatile uint8_t *) (reg) ) = \
					( ( *( (volatile uint8_t *) (reg) ) & ~( (uint8_t)(mask) ) ) | \
					( ( (uint8_t)(v) << (shift) ) & (uint8_t)(mask)          ) ) )

#define SETR8(reg, mask)		(  *( (volatile uint8_t *) (reg) ) = \
					( ( *( (volatile uint8_t *) (reg) ) & ~( (uint8_t)(mask) ) ) | (uint8_t)(mask) ) )

#define CLRR8(reg, mask)		(  *( (volatile uint8_t *) (reg) ) &= ~( (uint8_t)(mask) ) )

#define SETB8(reg, bit)			(  *( (volatile uint8_t *) (reg) ) |= (uint8_t)( (uint8_t)1 << (bit) ) )
#define CLRB8(reg, bit)			(  *( (volatile uint8_t *) (reg) ) &= ( ~( (uint8_t) ( (uint8_t)1 << (bit) ) ) ) )
#define GETB8(reg, bit)			(  *( (volatile uint8_t *) (reg) ) & (uint8_t) ( (uint8_t)1 << (bit) ) )
#define GETB8SHR(reg, bit)		( (*( (volatile uint8_t *) (reg) ) & (uint8_t) ( (uint8_t)1 << (bit) )) >> (bit) )

/* 16 bit access */
#define IN16(reg)			(  *( (volatile uint16_t *) (reg) ) )
#define OUT16(reg, data)		( (*( (volatile uint16_t *) (reg) ) ) = (uint16_t)(data) )

#define CLR16(reg)			(  *( (volatile uint16_t *) (reg) ) = (uint16_t)0 )
#define MASK16(reg, mask)		(  *( (volatile uint16_t *) (reg) ) & (uint16_t)(mask) )
#define UMSK16(reg, mask)		(  *( (volatile uint16_t *) (reg) ) & ~( (uint16_t)(mask) ) )

#define SETR16SHL(reg, mask, shift, v)	(  *( (volatile uint16_t *) (reg) ) = \
					( ( *( (volatile uint16_t *) (reg) ) & ~( (uint16_t)(mask) ) ) | \
					( ( (uint16_t)(v) << (shift) ) & (uint16_t)(mask)          ) ) )

#define SETR16(reg, mask)		(  *( (volatile uint16_t *) (reg) ) = \
					( ( *( (volatile uint16_t *) (reg) ) & ~( (uint16_t)(mask) ) ) | (uint16_t)(mask) ) )
#define CLRR16(reg, mask)		(  *( (volatile uint16_t *) (reg) ) &= ~( (uint16_t)(mask) ) )

#define SETB16(reg, bit)		(  *( (volatile uint16_t *) (reg) ) |= (uint16_t)( (uint16_t)1 << (bit) ) )
#define CLRB16(reg, bit)		(  *( (volatile uint16_t *) (reg) ) &= ( ~( (uint16_t) ( (uint16_t)1 << (bit) ) ) ) )
#define GETB16(reg, bit)		(  *( (volatile uint16_t *) (reg) ) & (uint16_t) ( (uint16_t)1 << (bit) ) )
#define GETB16SHR(reg, bit)		( (*( (volatile uint16_t *) (reg) ) & (uint16_t) ( (uint16_t)1 << (bit) )) >> (bit) )

/* 32 bit access */
#define IN32(reg)			(  *( (volatile uint32_t *) (reg) ) )
#define OUT32(reg, data)		( (*( (volatile uint32_t *) (reg) ) ) = (uint32_t)(data) )

#define CLR32(reg)			(  *( (volatile uint32_t *) (reg) ) = (uint32_t)0 )
#define MASK32(reg, mask)		(  *( (volatile uint32_t *) (reg) ) & (uint32_t)(mask) )
#define UMSK32(reg, mask)		(  *( (volatile uint32_t *) (reg) ) & ~( (uint32_t)(mask) ) )

#define SETR32SHL(reg, mask, shift, v)	(  *( (volatile uint32_t *) (reg) ) = \
					( ( *( (volatile uint32_t *) (reg) ) & ~( (uint32_t)(mask) ) ) | \
					( ( (uint32_t)(v) << (shift) ) & (uint32_t)(mask)          ) ) )

#define SETR32(reg, mask)		(  *( (volatile uint32_t *) (reg) ) = \
					( ( *( (volatile uint32_t *) (reg) ) & ~( (uint32_t)(mask) ) ) | (uint32_t)(mask) ) )

#define CLRR32(reg, mask)		(  *( (volatile uint32_t *) (reg) ) &= ~( (uint32_t)(mask) ) )

#define SETB32(reg, bit)		(  *( (volatile uint32_t *) (reg) ) |= (uint32_t)( (uint32_t)1 << (bit) ) )
#define CLRB32(reg, bit)		(  *( (volatile uint32_t *) (reg) ) &= ( ~( (uint32_t) ( (uint32_t)1 << (bit) ) ) ) )
#define GETB32(reg, bit)		(  *( (volatile uint32_t *) (reg) ) & (uint32_t) ( (uint32_t)1 << (bit) ) )
#define GETB32SHR(reg, bit)		( (*( (volatile uint32_t *) (reg) ) & (uint32_t) ( (uint32_t)1 << (bit) )) >> (bit) )

#endif /* REG_IO_HACK */

#define SR_CLRB32(reg, bit)		\
{					\
	int mask = __nds32__mfsr(reg)& ~(1<<bit);\
        __nds32__mtsr(mask, reg);	 \
	__nds32__dsb();				 \
}

#define SR_SETB32(reg,bit)\
{\
	int mask = __nds32__mfsr(reg)|(1<<bit);\
	__nds32__mtsr(mask, reg);			\
	__nds32__dsb();						\
}


// Register bit operation macro
#define BIT_MASK(bit_h, bit_l)	((UINT32)((((UINT64)0x1<<(1+bit_h-bit_l))-(UINT32)0x1)<<bit_l))
#define SET_BIT(var, bit)			do { var |= (0x1 << (bit)); } while(0) 
#define CLR_BIT(var, bit)			do { var &= (~(0x1 << (bit))); } while(0)

#define SET_FIELD(var, mask, offset, value)	do {\
        						var = ((var) & (~mask)) | (((value) << (offset)) & (mask)); \
        					} while (0)
#define GET_FIELD(var, mask, offset)		(((var) & (mask)) >> (offset))
#define PREPARE_FIELD(value, mask, offset)	(((value) << (offset)) & (mask))


/* Low-level port I/O */
#define inw(reg)        (*((volatile unsigned int *) (reg)))
#define outw(reg, data) ((*((volatile unsigned int *)(reg)))=(unsigned int)(data))

#endif /* __AE210P_REGS_INC__ */
