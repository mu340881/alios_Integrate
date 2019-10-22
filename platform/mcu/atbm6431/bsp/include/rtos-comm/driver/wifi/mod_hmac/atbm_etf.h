#ifndef _ATBM_ETF_H_
#define _ATBM_ETF_H_

struct rxstatus{
	u32 GainImb;
	u32 PhaseImb;
	u32 Cfo;
	u32 evm;
	u32  RSSI;
	u32 probcnt;
};

struct rxstatus_signed{
	u8 valid;
	s32 GainImb;
	s32 PhaseImb;
	s32 Cfo;
	s32 evm;// evm:rx evm
	s32 txevm;
	s32  RxRSSI;
	s32  TxRSSI;
	u16 dcxo;
	s32 result;
};

#define N_BIT_TO_SIGNED_32BIT(v,n)	(s32)(((v) & BIT(n-1))?((v)|0xffffffff<<n):(v))


#define ATBM_IOT_MP_MAGIC_DATA			(0x5a5b5c5d)
#define ATBM_IOT_MP_RECHECK_KEY_FLAG			(0x6a6b6c6d) //recheck key flag

#define ATBM_IOT_MP_CHIP_VERSION_BASE (0x0acc017c)

#define ATBM_IOT_MP_BASEADDR		(SRAM_ADDRESS + SRAM_LEN -64)
#define ATBM_IOT_MP_START_FLAG		(ATBM_IOT_MP_BASEADDR)
#define ATBM_IOT_MP_WEFUSE_FLAG		(ATBM_IOT_MP_BASEADDR+4)
#define ATBM_IOT_MP_RESULT			(ATBM_IOT_MP_BASEADDR+8)
//recheck key macro
#define ATBM_IOT_MP_RECHECK_KEY_DATA			(ATBM_IOT_MP_BASEADDR - 512)
#define ATBM_IOT_MP_RECHECK_KEY_CHECKSUM			(ATBM_IOT_MP_BASEADDR - 512 -4)
#define ATBM_IOT_MP_RECHECK_KEY_END_FLAG			(ATBM_IOT_MP_BASEADDR - 512 -8)
#define ATBM_IOT_MP_RECHECK_KEY_FLAG_ADDR			(ATBM_IOT_MP_BASEADDR - 4)





#define ATBM_IOT_KEY_CHECKSUM_BASEADDR	(ATBM_IOT_MP_BASEADDR - 512 - 4) //key checksum
#define ATBM_IOT_KEY_BASEADDR		(ATBM_IOT_MP_BASEADDR - 512) //key data 
#define ATBM_IOT_KEY_FLAG_BASEADDR	(ATBM_IOT_KEY_CHECKSUM_BASEADDR - 4) //write key flag
#define ATBM_IOT_KEY_WRITE_SUCCESS_OR_FAIL	(ATBM_IOT_KEY_CHECKSUM_BASEADDR - 8) //write key success or fail flag
#define ATBM_IOT_KEY_FLAG_DATA       0x12341234

#define DCXO_CODE_MINI		0//24//0
#define DCXO_CODE_MAX		127//38//63
#define TARGET_FREQOFFSET_HZ  (7000)
#define SCAN_PACKET_NUM 8

extern PUBLIC int atbm_iot_mp_start(u32 bWriteEfuse);
extern void atbm_iot_mp_test_enable();
extern void AT_Mp_ETF(char *pLine);
extern void AT_WifiETF_ReadMemory(char *arg);
extern void AT_WifiETF_Cfg_Filter(char *pLine);
extern void AT_WifiETF_Save_Efuse(char *pLine);
extern FLASH_FUNC void AT_WifiETF_Get_Efuse(char *pLine);
extern FLASH_FUNC void AT_WifiETF_Write_User_Key(char *pLine);
extern FLASH_FUNC void AT_WifiETF_Key_Recheck(char *pLine);
extern FLASH_FUNC void AT_WifiETF_Get_Mac(char *pLine);

#endif
