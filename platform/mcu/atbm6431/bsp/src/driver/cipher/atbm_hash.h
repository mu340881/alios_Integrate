#ifndef ATBM_AES_H_
#define ATBM_AES_H_
#include "stdint.h"
#define HASH_STATE_BYTE_TEST 0	//0: STATE in int(normal case),   1: sha state byte align, may not be used, just support it.

/**********************************************
header file of sha1 & sha256, md5,
1, provide hardware hash core function interface.
2, provide mbedtls compatible hash functions,
   here reused mbedTLS hash functions, just changed name by adding _compatible_, so the following API can be used without mbedtls LIB compiled in.
*********************************************************************************************************/
#if defined(WIN32) && !defined(__cplusplus)
#define inline __inline
#endif

#define MAX_TEST_HASH_SIZE  1600
#define MAX_TEST_MEMORY_SIZE  (MAX_TEST_HASH_SIZE + 4)


#define HASH_MAC_DATA_SIZE_ADDRESS  0x1720001C    //[19:0]
#define HASH_MAC_DATA_INPUT_ADDRESS 0x17200020    //[31:0]
#define HASH_MAC_OUTPUT_ADDRESS     0x17200024     //[31:0]
#define HASH_MAC_STATE_ADDRESS      0x1720002C    //[31:0]

//加密enable：
//Bit[0]. 1:开始信号
//Bit[3:1]: 0:Sha1,1:sha256,2:MD5
//bit[4], 1:interrupt enable,0:disable
#define HASH_MAC_CTL_CFG_ADDRESS  0x17200018

//反复读取17200028,读到1即代表完成。可以取走数据，读完后，配置17200018[0]为0
//若为中断模式，在中断服务函数读取数据，读完后，配置17200018[0]为0，清中断。
#define HASH_MAC_DONE_QUERY_ADDRESS  0x17200028

typedef enum{
	ATBM_HMAC_SHA1 = 0,
	ATBM_HMAC_SHA256,
	ATBM_HMAC_MD5,
	ATBM_HMAC_UNKNOWN
}eATBM_HASH_MAC_TYPE;


void ATBM_hash_mac_get_status();    /*max tested counter is read 200 times for 1500bytes, so default set 10000 is enough.*/
void ATBM_hash_mac_clear_status(eATBM_HASH_MAC_TYPE etype);
void ATBM_hash_mac_set_hardware(const unsigned char *data, unsigned int len, unsigned char *stateAddr, eATBM_HASH_MAC_TYPE etype );



/**************************************************
	sha256 context and APIs
**************************************************/
//typedef unsigned int uint32_t;
/**
 * \brief          The SHA-256 context structure.
 *
 *                 The structure is used both for SHA-256 and for SHA-224
 *                 checksum calculations. The choice between these two is
 *                 made in the call to mbedtls_compatible_sha256_starts_ret().
 */
typedef struct
{
    uint32_t total[2];          /*!< The number of Bytes processed.  */
    uint32_t state[8];          /*!< The intermediate digest state.  */
    unsigned char buffer[64];   /*!< The data block being processed. */
    int is224;                  /*!< Determines which function to use.
                                     <ul><li>0: Use SHA-256.</li>
                                     <li>1: Use SHA-224.</li></ul> */
}mbedtls_compatible_sha256_context;

/**
 * \brief          This function initializes a SHA-256 context.
 *
 * \param ctx      The SHA-256 context to initialize.
 */
void mbedtls_compatible_sha256_init( mbedtls_compatible_sha256_context *ctx );
/**
 * \brief          This function clears a SHA-256 context.
 *
 * \param ctx      The SHA-256 context to clear.
 */
void mbedtls_compatible_sha256_free( mbedtls_compatible_sha256_context *ctx );

/**
 * \brief          This function starts a SHA-224 or SHA-256 checksum
 *                 calculation.
 *
 * \param ctx      The context to initialize.
 * \param is224    Determines which function to use.
 *                 <ul><li>0: Use SHA-256.</li>
 *                 <li>1: Use SHA-224.</li></ul>
 *
 * \return         \c 0 on success.
 */
int mbedtls_compatible_sha256_starts_ret( mbedtls_compatible_sha256_context *ctx, int is224 );
/**
 * \brief          This function feeds an input buffer into an ongoing
 *                 SHA-256 checksum calculation.
 *
 * \param ctx      SHA-256 context
 * \param input    buffer holding the data
 * \param ilen     length of the input data
 *
 * \return         \c 0 on success.
 */
int mbedtls_compatible_sha256_update_ret( mbedtls_compatible_sha256_context *ctx,  const unsigned char *input, unsigned int ilen);

/**
 * \brief          This function finishes the SHA-256 operation, and writes
 *                 the result to the output buffer.
 *
 * \param ctx      The SHA-256 context.
 * \param output   The SHA-224 or SHA-256 checksum result.
 *
 * \return         \c 0 on success.
 */
int mbedtls_compatible_sha256_finish_ret( mbedtls_compatible_sha256_context *ctx, unsigned char output[32] );

/**
 * \brief          This function calculates the SHA-224 or SHA-256
 *                 checksum of a buffer.
 *
 *                 The function allocates the context, performs the
 *                 calculation, and frees the context.
 *
 *                 The SHA-256 result is calculated as
 *                 output = SHA-256(input buffer).
 *
 * \param input    The buffer holding the input data.
 * \param ilen     The length of the input data.
 * \param output   The SHA-224 or SHA-256 checksum result.
 * \param is224    Determines which function to use.
 *                 <ul><li>0: Use SHA-256.</li>
 *                 <li>1: Use SHA-224.</li></ul>
 */
int mbedtls_compatible_sha256_ret( const unsigned char *input,
                        unsigned int ilen,
                        unsigned char output[32],
                        int is224 );




/*The following sha256 API is not used in mbedtls2.7.0, be compatible for legacy code*/
 static inline void mbedtls_compatible_sha256_starts(
                                                mbedtls_compatible_sha256_context *ctx,
                                                int is224 )
{
    mbedtls_compatible_sha256_starts_ret( ctx, is224 );
}

static inline void mbedtls_compatible_sha256_update(
                                                mbedtls_compatible_sha256_context *ctx,
                                                const unsigned char *input,
                                                unsigned int ilen )
{
    mbedtls_compatible_sha256_update_ret( ctx, input, ilen );
}
static inline void mbedtls_compatible_sha256_finish(
                                                mbedtls_compatible_sha256_context *ctx,
                                                unsigned char output[32] )
{
    mbedtls_compatible_sha256_finish_ret( ctx, output );
}
static inline void mbedtls_compatible_sha256(
                                                    const unsigned char *input,
                                                    unsigned int ilen,
                                                    unsigned char output[32],
                                                    int is224 )
{
    mbedtls_compatible_sha256_ret( input, ilen, output, is224 );
}


int mbedtls_internal_sha256_process_big_block( mbedtls_compatible_sha256_context *ctx,
	const unsigned char *data, int length);

int mbedtls_compatible_sha256_big_block_ret( const unsigned char *input,
	unsigned int ilen,
	unsigned char output[32],
	int is224 );


/****************************

  SHA1

***************************/
typedef struct
{
	uint32_t total[2];          /*!< The number of Bytes processed.  */
	uint32_t state[5];          /*!< The intermediate digest state.  */
	unsigned char buffer[64];   /*!< The data block being processed. */
}
mbedtls_compatible_sha1_context;

/**
 * \brief          This function initializes a SHA-1 context.
 *
 * \param ctx      The SHA-1 context to initialize.
 *
 * \warning        SHA-1 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
void mbedtls_compatible_sha1_init( mbedtls_compatible_sha1_context *ctx );

/**
 * \brief          This function clears a SHA-1 context.
 *
 * \param ctx      The SHA-1 context to clear.
 *
 * \warning        SHA-1 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
void mbedtls_compatible_sha1_free( mbedtls_compatible_sha1_context *ctx );

/**
 * \brief          This function clones the state of a SHA-1 context.
 *
 * \param dst      The destination context.
 * \param src      The context to clone.
 *
 * \warning        SHA-1 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */


/**
 * \brief          This function starts a SHA-1 checksum calculation.
 *
 * \param ctx      The context to initialize.
 *
 * \return         \c 0 if successful
 *
 * \warning        SHA-1 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
int mbedtls_compatible_sha1_starts_ret( mbedtls_compatible_sha1_context *ctx );

/**
 * \brief          This function feeds an input buffer into an ongoing SHA-1
 *                 checksum calculation.
 *
 * \param ctx      The SHA-1 context.
 * \param input    The buffer holding the input data.
 * \param ilen     The length of the input data.
 *
 * \return         \c 0 if successful
 *
 * \warning        SHA-1 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
int mbedtls_compatible_sha1_update_ret( mbedtls_compatible_sha1_context *ctx,
                             const unsigned char *input,
                             unsigned int ilen );

/**
 * \brief          This function finishes the SHA-1 operation, and writes
 *                 the result to the output buffer.
 *
 * \param ctx      The SHA-1 context.
 * \param output   The SHA-1 checksum result.
 *
 * \return         \c 0 if successful
 *
 * \warning        SHA-1 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
int mbedtls_compatible_sha1_finish_ret( mbedtls_compatible_sha1_context *ctx,
                             unsigned char output[20] );


/**
 * \brief          This function calculates the SHA-1 checksum of a buffer.
 *
 *                 The function allocates the context, performs the
 *                 calculation, and frees the context.
 *
 *                 The SHA-1 result is calculated as
 *                 output = SHA-1(input buffer).
 *
 * \param input    The buffer holding the input data.
 * \param ilen     The length of the input data.
 * \param output   The SHA-1 checksum result.
 *
 * \return         \c 0 if successful
 *
 *
 */
int mbedtls_compatible_sha1_ret( const unsigned char *input,
                      unsigned int ilen,
                      unsigned char output[20] );


/*The following sha1 API is not used in mbedtls2.7.0, be compatible for legacy code*/
static inline void mbedtls_compatible_sha1_starts( mbedtls_compatible_sha1_context *ctx )
{
    mbedtls_compatible_sha1_starts_ret( ctx );
}
static inline void mbedtls_compatible_sha1_update(      mbedtls_compatible_sha1_context *ctx,
                                             const unsigned char *input,
                                             unsigned int ilen )
{
    mbedtls_compatible_sha1_update_ret( ctx, input, ilen );}

static inline void mbedtls_compatible_sha1_finish(
                                                mbedtls_compatible_sha1_context *ctx,
                                                unsigned char output[20] )
{
    mbedtls_compatible_sha1_finish_ret( ctx, output );
}
static inline void mbedtls_compatible_sha1( const unsigned char *input,
                                                    unsigned int ilen,
                                                    unsigned char output[20] )
{
    mbedtls_compatible_sha1_ret( input, ilen, output );
}



int mbedtls_internal_sha1_process_big_block( mbedtls_compatible_sha1_context *ctx,
	const unsigned char *data, int length);

static inline void mbedtls_compatible_sha1_big_block( const unsigned char *input,
	unsigned int ilen,
	unsigned char output[20] )
{
	mbedtls_compatible_sha1_ret( input, ilen, output );
}


/*************************************

MD5

***************************************/

typedef struct
{
	uint32_t total[2];          /*!< number of bytes processed  */
	uint32_t state[4];          /*!< intermediate digest state  */
	unsigned char buffer[64];   /*!< data block being processed */
}
mbedtls_compatible_md5_context;
void mbedtls_compatible_md5_init( mbedtls_compatible_md5_context *ctx );
void mbedtls_compatible_md5_free( mbedtls_compatible_md5_context *ctx );

int mbedtls_compatible_md5_starts_ret( mbedtls_compatible_md5_context *ctx );
int mbedtls_compatible_md5_update_ret( mbedtls_compatible_md5_context *ctx, const unsigned char *input,	unsigned int ilen );
int mbedtls_compatible_md5_finish_ret( mbedtls_compatible_md5_context *ctx,  unsigned char output[16] );
int mbedtls_compatible_md5_ret( const unsigned char *input,
					unsigned int ilen,unsigned char output[16] );


/*The following md5  API is not used in mbedtls2.7.0, be compatible for legacy code*/
static inline void mbedtls_compatible_md5_starts(
	mbedtls_compatible_md5_context *ctx )
{
	mbedtls_compatible_md5_starts_ret( ctx );
}
static inline void mbedtls_compatible_md5_update(
	mbedtls_compatible_md5_context *ctx,
	const unsigned char *input,
	unsigned int ilen )
{
	mbedtls_compatible_md5_update_ret( ctx, input, ilen );
}
static inline void mbedtls_compatible_md5_finish(
	mbedtls_compatible_md5_context *ctx,
	unsigned char output[16] )
{
	mbedtls_compatible_md5_finish_ret( ctx, output );
}
static inline void mbedtls_compatible_md5( const unsigned char *input,
					unsigned int ilen,	unsigned char output[16] )
{
	mbedtls_compatible_md5_ret( input, ilen, output );
}
void hash_test_main(void);
#endif
