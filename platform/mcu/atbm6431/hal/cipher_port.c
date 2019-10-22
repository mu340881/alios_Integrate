
#include "atbm_hal.h"
#include "src/driver/cipher/cipher.h"


typedef struct {
    int nr;                     /*!<  number of rounds  */
    uint32_t *rk;               /*!<  AES round keys    */
    uint32_t buf[68];           /*!<  unaligned data    */
}
mbedtls_aes_context;

int mbedtls_aes_setkey_enc( mbedtls_aes_context *ctx, const unsigned char *key,
                    unsigned int keybits )
{   
    switch( keybits )
    {
        case 128: ctx->nr = 128; break;
        case 192: ctx->nr = 192; break;
        case 256: ctx->nr = 256; break;
        default : return( -1 );
    }

    memcpy(ctx->buf,key,keybits/8);
	return 0;

}

int mbedtls_aes_setkey_dec( mbedtls_aes_context *ctx, const unsigned char *key,
                    unsigned int keybits )
{
    switch( keybits )
    {
        case 128: ctx->nr = 128; break;
        case 192: ctx->nr = 192; break;
        case 256: ctx->nr = 256; break;
        default : return( -1 );
    }

    memcpy(ctx->buf,key,keybits/8);

	return 0;
}

FLASH_FUNC void mbedtls_aes_decrypt( mbedtls_aes_context *ctx,
                          const unsigned char input[16],
                          unsigned char output[16] )
{
	 unsigned int alno = ATBM_CHIPER_TYPE_AES128;
	 char testbuf[16];
 
	 if(ctx->nr==128)
		 alno= ATBM_CHIPER_TYPE_AES128;
	 else if(ctx->nr==192)
		 alno= ATBM_CHIPER_TYPE_AES192;
	 else if(ctx->nr==256)
		 alno= ATBM_CHIPER_TYPE_AES256;  
	 
	 memcpy(testbuf,input,16);
	 ATBM_set_hardware(alno,ATBM_CHIPER_MODE_ECB,(unsigned char *)ctx->buf,(unsigned int)(ctx->nr/8),NULL,(unsigned char *)testbuf,(unsigned char *)testbuf,16,0,0);
	 memcpy(output,testbuf,16);	 
	 
}

FLASH_FUNC void mbedtls_aes_encrypt( mbedtls_aes_context *ctx,
                          const unsigned char input[16],
                          unsigned char output[16] )
{
	unsigned int alno = ATBM_CHIPER_TYPE_AES128;
	char testbuf[16];
	
	if(ctx->nr==128)
		alno= ATBM_CHIPER_TYPE_AES128;
	else if(ctx->nr==192)
		alno= ATBM_CHIPER_TYPE_AES192;
	else if(ctx->nr==256)
		alno= ATBM_CHIPER_TYPE_AES256;
	
	memcpy(testbuf,input,16);
	ATBM_set_hardware(alno,ATBM_CHIPER_MODE_ECB,(unsigned char *)ctx->buf,(unsigned int)(ctx->nr/8),NULL,(unsigned char *)testbuf,(unsigned char *)testbuf,16,1,0);
	memcpy(output,testbuf,16); 

}

