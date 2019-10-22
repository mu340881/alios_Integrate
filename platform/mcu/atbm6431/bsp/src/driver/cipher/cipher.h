/**************************************************************************************************************
 * altobeam iot hw cipher file 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 * Permission is hereby granted for use on Altobeam IOT 6431 only,
 * in which case ,it is free to  redistribute it and/or modify
 *
*****************************************************************************************************************/


#ifndef ATBM_CHIPER_HW_H
#define ATBM_CHIPER_HW_H

struct aes_struct_hw{
	short keybits;
	char  is_enc;
	char  reverd;
	char key[0];
}__attribute__((packed));

struct ck_struct_hw{
	short key_len;
	char  is_enc;
	char  reverd;
	char key[0];
}__attribute__((packed));

enum ATBM_CHIPER_MODE{
	ATBM_CHIPER_MODE_ECB=0,
	ATBM_CHIPER_MODE_CBC=1,
	ATBM_CHIPER_MODE_CFB=2,
	ATBM_CHIPER_MODE_OFB=3,
	ATBM_CHIPER_MODE_CTR=4
};

enum ATBM_CHIPER_ALNOTYPE{
	ATBM_CHIPER_TYPE_DES=0,
	ATBM_CHIPER_TYPE_3DES128,
	ATBM_CHIPER_TYPE_3DES192,
	ATBM_CHIPER_TYPE_AES128,
	ATBM_CHIPER_TYPE_AES192,
	ATBM_CHIPER_TYPE_AES256
};


/*. 0:8bits,1:16bits,2:32bits,3:64bits,4:128bits. */
enum ATBM_CHIPER_CFBMODE{
	ATBM_CFB_TYPE_8=0,
	ATBM_CFB_TYPE_16,
	ATBM_CFB_TYPE_32,
	ATBM_CFB_TYPE_64,
	ATBM_CFB_TYPE_128
};




#define AES_IV_ADDRESS			0x17200008
#define AES_KEY_ADDRESS			0x1720000c
#define AES_DATA_INPUT_ADDRESS		0x17200010
#define AES_DATA_SIZE_ADDRESS		0x17200004  //bit[19:0]
#define AES_DATA_OUTPUT_ADDRESS		0x17200014


#define AES_CTL_CFG_ADDRESS             0x17200000
#define AES_DONE_QUERY_ADDRESS          0x17200134
#if (CHIP_TYPE==CHIP_ARES_B)
void ATBM_init_hardware_hash_cipher();
void ATBM_set_hardware(unsigned int alno,unsigned int mode,unsigned char *key, unsigned int keyLen,  unsigned char *iv,  unsigned char *sbuf, unsigned char *dbuf, unsigned int len,int is_encrypt,int cfb_mode);

#endif

#endif //ATBM_CHIPER_HW_H
