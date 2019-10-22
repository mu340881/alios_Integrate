#ifndef __FLASH_H_
#define __FLASH_H_

#include <type.h>
#include <stdio.h>
#include <string.h>
#include "stddefs.h"
#include "cfg_cw1260_iot_fpga.h"
#include "dmad.h"

/*****************************************************************************
 * Configuration section 
 ****************************************************************************/	
	
#define DEVICE_SUPPORT_DUAL		1
#define DEVICE_SUPPORT_QUAD		2
#define RESEVED_DATA			0


//GIGA GD25Q80C
#define FLASH_GD25Q80C_MANUFACTURER_ID 	(0xC8)
#define FLASH_GD25Q80C_MEM_TYPE		 	(0x40)
#define FLASH_GD25Q80C_CAPACITY		 	(0x14)


//GIGA GD25Q32C
#define FLASH_GD25Q32C_MANUFACTURER_ID  (0xC8)
#define FLASH_GD25Q32C_MEM_TYPE		 	(0x40)
#define FLASH_GD25Q32C_CAPACITY		 	(0x16)

//GIGA W25X80
#define FLASH_W25X80_MANUFACTURER_ID 	(0xef)
#define FLASH_W25X80_MEM_TYPE		 	(0x60)
#define FLASH_W25X80_CAPACITY		 	(0x18)

//GIGA W25X80
#define FLASH_W25X80_MANUFACTURER_ID 	(0xef)
#define FLASH_W25X80_MEM_TYPE		 	(0x60)
#define FLASH_W25X80_CAPACITY		 	(0x18)



//#define CTRL_FLASH_CODE					(1)

#define FLASH_DEBUG_TRACE	0/* non-zero to enable debug trace message */
//#define CPE_SPI_IRQ_VECTOR		IRQ_SPI2_VECTOR
#define CPE_SPI_REG_BASE		SPI_BASE_ADDRESS
#define CPE_SPI_TX_FIFO_SIZE	4 		//byte

#define CPE_SPI_EXT_CLK			APB_CLK

#define SPI_EXT_CLK_40M						(0)
#define SPI_EXT_CLK_80M						(4)

#define FLASH_R_CMD_DUAL_3BH				(2)				
#define FLASH_R_CMD_QUAD_6BH				(3)						
#define FLASH_R_CMD_DUAL_BBH				(4)					
#define FLASH_R_CMD_QUAD_EBH				(5)	


	
#define DEVICE_ID				0x1860efu//0x4018u
#define DEVICE_SIZE				0x1000000u
#define DEVICE_BLOCK_SIZE		0x008000u
#define DEVICE_SECTOR_SIZE		0x001000u
#define DEVICE_PAGE_SIZE		0x000100u
//#define DEVICE_SUPPORT_DUAL
//#define DEVICE_SUPPORT_QUAD
//#define DEVICE_SUPPORT_DMA

#if (CHIP_TYPE==CHIP_ATHENAB)
#define FLASH_BASE				0x12000000
#else //#if (CHIP_TYPE==CHIP_ATHENAB)
#define FLASH_BASE		 		0x400000
#endif  //#if (CHIP_TYPE==CHIP_ATHENAB)

#define IOT_BOOT_FLASH_ADDR			(FLASH_BASE+0x000000)
//#define IOT_EILM_FLASH_ADDR			(FLASH_BASE+0x1C0000)//size 0x40000 code will copy to EILM
//#define IOT_FLASHCODE_FLASH_ADDR	(FLASH_BASE+0x100000) //size 0xc0000 code run in flash
//#define IOT_EDLM_FLASH_ADDR			(FLASH_BASE+0x280000)

//#define IOT_MAX_BOOT_SIZE   		0x10000

//#define IOT_MAX_CODE_SIZE   0x80000

//#define IOT_MAX_ICCMCODE_SIZE   0x40000
//#define IOT_MAX_DCCM_SIZE   	0xc000
//#define IOT_MAX_FLASHCODE_SIZE   0x80000



typedef struct
{
	uint8 Manufacturer_id;
	uint8 Support_Dual_Quad;
	uint8 SpiClk;
	uint8 FlashReadCMD;
	uint8 Capacity;
}FlashInfo_T;

// -------------------------------------------------------------------------------
//  Macro define
// -------------------------------------------------------------------------------

/* Debug trace enable switch */

/* Debug Trace Mechanism */
#if (FLASH_DEBUG_TRACE)
#define FLASH_TRACE(...)				\
	do{									\
		DBG_Printf("[%s]:", __func__);		\
		DBG_Printf(__VA_ARGS__);			\
	}while(0)
	
#else /* FLASH_DEBUG_TRACE */
#define FLASH_TRACE(...)
#endif /* FLASH_DEBUG_TRACE */


#define FLASH_SUCCESS			0
#define FLASH_FAILURE			-1


// Flash Command
#define SPI_CMD_READ      0x03  // Read data
#define SPI_CMD_FAST_READ 0x0b  // Fast read data
#define SPI_CMD_2READ     0xbb  // 2x I/O read
#define SPI_CMD_4READ     0xeb  // 4x I/O read
#define SPI_CMD_DREAD     0x3b  // 2x I/O read
#define SPI_CMD_QREAD     0x6b  // 4x I/O read
#define SPI_CMD_RDID      0x9f  // Read ID
#define SPI_CMD_READ_ID   0x90  // Read ID
#define SPI_CMD_WREN      0x06  // Write enable
#define SPI_CMD_WRDI      0x04  // Write disable
#define SPI_CMD_SE        0x20	// Sector erase
#define SPI_CMD_BE        0xd8  // Block erase	//64KB Block Erase (D8h)  	//32KB Block Erase (52h)  
#define SPI_CMD_CE        0xc7  // Chip erase 0xc7
#define SPI_CMD_PP        0x02  // Page program
#define SPI_CMD_4PP       0x32  // Quad page program
#define SPI_CMD_RDSR      0x05  // Read status register bit 0~7
#define SPI_CMD_RDSR_35H  0x35  // Read status register bit 15~8

#define SPI_CMD_WRSR      0x01  // Write status register-1
#define SPI_CMD_WRSR_31H  0x31  // Write status register-2
#define SPI_CMD_DP        0xb9  // Deep power down
#define SPI_CMD_RES       0xab  // Release from deep power down

#define SPI_NO_CMD		  0xff

//flash GD25Q80C Command
#define SPI_CMD_WREN_ST   0x50  // Write Enable for Volatile Status Register Command




// Status byte
#define FLASH_STATUS_WIP_MASK   0x01
#define FLASH_STATUS_WEL_MASK   0x02
#define FLASH_STATUS_WPS_BIT  	BIT(18)
#define FLASH_STATUS_CMP_BIT  	BIT(14)
#define FLASH_STATUS_SEC_BIT  	BIT(6)
#define FLASH_STATUS_TB_BIT  	BIT(5)
#define FLASH_STATUS_BP2_BIT  	BIT(4)
#define FLASH_STATUS_BP1_BIT  	BIT(3)
#define FLASH_STATUS_BP0_BIT  	BIT(2)


#define WORD_TO_BYTES(b3, b2, b1, b0, w0) do {\
	b0=(UINT8)(w0);\
	b1=(UINT8)((w0)>>8);\
	b2=(UINT8)((w0)>>16);\
	b3=(UINT8)((w0)>>24);} while (0)

#define BYTES_TO_WORD(w0, b3, b2, b1, b0) \
	w0=((b3)<<24)|((b2)<<16)|((b1)<<8)|(b0)


static const char lut_br[16] = {0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
	0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf};

#define BIT_REVERT8(x) (lut_br[((x) & 0xf0) >> 4] | lut_br[(x) & 0xf] << 4)

#define BIT_REVERT_BY_BYTE(x) (\
		BIT_REVERT8((x) >>  0) <<  0 |\
		BIT_REVERT8((x) >>  8) <<  8 |\
		BIT_REVERT8((x) >> 16) << 16 |\
		BIT_REVERT8((x) >> 24) << 24)

#define BIT_REVERT24(x) (\
		BIT_REVERT8((x) >>  0) << 16 |\
		BIT_REVERT8((x) >>  8) <<  8 |\
		BIT_REVERT8((x) >> 16) <<  0)

// REG SPI data control: TRAMODE
#define TRAMODE_WRnRD      0x0
#define TRAMODE_WRonly     0x1
#define TRAMODE_RDonly     0x2
#define TRAMODE_WR_RD      0x3
#define TRAMODE_RD_WR      0x4
#define TRAMODE_WR_DMY_RD  0x5
#define TRAMODE_RD_DMY_WR  0x6
#define TRAMODE_NoneData   0x7
#define TRAMODE_DMY_WR     0x8
#define TRAMODE_DMY_RD     0x9
#define TRAMODE_LAST	TRAMODE_DMY_RD

// SPI INTERRUPT
#define SPI_SPIEI         ATCSPI200_INTEN_SPIEIE_MASK
#define SPI_TXFTHI        ATCSPI200_INTEN_TXFTHIE_MASK
#define SPI_RXFTHI        ATCSPI200_INTEN_RXFTHIE_MASK
#define SPI_TXFURI        ATCSPI200_INTEN_TXFURIE_MASK
#define SPI_RXFORI        ATCSPI200_INTEN_RXFORIE_MASK
#define SPI_SLVCMDI       ATCSPI200_INTEN_SLVCMDEN_MASK


// Offset + 0x00 ID and Revision Register
#define ATCSPI200_VER_ID_MASK  (BIT_MASK(31, 16))
#define ATCSPI200_VER_ID_OFFSET  (16)
#define ATCSPI200_VER_ID_DEFAULT  (0x0200)
#define ATCSPI200_VER_MAJOR_MASK  (BIT_MASK(15, 4))
#define ATCSPI200_VER_MAJOR_OFFSET  (4)
#define ATCSPI200_VER_MAJOR_DEFAULT  (0x202)
#define ATCSPI200_VER_MINOR_MASK  (BIT_MASK(3, 0))
#define ATCSPI200_VER_MINOR_OFFSET  (0)
#define ATCSPI200_VER_MINOR_DEFAULT  (0x0)
#define ATCSPI200_VER_DEFAULT  ( \
		ATCSPI200_DEFAULT(VER, ID) |\
		ATCSPI200_DEFAULT(VER, MAJOR) |\
		ATCSPI200_DEFAULT(VER, MINOR)\
		)

// Offset + 0x10  SPI Interface Setting
#define ATCSPI200_IFSET_CLOCK_PERIOD_MASK  (BIT_MASK(21, 18))
#define ATCSPI200_IFSET_CLOCK_PERIOD_OFFSET  (18)
#define ATCSPI200_IFSET_CLOCK_PERIOD_DEFAULT  (0x0)
#define ATCSPI200_IFSET_ADDR_LEN_MASK  (BIT_MASK(17, 16))
#define ATCSPI200_IFSET_ADDR_LEN_OFFSET  (16)
#define ATCSPI200_IFSET_ADDR_LEN_DEFAULT  (0x2)
#define ATCSPI200_IFSET_DATA_LEN_MASK  (BIT_MASK(12, 8))
#define ATCSPI200_IFSET_DATA_LEN_OFFSET  (8)
#define ATCSPI200_IFSET_DATA_LEN_DEFAULT  (0x7)
#define ATCSPI200_IFSET_DATA_MERGE_MASK  (BIT_MASK(7, 7))
#define ATCSPI200_IFSET_DATA_MERGE_OFFSET  (7)
#define ATCSPI200_IFSET_DATA_MERGE_DEFAULT  (1)
#define ATCSPI200_IFSET_3LINE_MASK  (BIT_MASK(4, 4))
#define ATCSPI200_IFSET_3LINE_OFFSET  (4)
#define ATCSPI200_IFSET_3LINE_DEFAULT  (0)
#define ATCSPI200_IFSET_LSB_MASK  (BIT_MASK(3, 3))
#define ATCSPI200_IFSET_LSB_OFFSET  (3)
#define ATCSPI200_IFSET_LSB_DEFAULT  (0)
#define ATCSPI200_IFSET_MSSL_MASK  (BIT_MASK(2, 2))
#define ATCSPI200_IFSET_MSSL_OFFSET  (2)
#define ATCSPI200_IFSET_MSSL_DEFAULT  (0)
#define ATCSPI200_IFSET_CPOL_MASK  (BIT_MASK(1, 1))
#define ATCSPI200_IFSET_CPOL_OFFSET  (1)
#define ATCSPI200_IFSET_CPOL_DEFAULT  (0)
#define ATCSPI200_IFSET_CPHA_MASK  (BIT_MASK(0, 0))
#define ATCSPI200_IFSET_CPHA_OFFSET  (0)
#define ATCSPI200_IFSET_CPHA_DEFAULT  (0)
#define ATCSPI200_IFSET_DEFAULT  ( \
		ATCSPI200_DEFAULT(IFSET, CLOCK_PERIOD) |\
		ATCSPI200_DEFAULT(IFSET, ADDR_LEN) |\
		ATCSPI200_DEFAULT(IFSET, DATA_LEN) |\
		ATCSPI200_DEFAULT(IFSET, DATA_MERGE) |\
		ATCSPI200_DEFAULT(IFSET, 3LINE) |\
		ATCSPI200_DEFAULT(IFSET, LSB) |\
		ATCSPI200_DEFAULT(IFSET, MSSL) |\
		ATCSPI200_DEFAULT(IFSET, CPOL) |\
		ATCSPI200_DEFAULT(IFSET, CPHA) \
		)

// SPI PIO Register
#define ATCSPI200_PIO_PIOE_MASK  (BIT_MASK(24, 24))
#define ATCSPI200_PIO_PIOE_OFFSET  (24)
#define ATCSPI200_PIO_PIOE_DEFAULT  (0)
#define ATCSPI200_PIO_HOLD_OE_MASK  (BIT_MASK(21, 21))
#define ATCSPI200_PIO_HOLD_OE_OFFSET  (21)
#define ATCSPI200_PIO_HOLD_OE_DEFAULT  (0)
#define ATCSPI200_PIO_WP_OE_MASK  (BIT_MASK(20, 20))
#define ATCSPI200_PIO_WP_OE_OFFSET  (20)
#define ATCSPI200_PIO_WP_OE_DEFAULT  (0)
#define ATCSPI200_PIO_MISO_OE_MASK  (BIT_MASK(19, 19))
#define ATCSPI200_PIO_MISO_OE_OFFSET  (19)
#define ATCSPI200_PIO_MISO_OE_DEFAULT  (0)
#define ATCSPI200_PIO_MOSI_OE_MASK  (BIT_MASK(18, 18))
#define ATCSPI200_PIO_MOSI_OE_OFFSET  (18)
#define ATCSPI200_PIO_MOSI_OE_DEFAULT  (0)
#define ATCSPI200_PIO_SCK_OE_MASK  (BIT_MASK(17, 17))
#define ATCSPI200_PIO_SCK_OE_OFFSET  (17)
#define ATCSPI200_PIO_SCK_OE_DEFAULT  (0)
#define ATCSPI200_PIO_CS_OE_MASK  (BIT_MASK(16, 16))
#define ATCSPI200_PIO_CS_OE_OFFSET  (16)
#define ATCSPI200_PIO_CS_OE_DEFAULT  (0)
#define ATCSPI200_PIO_HOLD_O_MASK  (BIT_MASK(13, 13))
#define ATCSPI200_PIO_HOLD_O_OFFSET  (13)
#define ATCSPI200_PIO_HOLD_O_DEFAULT  (1)
#define ATCSPI200_PIO_WP_O_MASK  (BIT_MASK(12, 12))
#define ATCSPI200_PIO_WP_O_OFFSET  (12)
#define ATCSPI200_PIO_WP_O_DEFAULT  (1)
#define ATCSPI200_PIO_MISO_O_MASK  (BIT_MASK(11, 11))
#define ATCSPI200_PIO_MISO_O_OFFSET  (11)
#define ATCSPI200_PIO_MISO_O_DEFAULT  (0)
#define ATCSPI200_PIO_MOSI_O_MASK  (BIT_MASK(10, 10))
#define ATCSPI200_PIO_MOSI_O_OFFSET  (10)
#define ATCSPI200_PIO_MOSI_O_DEFAULT  (0)
#define ATCSPI200_PIO_SCK_O_MASK  (BIT_MASK(9, 9))
#define ATCSPI200_PIO_SCK_O_OFFSET  (9)
#define ATCSPI200_PIO_SCK_O_DEFAULT  (0)
#define ATCSPI200_PIO_CS_O_MASK  (BIT_MASK(8, 8))
#define ATCSPI200_PIO_CS_O_OFFSET  (8)
#define ATCSPI200_PIO_CS_O_DEFAULT  (1)
#define ATCSPI200_PIO_HOLD_I_MASK  (BIT_MASK(5, 5))
#define ATCSPI200_PIO_HOLD_I_OFFSET  (5)
#define ATCSPI200_PIO_HOLD_I_DEFAULT  (0)
#define ATCSPI200_PIO_WP_I_MASK  (BIT_MASK(4, 4))
#define ATCSPI200_PIO_WP_I_OFFSET  (4)
#define ATCSPI200_PIO_WP_I_DEFAULT  (0)
#define ATCSPI200_PIO_MISO_I_MASK  (BIT_MASK(3, 3))
#define ATCSPI200_PIO_MISO_I_OFFSET  (3)
#define ATCSPI200_PIO_MISO_I_DEFAULT  (0)
#define ATCSPI200_PIO_MOSI_I_MASK  (BIT_MASK(2, 2))
#define ATCSPI200_PIO_MOSI_I_OFFSET  (2)
#define ATCSPI200_PIO_MOSI_I_DEFAULT  (0)
#define ATCSPI200_PIO_SCK_I_MASK  (BIT_MASK(1, 1))
#define ATCSPI200_PIO_SCK_I_OFFSET  (1)
#define ATCSPI200_PIO_SCK_I_DEFAULT  (0)
#define ATCSPI200_PIO_CS_I_MASK  (BIT_MASK(0, 0))
#define ATCSPI200_PIO_CS_I_OFFSET  (0)
#define ATCSPI200_PIO_CS_I_DEFAULT  (0)
#define ATCSPI200_PIO_DEFAULT  ( \
		ATCSPI200_DEFAULT(PIO, PIOE) |\
		ATCSPI200_DEFAULT(PIO, HOLD_OE) |\
		ATCSPI200_DEFAULT(PIO, WP_OE) |\
		ATCSPI200_DEFAULT(PIO, MISO_OE) |\
		ATCSPI200_DEFAULT(PIO, MOSI_OE) |\
		ATCSPI200_DEFAULT(PIO, SCK_OE) |\
		ATCSPI200_DEFAULT(PIO, CS_OE) |\
		ATCSPI200_DEFAULT(PIO, HOLD_O) |\
		ATCSPI200_DEFAULT(PIO, WP_O) |\
		ATCSPI200_DEFAULT(PIO, MISO_O) |\
		ATCSPI200_DEFAULT(PIO, MOSI_O) |\
		ATCSPI200_DEFAULT(PIO, SCK_O) |\
		ATCSPI200_DEFAULT(PIO, CS_O) |\
		ATCSPI200_DEFAULT(PIO, HOLD_I) |\
		ATCSPI200_DEFAULT(PIO, WP_I) |\
		ATCSPI200_DEFAULT(PIO, MISO_I) |\
		ATCSPI200_DEFAULT(PIO, MOSI_I) |\
		ATCSPI200_DEFAULT(PIO, SCK_I) |\
		ATCSPI200_DEFAULT(PIO, CS_I) \
		)


// REG SPI Data Control Register
#define ATCSPI200_REGDCTRL_CMDEN_MASK  (BIT_MASK(30, 30))
#define ATCSPI200_REGDCTRL_CMDEN_OFFSET  (30)
#define ATCSPI200_REGDCTRL_CMDEN_DEFAULT  (0)
#define ATCSPI200_REGDCTRL_ADDREN_MASK  (BIT_MASK(29, 29))
#define ATCSPI200_REGDCTRL_ADDREN_OFFSET  (29)
#define ATCSPI200_REGDCTRL_ADDREN_DEFAULT  (0)
#define ATCSPI200_REGDCTRL_ADDR_DQ_MASK  (BIT_MASK(28, 28))
#define ATCSPI200_REGDCTRL_ADDR_DQ_OFFSET  (28)
#define ATCSPI200_REGDCTRL_ADDR_DQ_DEFAULT  (0)
#define ATCSPI200_REGDCTRL_TRAMODE_MASK  (BIT_MASK(27, 24))
#define ATCSPI200_REGDCTRL_TRAMODE_OFFSET  (24)
#define ATCSPI200_REGDCTRL_TRAMODE_DEFAULT  (0)
#define ATCSPI200_REGDCTRL_DualQuad_MASK  (BIT_MASK(23, 22))
#define ATCSPI200_REGDCTRL_DualQuad_OFFSET  (22)
#define ATCSPI200_REGDCTRL_DualQuad_DEFAULT  (0)
#define ATCSPI200_REGDCTRL_TOKEN_EN_MASK  (BIT_MASK(21, 21))
#define ATCSPI200_REGDCTRL_TOKEN_EN_OFFSET  (21)
#define ATCSPI200_REGDCTRL_TOKEN_EN_DEFAULT  (0)
#define ATCSPI200_REGDCTRL_WCNT_MASK  (BIT_MASK(20, 12))
#define ATCSPI200_REGDCTRL_WCNT_OFFSET  (12)
#define ATCSPI200_REGDCTRL_WCNT_DEFAULT  (0)
#define ATCSPI200_REGDCTRL_TOKEN_VAL_MASK  (BIT_MASK(11, 11))
#define ATCSPI200_REGDCTRL_TOKEN_VAL_OFFSET  (11)
#define ATCSPI200_REGDCTRL_TOKEN_VAL_DEFAULT  (0)
#define ATCSPI200_REGDCTRL_DYCNT_MASK  (BIT_MASK(10, 9))
#define ATCSPI200_REGDCTRL_DYCNT_OFFSET  (9)
#define ATCSPI200_REGDCTRL_DYCNT_DEFAULT  (0)
#define ATCSPI200_REGDCTRL_RCNT_MASK  (BIT_MASK(8, 0))
#define ATCSPI200_REGDCTRL_RCNT_OFFSET  (0)
#define ATCSPI200_REGDCTRL_RCNT_DEFAULT  (0)
#define ATCSPI200_REGDCTRL_DEFAULT  ( \
		ATCSPI200_DEFAULT(REGDCTRL, CMDEN) |\
		ATCSPI200_DEFAULT(REGDCTRL, ADDREN) |\
		ATCSPI200_DEFAULT(REGDCTRL, ADDR_DQ) |\
		ATCSPI200_DEFAULT(REGDCTRL, TRAMODE) |\
		ATCSPI200_DEFAULT(REGDCTRL, DualQuad) |\
		ATCSPI200_DEFAULT(REGDCTRL, TOKEN_EN) |\
		ATCSPI200_DEFAULT(REGDCTRL, WCNT) |\
		ATCSPI200_DEFAULT(REGDCTRL, TOKEN_VAL) |\
		ATCSPI200_DEFAULT(REGDCTRL, DYCNT) |\
		ATCSPI200_DEFAULT(REGDCTRL, RCNT) \
		)


// REG SPI Command Port
#define ATCSPI200_REGCPORT_CMD_MASK  (BIT_MASK(7, 0))
#define ATCSPI200_REGCPORT_CMD_OFFSET  (0)
#define ATCSPI200_REGCPORT_CMD_DEFAULT  (0)
#define ATCSPI200_REGCPORT_DEFAULT  ( \
		ATCSPI200_DEFAULT(REGCPORT, CMD) \
		)

// REG SPI Address Register
#define ATCSPI200_REGAPORT_ADDR_MASK  (BIT_MASK(23, 0))
#define ATCSPI200_REGAPORT_ADDR_OFFSET  (0)
#define ATCSPI200_REGAPORT_ADDR_DEFAULT  (0)
#define ATCSPI200_REGAPORT_DEFAULT  ( \
		ATCSPI200_DEFAULT(REGAPORT, ADDR) \
		)

// REG SPI DATA Register
#define ATCSPI200_REGDPORT_DATAP_MASK  0xFFFFFFFF //(BIT_MASK(31, 0))
#define ATCSPI200_REGDPORT_DATAP_OFFSET  (0)
#define ATCSPI200_REGDPORT_DATAP_DEFAULT  (0)
#define ATCSPI200_REGDPORT_DEFAULT  ( \
		ATCSPI200_DEFAULT(REGDPORT, DATAP) \
		)

// SPI Control Register
#define ATCSPI200_CTRL_TXFTH_MASK  (BIT_MASK(20, 16))
#define ATCSPI200_CTRL_TXFTH_OFFSET  (16)
#define ATCSPI200_CTRL_TXFTH_DEFAULT  (0)
#define ATCSPI200_CTRL_RXFTH_MASK  (BIT_MASK(12, 8))
#define ATCSPI200_CTRL_RXFTH_OFFSET  (8)
#define ATCSPI200_CTRL_RXFTH_DEFAULT  (0)
#define ATCSPI200_CTRL_TXDMAEN_MASK  (BIT_MASK(4, 4))
#define ATCSPI200_CTRL_TXDMAEN_OFFSET  (4)
#define ATCSPI200_CTRL_TXDMAEN_DEFAULT  (0)
#define ATCSPI200_CTRL_RXDMAEN_MASK  (BIT_MASK(3, 3))
#define ATCSPI200_CTRL_RXDMAEN_OFFSET  (3)
#define ATCSPI200_CTRL_RXDMAEN_DEFAULT  (0)
#define ATCSPI200_CTRL_TXFRST_MASK  (BIT_MASK(2, 2))
#define ATCSPI200_CTRL_TXFRST_OFFSET  (2)
#define ATCSPI200_CTRL_TXFRST_DEFAULT  (0)
#define ATCSPI200_CTRL_RXFRST_MASK  (BIT_MASK(1, 1))
#define ATCSPI200_CTRL_RXFRST_OFFSET  (1)
#define ATCSPI200_CTRL_RXFRST_DEFAULT  (0)
#define ATCSPI200_CTRL_SPIRST_MASK  (BIT_MASK(0, 0))
#define ATCSPI200_CTRL_SPIRST_OFFSET  (0)
#define ATCSPI200_CTRL_SPIRST_DEFAULT  (0)
#define ATCSPI200_CTRL_DEFAULT  ( \
		ATCSPI200_DEFAULT(CTRL, TXFTH) |\
		ATCSPI200_DEFAULT(CTRL, RXFTH) |\
		ATCSPI200_DEFAULT(CTRL, TXDMAEN) |\
		ATCSPI200_DEFAULT(CTRL, RXDMAEN) |\
		ATCSPI200_DEFAULT(CTRL, TXFRST) |\
		ATCSPI200_DEFAULT(CTRL, RXFRST) |\
		ATCSPI200_DEFAULT(CTRL, SPIRST) \
		)

// SPI Status Register
#define ATCSPI200_ST_TXFFL_MASK  (BIT_MASK(23, 23))
#define ATCSPI200_ST_TXFFL_OFFSET  (23)
#define ATCSPI200_ST_TXFFL_DEFAULT  (0)
#define ATCSPI200_ST_TXFEM_MASK  (BIT_MASK(22, 22))
#define ATCSPI200_ST_TXFEM_OFFSET  (22)
#define ATCSPI200_ST_TXFEM_DEFAULT  (1)
#define ATCSPI200_ST_TXFVE_MASK  (BIT_MASK(20, 16))
#define ATCSPI200_ST_TXFVE_OFFSET  (16)
#define ATCSPI200_ST_TXFVE_DEFAULT  (0)
#define ATCSPI200_ST_RXFFL_MASK  (BIT_MASK(15, 15))
#define ATCSPI200_ST_RXFFL_OFFSET  (15)
#define ATCSPI200_ST_RXFFL_DEFAULT  (0)
#define ATCSPI200_ST_RXFEM_MASK  (BIT_MASK(14, 14))
#define ATCSPI200_ST_RXFEM_OFFSET  (14)
#define ATCSPI200_ST_RXFEM_DEFAULT  (1)
#define ATCSPI200_ST_RXFVE_MASK  (BIT_MASK(12, 8))
#define ATCSPI200_ST_RXFVE_OFFSET  (8)
#define ATCSPI200_ST_RXFVE_DEFAULT  (0)
#define ATCSPI200_ST_SPIBSY_MASK  (BIT_MASK(0, 0))
#define ATCSPI200_ST_SPIBSY_OFFSET  (0)
#define ATCSPI200_ST_SPIBSY_DEFAULT  (0)
#define ATCSPI200_ST_DEFAULT  ( \
		ATCSPI200_DEFAULT(ST, TXFFL) |\
		ATCSPI200_DEFAULT(ST, TXFEM) |\
		ATCSPI200_DEFAULT(ST, TXFVE) |\
		ATCSPI200_DEFAULT(ST, RXFFL) |\
		ATCSPI200_DEFAULT(ST, RXFEM) |\
		ATCSPI200_DEFAULT(ST, RXFVE) |\
		ATCSPI200_DEFAULT(ST, SPIBSY) \
		)

// SPI Interrupt Enable Register
#define ATCSPI200_INTEN_SLVCMDEN_MASK  (BIT_MASK(5, 5))
#define ATCSPI200_INTEN_SLVCMDEN_OFFSET  (5)
#define ATCSPI200_INTEN_SLVCMDEN_DEFAULT  (0)
#define ATCSPI200_INTEN_SPIEIE_MASK  (BIT_MASK(4, 4))
#define ATCSPI200_INTEN_SPIEIE_OFFSET  (4)
#define ATCSPI200_INTEN_SPIEIE_DEFAULT  (0)
#define ATCSPI200_INTEN_TXFTHIE_MASK  (BIT_MASK(3, 3))
#define ATCSPI200_INTEN_TXFTHIE_OFFSET  (3)
#define ATCSPI200_INTEN_TXFTHIE_DEFAULT  (0)
#define ATCSPI200_INTEN_RXFTHIE_MASK  (BIT_MASK(2, 2))
#define ATCSPI200_INTEN_RXFTHIE_OFFSET  (2)
#define ATCSPI200_INTEN_RXFTHIE_DEFAULT  (0)
#define ATCSPI200_INTEN_TXFURIE_MASK  (BIT_MASK(1, 1))
#define ATCSPI200_INTEN_TXFURIE_OFFSET  (1)
#define ATCSPI200_INTEN_TXFURIE_DEFAULT  (0)
#define ATCSPI200_INTEN_RXFORIE_MASK  (BIT_MASK(0, 0))
#define ATCSPI200_INTEN_RXFORIE_OFFSET  (0)
#define ATCSPI200_INTEN_RXFORIE_DEFAULT  (0)
#define ATCSPI200_INTEN_DEFAULT  ( \
		ATCSPI200_DEFAULT(INTEN, SLVCMDEN) |\
		ATCSPI200_DEFAULT(INTEN, SPIEIE) |\
		ATCSPI200_DEFAULT(INTEN, TXFTHIE) |\
		ATCSPI200_DEFAULT(INTEN, RXFTHIE) |\
		ATCSPI200_DEFAULT(INTEN, TXFURIE) |\
		ATCSPI200_DEFAULT(INTEN, RXFORIE) \
		)

// SPI Interrupt Status Register
#define ATCSPI200_INTST_SLVCMD_MASK  (BIT_MASK(5, 5))
#define ATCSPI200_INTST_SLVCMD_OFFSET  (5)
#define ATCSPI200_INTST_SLVCMD_DEFAULT  (0)
#define ATCSPI200_INTST_SPIEI_MASK  (BIT_MASK(4, 4))
#define ATCSPI200_INTST_SPIEI_OFFSET  (4)
#define ATCSPI200_INTST_SPIEI_DEFAULT  (0)
#define ATCSPI200_INTST_TXFTHI_MASK  (BIT_MASK(3, 3))
#define ATCSPI200_INTST_TXFTHI_OFFSET  (3)
#define ATCSPI200_INTST_TXFTHI_DEFAULT  (0)
#define ATCSPI200_INTST_RXFTHI_MASK  (BIT_MASK(2, 2))
#define ATCSPI200_INTST_RXFTHI_OFFSET  (2)
#define ATCSPI200_INTST_RXFTHI_DEFAULT  (0)
#define ATCSPI200_INTST_TXFURI_MASK  (BIT_MASK(1, 1))
#define ATCSPI200_INTST_TXFURI_OFFSET  (1)
#define ATCSPI200_INTST_TXFURI_DEFAULT  (0)
#define ATCSPI200_INTST_RXFORI_MASK  (BIT_MASK(0, 0))
#define ATCSPI200_INTST_RXFORI_OFFSET  (0)
#define ATCSPI200_INTST_RXFORI_DEFAULT  (0)
#define ATCSPI200_INTST_DEFAULT  ( \
		ATCSPI200_DEFAULT(INTST, SLVCMD) |\
		ATCSPI200_DEFAULT(INTST, SPIEI) |\
		ATCSPI200_DEFAULT(INTST, TXFTHI) |\
		ATCSPI200_DEFAULT(INTST, RXFTHI) |\
		ATCSPI200_DEFAULT(INTST, TXFURI) |\
		ATCSPI200_DEFAULT(INTST, RXFORI) \
		)


// REG SPI Interface timing Setting
#define ATCSPI200_REGTIMING_CS_CLK_MASK  (BIT_MASK(13, 12))
#define ATCSPI200_REGTIMING_CS_CLK_OFFSET  (12)
#define ATCSPI200_REGTIMING_CS_CLK_DEFAULT  (0)
#define ATCSPI200_REGTIMING_CSHT_MASK  (BIT_MASK(11, 8))
#define ATCSPI200_REGTIMING_CSHT_OFFSET  (8)
#define ATCSPI200_REGTIMING_CSHT_DEFAULT  (0x2)
#define ATCSPI200_REGTIMING_SCLK_DIV_MASK  (BIT_MASK(7, 0))
#define ATCSPI200_REGTIMING_SCLK_DIV_OFFSET  (0)
#define ATCSPI200_REGTIMING_SCLK_DIV_DEFAULT  (0x1)
#define ATCSPI200_REGTIMING_DEFAULT  ( \
		ATCSPI200_DEFAULT(REGTIMING, CS_CLK) |\
		ATCSPI200_DEFAULT(REGTIMING, CSHT) |\
		ATCSPI200_DEFAULT(REGTIMING, SCLK_DIV) \
		)


// Memory SPI DAta Control and Interface Timing
#define ATCSPI200_MEMDCTRL_CHG_STA_MASK  (BIT_MASK(8, 8))
#define ATCSPI200_MEMDCTRL_CHG_STA_OFFSET  (8)
#define ATCSPI200_MEMDCTRL_CHG_STA_DEFAULT  (0)
#define ATCSPI200_MEMDCTRL_CMD_R_MASK  (BIT_MASK(2, 0))
#define ATCSPI200_MEMDCTRL_CMD_R_OFFSET  (0)
#define ATCSPI200_MEMDCTRL_CMD_R_DEFAULT  (0x0)
#define ATCSPI200_MEMDCTRL_DEFAULT  ( \
		ATCSPI200_DEFAULT(MEMDCTRL, CHG_STA) |\
		ATCSPI200_DEFAULT(MEMDCTRL, CMD_R) \
		)

// The Slave mode status (Offset +0x60)
#define ATCSPI200_SLVST_DUR_MASK	(BIT_MASK(18, 18))
#define ATCSPI200_SLVST_DUR_OFFSET	(18)
#define ATCSPI200_SLVST_DUR_DEFAULT	(0)
#define ATCSPI200_SLVST_DOR_MASK	(BIT_MASK(17, 17))
#define ATCSPI200_SLVST_DOR_OFFSET	(17)
#define ATCSPI200_SLVST_DOR_DEFAULT	(0)
#define ATCSPI200_SLVST_READY_MASK	(BIT_MASK(16, 16))
#define ATCSPI200_SLVST_READY_OFFSET	(16)
#define ATCSPI200_SLVST_READY_DEFAULT	(0)
#define ATCSPI200_SLVST_UR_Status_MASK	(BIT_MASK(15, 0))
#define ATCSPI200_SLVST_UR_Status_OFFSET	(0)
#define ATCSPI200_SLVST_UR_Status_DEFAULT	(0)
#define ATCSPI200_SLVST_DEFAULT ( \
		ATCSPI200_DEFAULT(SLVST, DUR) |\
		ATCSPI200_DEFAULT(SLVST, DOR) |\
		ATCSPI200_DEFAULT(SLVST, READY) |\
		ATCSPI200_DEFAULT(SLVST, UR_Status)\
		)

// The Slave Read/Write data counter (Offset +0x64)
#define ATCSPI200_SLVCNT_W_VLD_MASK	(BIT_MASK(31, 31))
#define ATCSPI200_SLVCNT_W_VLD_OFFSET	(31)
#define ATCSPI200_SLVCNT_W_VLD_DEFAULT	(0)
#define ATCSPI200_SLVCNT_W_CNT_MASK	(BIT_MASK(24, 16))
#define ATCSPI200_SLVCNT_W_CNT_OFFSET	(16)
#define ATCSPI200_SLVCNT_W_CNT_DEFAULT	(0)
#define ATCSPI200_SLVCNT_R_VLD_MASK	(BIT_MASK(15, 15))
#define ATCSPI200_SLVCNT_R_VLD_OFFSET	(15)
#define ATCSPI200_SLVCNT_R_VLD_DEFAULT	(0)
#define ATCSPI200_SLVCNT_R_CNT_MASK	(BIT_MASK(8, 0))
#define ATCSPI200_SLVCNT_R_CNT_OFFSET	(0)
#define ATCSPI200_SLVCNT_R_CNT_DEFAULT	(0)
#define ATCSPI200_SLVCNT_DEFAULT ( \
		ATCSPI200_DEFAULT(SLVCNT, W_VLD) |\
		ATCSPI200_DEFAULT(SLVCNT, W_CNT) |\
		ATCSPI200_DEFAULT(SLVCNT, R_VLD) |\
		ATCSPI200_DEFAULT(SLVCNT, R_CNT) \
		)

// Hardware Configured Status
#define ATCSPI200_HWCFG_SLAVE_SP_MASK  (BIT_MASK(14, 14))
#define ATCSPI200_HWCFG_SLAVE_SP_OFFSET  (14)
#define ATCSPI200_HWCFG_SLAVE_SP_DEFAULT  (0)
#define ATCSPI200_HWCFG_EILM_MEM_SP_MASK  (BIT_MASK(13, 13))
#define ATCSPI200_HWCFG_EILM_MEM_SP_OFFSET  (13)
#define ATCSPI200_HWCFG_EILM_MEM_SP_DEFAULT  (0)
#define ATCSPI200_HWCFG_AHB_MEM_SP_MASK  (BIT_MASK(12, 12))
#define ATCSPI200_HWCFG_AHB_MEM_SP_OFFSET  (12)
#define ATCSPI200_HWCFG_AHB_MEM_SP_DEFAULT  (0)
#define ATCSPI200_HWCFG_PIO_SP_MASK  (BIT_MASK(11, 11))
#define ATCSPI200_HWCFG_PIO_SP_OFFSET  (11)
#define ATCSPI200_HWCFG_PIO_SP_DEFAULT  (0)
#define ATCSPI200_HWCFG_SCLK_1T_MASK  (BIT_MASK(10, 10))
#define ATCSPI200_HWCFG_SCLK_1T_OFFSET  (10)
#define ATCSPI200_HWCFG_SCLK_1T_DEFAULT  (1)
#define ATCSPI200_HWCFG_QuadSPI_MASK  (BIT_MASK(9, 9))
#define ATCSPI200_HWCFG_QuadSPI_OFFSET  (9)
#define ATCSPI200_HWCFG_QuadSPI_DEFAULT  (1)
#define ATCSPI200_HWCFG_DualSPI_MASK  (BIT_MASK(8, 8))
#define ATCSPI200_HWCFG_DualSPI_OFFSET  (8)
#define ATCSPI200_HWCFG_DualSPI_DEFAULT  (1)
#define ATCSPI200_HWCFG_TxFIFO_SIZE_MASK  (BIT_MASK(5, 4))
#define ATCSPI200_HWCFG_TxFIFO_SIZE_OFFSET  (4)
#define ATCSPI200_HWCFG_TxFIFO_SIZE_DEFAULT  (0x1)
#define ATCSPI200_HWCFG_RxFIFO_SIZE_MASK  (BIT_MASK(1, 0))
#define ATCSPI200_HWCFG_RxFIFO_SIZE_OFFSET  (0)
#define ATCSPI200_HWCFG_RxFIFO_SIZE_DEFAULT  (0x1)
#define ATCSPI200_HWCFG_DEFAULT  ( \
		ATCSPI200_DEFAULT(HWCFG, SLAVE_SP) |\
		ATCSPI200_DEFAULT(HWCFG, PIO_SP) |\
		ATCSPI200_DEFAULT(HWCFG, SCLK_1T) |\
		ATCSPI200_DEFAULT(HWCFG, QuadSPI) |\
		ATCSPI200_DEFAULT(HWCFG, DualSPI) |\
		ATCSPI200_DEFAULT(HWCFG, TxFIFO_SIZE) |\
		ATCSPI200_DEFAULT(HWCFG, RxFIFO_SIZE) \
		)

// ======================================================
// ATCSPI200 register definition
// ======================================================
// ATCSPI200 registers
typedef struct atcspi200_reg_t {
	volatile UINT32 Ver;            // 0x00 ID and Revision Register
	volatile UINT32 Reserved_04;
	volatile UINT32 Reserved_08;
	volatile UINT32 Reserved_0C;
	volatile UINT32 IfSet;          // 0x10 SPI Interface Format Setting
	volatile UINT32 PIO;            // 0x14 SPI PIO Register
	volatile UINT32 ReqDelay;       //0x18  SPI transimiited req delay, added in AthenaLite iot
	volatile UINT32 Reserved_1C;
	volatile UINT32 RegDCtrl;       // 0x20 REG SPI Data Control Register
	volatile UINT32 RegCPort;       // 0x24 REG SPI Command Port Register
	volatile UINT32 RegAPort;       // 0x28 REG SPI Address Port Register
	volatile UINT32 RegDPort;       // 0x2C REG SPI Date Port Register
	volatile UINT32 Ctrl;           // 0x30 SPI Control Register
	volatile UINT32 St;             // 0x34 SPI Status Register
	volatile UINT32 IntEn;          // 0x38 SPI Interrupt Enable Register
	volatile UINT32 IntSt;          // 0x3C SPI Interrupt Status Register
	volatile UINT32 RegTiming;      // 0x40 Reg SPI Interface timing Setting
	volatile UINT32 Reserved_44;
	volatile UINT32 Reserved_48;
	volatile UINT32 Reserved_4C;
	volatile UINT32 MemDCtrl;       // 0x50 Memory SPI Data control and Interface Timing
	volatile UINT32 Reserved_54;    //
	volatile UINT32 Reserved_58;
	volatile UINT32 Reserved_5C; 
	volatile UINT32 SlvSt;
	volatile UINT32 SlvCnt;
	volatile UINT32 Reserved_68;
	volatile UINT32 Reserved_6C;
	volatile UINT32 Reserved_70;
	volatile UINT32 Reserved_74;
	volatile UINT32 Reserved_78;
	volatile UINT32 HWCfg;          // 0x7C Hardware Configured Status
} atcspi200_reg_s, *atcspi200_reg_p;





// ======================================================
// ATCSPI200 access macro
// ======================================================
#define ATCSPI200_GET_FIELD(var, reg, field)		GET_FIELD(var, ATCSPI200_##reg##_##field##_##MASK, ATCSPI200_##reg##_##field##_##OFFSET)
#define ATCSPI200_DEFAULT(reg, field)				PREPARE_FIELD(ATCSPI200_##reg##_##field##_##DEFAULT, ATCSPI200_##reg##_##field##_##MASK, ATCSPI200_##reg##_##field##_##OFFSET )
#define ATCSPI200_PREPARE(reg, field, value)		PREPARE_FIELD(value, ATCSPI200_##reg##_##field##_##MASK, ATCSPI200_##reg##_##field##_##OFFSET )

int flash_read(UINT32 addr,UINT32 *rbuf,UINT32 rcnt);
int flash_fast_read(UINT32 addr,UINT32 *rbuf,UINT32 rcnt);
int flash_read_info(UINT32 *info);
int flash_read_id(UINT32 *id);
int flash_write_enable();
int flash_write_disable();
int flash_check_wip();
#if (CHIP_TYPE==CHIP_ARES_B)
int flash_chip_deepSleep();
#endif
int flash_chip_erase();
int flash_sector_erase(UINT32 addr);
int flash_block_erase(UINT32 addr);
int flash_page_program(UINT32 addr,UINT32 *wbuf,UINT32 wcnt);
int flash_write_status_register(UINT32 *wr_status);
int flash_read_status_register(UINT32 *rd_status);
int flash_init(void);

int spi_exe_cmd(
		UINT8 cmd,
		UINT32 addr,
		UINT32 rcnt, UINT32 *rbuf,
		UINT32 wcnt, UINT32 *wbuf);
int spi_init(void);


int spi_maxim_init(void);
int spi_maxim_write(UINT8 addr,UINT16 rfdata);


int spi_flash_test(void);

#endif //__FLASH_H_
