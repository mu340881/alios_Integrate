/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <string.h>
#include <aos/aos.h>
#include "iot_import.h"
#include "ali_crypto.h"

#if defined(INFRA_AES)
    #include "infra_aes.h"
#else
    #include "mbedtls/aes.h"
#endif

typedef struct {
#if defined(INFRA_AES)
    infra_aes_context ctx;
#else
    mbedtls_aes_context ctx;
#endif
    uint8_t iv[16];
    uint8_t key[16];
} platform_aes_t;

p_HAL_Aes128_t HAL_Aes128_Init(
            const uint8_t *key,
            const uint8_t *iv,
            AES_DIR_t dir)
{
    int ret = 0;
    platform_aes_t *p_aes128 = NULL;

    if (!key || !iv) return p_aes128;

    p_aes128 = (platform_aes_t *)HAL_Malloc(sizeof(platform_aes_t));
    if (!p_aes128) return p_aes128;
    memset(p_aes128, 0, sizeof(platform_aes_t));

#if defined(INFRA_AES)
    infra_aes_init(&p_aes128->ctx);
    if (dir == 0) {
        ret = infra_aes_setkey_enc(&p_aes128->ctx, key, 128);
    } else {
        ret = infra_aes_setkey_dec(&p_aes128->ctx, key, 128);
    }
#else
    mbedtls_aes_init(&p_aes128->ctx);
    if (dir == 0) {
        ret = mbedtls_aes_setkey_enc(&p_aes128->ctx, key, 128);
    } else {
        ret = mbedtls_aes_setkey_dec(&p_aes128->ctx, key, 128);
    }
#endif

    if (ret == 0) {
        memcpy(p_aes128->iv, iv, 16);
        memcpy(p_aes128->key, key, 16);
    } else {
       HAL_Free(p_aes128);
        p_aes128 = NULL;
    }

    return (p_HAL_Aes128_t) p_aes128;
}

int HAL_Aes128_Destroy(p_HAL_Aes128_t aes)
{
    if (!aes) return -1;


#if defined(INFRA_AES)
    infra_aes_free(&((platform_aes_t *)aes)->ctx);
#else
    mbedtls_aes_free(&((platform_aes_t *)aes)->ctx);
#endif

   HAL_Free(aes);
    return 0;
}

int HAL_Aes128_Cbc_Decrypt(
            p_HAL_Aes128_t aes,
            const void *src,
            size_t blockNum,
            void *dst)
{
    int i   = 0;
    int ret = -1;
    platform_aes_t *p_aes128 = (platform_aes_t *)aes;

    if (!aes || !src || !dst) return ret;

    for (i = 0; i < blockNum; ++i) {
#if defined(INFRA_AES)
        ret = infra_aes_crypt_cbc(&p_aes128->ctx, INFRA_AES_DECRYPT, AES_BLOCK_SIZE,
                                    p_aes128->iv, src, dst);
#else
        ret = mbedtls_aes_crypt_cbc(&p_aes128->ctx, MBEDTLS_AES_DECRYPT, AES_BLOCK_SIZE,
                                    p_aes128->iv, src, dst);
#endif
        src += 16;
        dst += 16;
    }

    return ret;
}

int HAL_Aes128_Cfb_Decrypt(
            p_HAL_Aes128_t aes,
            const void *src,
            size_t length,
            void *dst)
{
    size_t offset = 0;
    int ret = -1;
    platform_aes_t *p_aes128 = (platform_aes_t *)aes;

    if (!aes || !src || !dst) return ret;

#if defined(INFRA_AES)
    ret = infra_aes_setkey_enc(&p_aes128->ctx, p_aes128->key, 128);
    ret = infra_aes_crypt_cfb128(&p_aes128->ctx, INFRA_AES_DECRYPT, length,
                                   &offset, p_aes128->iv, src, dst);
#else
    ret = mbedtls_aes_setkey_enc(&p_aes128->ctx, p_aes128->key, 128);
    ret = mbedtls_aes_crypt_cfb128(&p_aes128->ctx, MBEDTLS_AES_DECRYPT, length,
                                   &offset, p_aes128->iv, src, dst);
#endif

    return ret;
}

int HAL_Aes128_Cfb_Encrypt(
            p_HAL_Aes128_t aes,
            const void *src,
            size_t length,
            void *dst)
{
    size_t offset = 0;
    int ret = -1;
    platform_aes_t *p_aes128 = (platform_aes_t *)aes;

    if (!aes || !src || !dst) return ret;

#if defined(INFRA_AES)
    ret = infra_aes_crypt_cfb128(&p_aes128->ctx, INFRA_AES_ENCRYPT, length,
                                   &offset, p_aes128->iv, src, dst);
#else
    ret = mbedtls_aes_crypt_cfb128(&p_aes128->ctx, MBEDTLS_AES_ENCRYPT, length,
                                   &offset, p_aes128->iv, src, dst);
#endif
    return ret;
}

int HAL_Aes128_Cbc_Encrypt(
            p_HAL_Aes128_t aes,
            const void *src,
            size_t blockNum,
            void *dst)
{
    int i   = 0;
    int ret = ret;
    platform_aes_t *p_aes128 = (platform_aes_t *)aes;

    if (!aes || !src || !dst) return -1;

    for (i = 0; i < blockNum; ++i) {
#if defined(INFRA_AES)
        ret = infra_aes_crypt_cbc(&p_aes128->ctx, INFRA_AES_ENCRYPT, AES_BLOCK_SIZE,
                                    p_aes128->iv, src, dst);
#else
        ret = mbedtls_aes_crypt_cbc(&p_aes128->ctx, MBEDTLS_AES_ENCRYPT, AES_BLOCK_SIZE,
                                    p_aes128->iv, src, dst);
#endif
        src += 16;
        dst += 16;
    }

    return ret;
}

//#endif
