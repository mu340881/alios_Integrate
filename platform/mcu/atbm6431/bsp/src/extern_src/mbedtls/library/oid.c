/**
 * \file oid.c
 *
 * \brief Object Identifier (OID) database
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_OID_C)

#include "mbedtls/oid.h"
#include "mbedtls/rsa.h"

#include <stdio.h>
#include <string.h>

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/mbedtls_platform.h"
#else
#define mbedtls_snprintf snprintf
#endif

#if defined(MBEDTLS_X509_USE_C) || defined(MBEDTLS_X509_CREATE_C)
#include "mbedtls/x509.h"
#endif

/*
 * Macro to automatically add the size of #define'd OIDs
 */
#define ADD_LEN(s)      s, MBEDTLS_OID_SIZE(s)

/*
 * Macro to generate an internal function for oid_XXX_from_asn1() (used by
 * the other functions)
 */
#define FN_OID_TYPED_FROM_ASN1( TYPE_T, NAME, LIST )                        \
FLASH_FUNC static const TYPE_T * oid_ ## NAME ## _from_asn1( const mbedtls_asn1_buf *oid )     \
{                                                                           \
    const TYPE_T *p = LIST;                                                 \
    const mbedtls_oid_descriptor_t *cur = (const mbedtls_oid_descriptor_t *) p;             \
    if( p == NULL || oid == NULL ) return( NULL );                          \
    while( cur->asn1 != NULL ) {                                            \
        if( cur->asn1_len == oid->len &&                                    \
            memcmp( cur->asn1, oid->p, oid->len ) == 0 ) {                  \
            return( p );                                                    \
        }                                                                   \
        p++;                                                                \
        cur = (const mbedtls_oid_descriptor_t *) p;                                 \
    }                                                                       \
    return( NULL );                                                         \
}

/*
 * Macro to generate a function for retrieving a single attribute from the
 * descriptor of an mbedtls_oid_descriptor_t wrapper.
 */
#define FN_OID_GET_DESCRIPTOR_ATTR1(FN_NAME, TYPE_T, TYPE_NAME, ATTR1_TYPE, ATTR1) \
FLASH_FUNC int FN_NAME( const mbedtls_asn1_buf *oid, ATTR1_TYPE * ATTR1 )                  \
{                                                                       \
    const TYPE_T *data = oid_ ## TYPE_NAME ## _from_asn1( oid );        \
    if( data == NULL ) return( MBEDTLS_ERR_OID_NOT_FOUND );            \
    *ATTR1 = data->descriptor.ATTR1;                                    \
    return( 0 );                                                        \
}

/*
 * Macro to generate a function for retrieving a single attribute from an
 * mbedtls_oid_descriptor_t wrapper.
 */
#define FN_OID_GET_ATTR1(FN_NAME, TYPE_T, TYPE_NAME, ATTR1_TYPE, ATTR1) \
FLASH_FUNC int FN_NAME( const mbedtls_asn1_buf *oid, ATTR1_TYPE * ATTR1 )                  \
{                                                                       \
    const TYPE_T *data = oid_ ## TYPE_NAME ## _from_asn1( oid );        \
    if( data == NULL ) return( MBEDTLS_ERR_OID_NOT_FOUND );            \
    *ATTR1 = data->ATTR1;                                               \
    return( 0 );                                                        \
}

/*
 * Macro to generate a function for retrieving two attributes from an
 * mbedtls_oid_descriptor_t wrapper.
 */
#define FN_OID_GET_ATTR2(FN_NAME, TYPE_T, TYPE_NAME, ATTR1_TYPE, ATTR1,     \
                         ATTR2_TYPE, ATTR2)                                 \
FLASH_FUNC int FN_NAME( const mbedtls_asn1_buf *oid, ATTR1_TYPE * ATTR1, ATTR2_TYPE * ATTR2 )  \
{                                                                           \
    const TYPE_T *data = oid_ ## TYPE_NAME ## _from_asn1( oid );            \
    if( data == NULL ) return( MBEDTLS_ERR_OID_NOT_FOUND );                \
    *ATTR1 = data->ATTR1;                                                   \
    *ATTR2 = data->ATTR2;                                                   \
    return( 0 );                                                            \
}

/*
 * Macro to generate a function for retrieving the OID based on a single
 * attribute from a mbedtls_oid_descriptor_t wrapper.
 */
#define FN_OID_GET_OID_BY_ATTR1(FN_NAME, TYPE_T, LIST, ATTR1_TYPE, ATTR1)   \
FLASH_FUNC int FN_NAME( ATTR1_TYPE ATTR1, const char **oid, size_t *olen )             \
{                                                                           \
    const TYPE_T *cur = LIST;                                               \
    while( cur->descriptor.asn1 != NULL ) {                                 \
        if( cur->ATTR1 == ATTR1 ) {                                         \
            *oid = cur->descriptor.asn1;                                    \
            *olen = cur->descriptor.asn1_len;                               \
            return( 0 );                                                    \
        }                                                                   \
        cur++;                                                              \
    }                                                                       \
    return( MBEDTLS_ERR_OID_NOT_FOUND );                                   \
}

/*
 * Macro to generate a function for retrieving the OID based on two
 * attributes from a mbedtls_oid_descriptor_t wrapper.
 */
#define FN_OID_GET_OID_BY_ATTR2(FN_NAME, TYPE_T, LIST, ATTR1_TYPE, ATTR1,   \
                                ATTR2_TYPE, ATTR2)                          \
FLASH_FUNC int FN_NAME( ATTR1_TYPE ATTR1, ATTR2_TYPE ATTR2, const char **oid ,         \
             size_t *olen )                                                 \
{                                                                           \
    const TYPE_T *cur = LIST;                                               \
    while( cur->descriptor.asn1 != NULL ) {                                 \
        if( cur->ATTR1 == ATTR1 && cur->ATTR2 == ATTR2 ) {                  \
            *oid = cur->descriptor.asn1;                                    \
            *olen = cur->descriptor.asn1_len;                               \
            return( 0 );                                                    \
        }                                                                   \
        cur++;                                                              \
    }                                                                       \
    return( MBEDTLS_ERR_OID_NOT_FOUND );                                   \
}

#if defined(MBEDTLS_X509_USE_C) || defined(MBEDTLS_X509_CREATE_C)
/*
 * For X520 attribute types
 */
typedef struct {
    mbedtls_oid_descriptor_t    descriptor;
    const char          *short_name;
} oid_x520_attr_t;

FLASH_RDATA const static  const oid_x520_attr_t oid_x520_attr_type[]  =
{
    {
        { ADD_LEN( MBEDTLS_OID_AT_CN ),          "",               "Common Name" },
        "CN",
    },
    {
        { ADD_LEN( MBEDTLS_OID_AT_COUNTRY ),     "",              "Country" },
        "C",
    },
    {
        { ADD_LEN( MBEDTLS_OID_AT_LOCALITY ),    "",                 "Locality" },
        "L",
    },
    {
        { ADD_LEN( MBEDTLS_OID_AT_STATE ),       "",                    "State" },
        "ST",
    },
    {
        { ADD_LEN( MBEDTLS_OID_AT_ORGANIZATION ),"",         "Organization" },
        "O",
    },
    {
        { ADD_LEN( MBEDTLS_OID_AT_ORG_UNIT ),    "",   "Org Unit" },
        "OU",
    },
    {
        { ADD_LEN( MBEDTLS_OID_PKCS9_EMAIL ),    "",                   "E-mail address" },
        "emailAddress",
    },
    {
        { ADD_LEN( MBEDTLS_OID_AT_SERIAL_NUMBER ),"",            "Serial number" },
        "serialNumber",
    },
    {
        { ADD_LEN( MBEDTLS_OID_AT_POSTAL_ADDRESS ),"",          "Postal address" },
        "postalAddress",
    },
    {
        { ADD_LEN( MBEDTLS_OID_AT_POSTAL_CODE ), "",               "Postal code" },
        "postalCode",
    },
    {
        { ADD_LEN( MBEDTLS_OID_AT_SUR_NAME ),    "",                  "Surname" },
        "SN",
    },
    {
        { ADD_LEN( MBEDTLS_OID_AT_GIVEN_NAME ),  "",                "Given name" },
        "GN",
    },
    {
        { ADD_LEN( MBEDTLS_OID_AT_INITIALS ),    "",                 "Initials" },
        "initials",
    },
    {
        { ADD_LEN( MBEDTLS_OID_AT_GENERATION_QUALIFIER ), "", "Generation qualifier" },
        "generationQualifier",
    },
    {
        { ADD_LEN( MBEDTLS_OID_AT_TITLE ),       "",                    "Title" },
        "title",
    },
    {
        { ADD_LEN( MBEDTLS_OID_AT_DN_QUALIFIER ),"",              "Distinguished Name qualifier" },
        "dnQualifier",
    },
    {
        { ADD_LEN( MBEDTLS_OID_AT_PSEUDONYM ),   "",                "Pseudonym" },
        "pseudonym",
    },
    {
        { ADD_LEN( MBEDTLS_OID_DOMAIN_COMPONENT ), "",           "Domain component" },
        "DC",
    },
    {
        { ADD_LEN( MBEDTLS_OID_AT_UNIQUE_IDENTIFIER ), "",    "Unique Identifier" },
        "uniqueIdentifier",
    },
    {
        { NULL, 0, NULL, NULL },
        NULL,
    }
};

FN_OID_TYPED_FROM_ASN1(oid_x520_attr_t, x520_attr, oid_x520_attr_type)
FN_OID_GET_ATTR1(mbedtls_oid_get_attr_short_name, oid_x520_attr_t, x520_attr, const char *, short_name)

/*
 * For X509 extensions
 */
typedef struct {
    mbedtls_oid_descriptor_t    descriptor;
    int                 ext_type;
} oid_x509_ext_t;

FLASH_RDATA static const oid_x509_ext_t oid_x509_ext[]  =
{
    {
        { ADD_LEN( MBEDTLS_OID_BASIC_CONSTRAINTS ),    "",   "Basic Const" },
        MBEDTLS_X509_EXT_BASIC_CONSTRAINTS,
    },
    {
        { ADD_LEN( MBEDTLS_OID_KEY_USAGE ),            "",           "Key Usage" },
        MBEDTLS_X509_EXT_KEY_USAGE,
    },
    {
        { ADD_LEN( MBEDTLS_OID_EXTENDED_KEY_USAGE ),   "",        "ExtKeyUsage" },
        MBEDTLS_X509_EXT_EXTENDED_KEY_USAGE,
    },
    {
        { ADD_LEN( MBEDTLS_OID_SUBJECT_ALT_NAME ),     "",     "Subject Alt Name" },
        MBEDTLS_X509_EXT_SUBJECT_ALT_NAME,
    },
    {
        { ADD_LEN( MBEDTLS_OID_NS_CERT_TYPE ),         "",     "Netscape CRT Type" },
        MBEDTLS_X509_EXT_NS_CERT_TYPE,
    },
    {
        { NULL, 0, NULL, NULL },
        0,
    },
};

FN_OID_TYPED_FROM_ASN1(oid_x509_ext_t, x509_ext, oid_x509_ext)
FN_OID_GET_ATTR1(mbedtls_oid_get_x509_ext_type, oid_x509_ext_t, x509_ext, int, ext_type)

FLASH_RDATA static const mbedtls_oid_descriptor_t oid_ext_key_usage[]  =
{
    { ADD_LEN( MBEDTLS_OID_SERVER_AUTH ),      "",      "TLSWebSrv AUT" },
    { ADD_LEN( MBEDTLS_OID_CLIENT_AUTH ),      "",      "TLSWebCli AUT" },
    { ADD_LEN( MBEDTLS_OID_CODE_SIGNING ),     "",     "Code Signing" },
    { ADD_LEN( MBEDTLS_OID_EMAIL_PROTECTION ), "", "E-mail Protection" },
    { ADD_LEN( MBEDTLS_OID_TIME_STAMPING ),    "",    "Time Stamping" },
    { ADD_LEN( MBEDTLS_OID_OCSP_SIGNING ),     "",     "OCSP Signing" },
    { NULL, 0, NULL, NULL },
};

FN_OID_TYPED_FROM_ASN1(mbedtls_oid_descriptor_t, ext_key_usage, oid_ext_key_usage)
FN_OID_GET_ATTR1(mbedtls_oid_get_extended_key_usage, mbedtls_oid_descriptor_t, ext_key_usage, const char *, description)
#endif /* MBEDTLS_X509_USE_C || MBEDTLS_X509_CREATE_C */

#if defined(MBEDTLS_MD_C)
/*
 * For SignatureAlgorithmIdentifier
 */
typedef struct {
    mbedtls_oid_descriptor_t    descriptor;
    mbedtls_md_type_t           md_alg;
    mbedtls_pk_type_t           pk_alg;
} oid_sig_alg_t;

FLASH_RDATA static const oid_sig_alg_t oid_sig_alg[]  =
{
#if defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_MD2_C)
    {
        { ADD_LEN( MBEDTLS_OID_PKCS1_MD2 ),        "",     "RSA MD2" },
        MBEDTLS_MD_MD2,      MBEDTLS_PK_RSA,
    },
#endif /* MBEDTLS_MD2_C */
#if defined(MBEDTLS_MD4_C)
    {
        { ADD_LEN( MBEDTLS_OID_PKCS1_MD4 ),        "",     "RSA MD4" },
        MBEDTLS_MD_MD4,      MBEDTLS_PK_RSA,
    },
#endif /* MBEDTLS_MD4_C */
#if defined(MBEDTLS_MD5_C)
    {
        { ADD_LEN( MBEDTLS_OID_PKCS1_MD5 ),        "",     "RSA MD5" },
        MBEDTLS_MD_MD5,      MBEDTLS_PK_RSA,
    },
#endif /* MBEDTLS_MD5_C */
#if defined(MBEDTLS_SHA1_C)
    {
        { ADD_LEN( MBEDTLS_OID_PKCS1_SHA1 ),       "",   "RSA SHA1" },
        MBEDTLS_MD_SHA1,     MBEDTLS_PK_RSA,
    },
#endif /* MBEDTLS_SHA1_C */
#if defined(MBEDTLS_SHA256_C)
    {
        { ADD_LEN( MBEDTLS_OID_PKCS1_SHA224 ),     "",  "RSA SHA-224" },
        MBEDTLS_MD_SHA224,   MBEDTLS_PK_RSA,
    },
    {
        { ADD_LEN( MBEDTLS_OID_PKCS1_SHA256 ),     "",  "RSA SHA-256" },
        MBEDTLS_MD_SHA256,   MBEDTLS_PK_RSA,
    },
#endif /* MBEDTLS_SHA256_C */
#if defined(MBEDTLS_SHA512_C)
    {
        { ADD_LEN( MBEDTLS_OID_PKCS1_SHA384 ),     "",  "RSA SHA-384" },
        MBEDTLS_MD_SHA384,   MBEDTLS_PK_RSA,
    },
    {
        { ADD_LEN( MBEDTLS_OID_PKCS1_SHA512 ),     "",  "RSA SHA-512" },
        MBEDTLS_MD_SHA512,   MBEDTLS_PK_RSA,
    },
#endif /* MBEDTLS_SHA512_C */
#if defined(MBEDTLS_SHA1_C)
    {
        { ADD_LEN( MBEDTLS_OID_RSA_SHA_OBS ),      "",   "RSA SHA1" },
        MBEDTLS_MD_SHA1,     MBEDTLS_PK_RSA,
    },
#endif /* MBEDTLS_SHA1_C */
#endif /* MBEDTLS_RSA_C */
#if defined(MBEDTLS_ECDSA_C)
#if defined(MBEDTLS_SHA1_C)
    {
        { ADD_LEN( MBEDTLS_OID_ECDSA_SHA1 ),       "",      "ECDSA SHA1" },
        MBEDTLS_MD_SHA1,     MBEDTLS_PK_ECDSA,
    },
#endif /* MBEDTLS_SHA1_C */
#if defined(MBEDTLS_SHA256_C)
    {
        { ADD_LEN( MBEDTLS_OID_ECDSA_SHA224 ),     "",    "ECDSA SHA224" },
        MBEDTLS_MD_SHA224,   MBEDTLS_PK_ECDSA,
    },
    {
        { ADD_LEN( MBEDTLS_OID_ECDSA_SHA256 ),     "",    "ECDSA SHA256" },
        MBEDTLS_MD_SHA256,   MBEDTLS_PK_ECDSA,
    },
#endif /* MBEDTLS_SHA256_C */
#if defined(MBEDTLS_SHA512_C)
    {
        { ADD_LEN( MBEDTLS_OID_ECDSA_SHA384 ),     "",    "ECDSA SHA384" },
        MBEDTLS_MD_SHA384,   MBEDTLS_PK_ECDSA,
    },
    {
        { ADD_LEN( MBEDTLS_OID_ECDSA_SHA512 ),     "",    "ECDSA SHA512" },
        MBEDTLS_MD_SHA512,   MBEDTLS_PK_ECDSA,
    },
#endif /* MBEDTLS_SHA512_C */
#endif /* MBEDTLS_ECDSA_C */
#if defined(MBEDTLS_RSA_C)
    {
        { ADD_LEN( MBEDTLS_OID_RSASSA_PSS ),        "",           "RSASSA-PSS" },
        MBEDTLS_MD_NONE,     MBEDTLS_PK_RSASSA_PSS,
    },
#endif /* MBEDTLS_RSA_C */
    {
        { NULL, 0, NULL, NULL },
        MBEDTLS_MD_NONE, MBEDTLS_PK_NONE,
    },
};

FN_OID_TYPED_FROM_ASN1(oid_sig_alg_t, sig_alg, oid_sig_alg)
FN_OID_GET_DESCRIPTOR_ATTR1(mbedtls_oid_get_sig_alg_desc, oid_sig_alg_t, sig_alg, const char *, description)
FN_OID_GET_ATTR2(mbedtls_oid_get_sig_alg, oid_sig_alg_t, sig_alg, mbedtls_md_type_t, md_alg, mbedtls_pk_type_t, pk_alg)
FN_OID_GET_OID_BY_ATTR2(mbedtls_oid_get_oid_by_sig_alg, oid_sig_alg_t, oid_sig_alg, mbedtls_pk_type_t, pk_alg, mbedtls_md_type_t, md_alg)
#endif /* MBEDTLS_MD_C */

/*
 * For PublicKeyInfo (PKCS1, RFC 5480)
 */
typedef struct {
    mbedtls_oid_descriptor_t    descriptor;
    mbedtls_pk_type_t           pk_alg;
} oid_pk_alg_t;

FLASH_RDATA static const oid_pk_alg_t oid_pk_alg[]  =
{
    {
        { ADD_LEN( MBEDTLS_OID_PKCS1_RSA ),      "",   "RSA" },
        MBEDTLS_PK_RSA,
    },
    {
        { ADD_LEN( MBEDTLS_OID_EC_ALG_UNRESTRICTED ),  "",   "GenECkey" },
        MBEDTLS_PK_ECKEY,
    },
    {
        { ADD_LEN( MBEDTLS_OID_EC_ALG_ECDH ),          "",          "ECKey4ECDH" },
        MBEDTLS_PK_ECKEY_DH,
    },
    {
        { NULL, 0, NULL, NULL },
        MBEDTLS_PK_NONE,
    },
};

FN_OID_TYPED_FROM_ASN1(oid_pk_alg_t, pk_alg, oid_pk_alg)
FN_OID_GET_ATTR1(mbedtls_oid_get_pk_alg, oid_pk_alg_t, pk_alg, mbedtls_pk_type_t, pk_alg)
FN_OID_GET_OID_BY_ATTR1(mbedtls_oid_get_oid_by_pk_alg, oid_pk_alg_t, oid_pk_alg, mbedtls_pk_type_t, pk_alg)

#if defined(MBEDTLS_ECP_C)
/*
 * For namedCurve (RFC 5480)
 */
typedef struct {
    mbedtls_oid_descriptor_t    descriptor;
    mbedtls_ecp_group_id        grp_id;
} oid_ecp_grp_t;

FLASH_RDATA static const oid_ecp_grp_t oid_ecp_grp[]  =
{
#if defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED)
    {
        { ADD_LEN( MBEDTLS_OID_EC_GRP_SECP192R1 ), "",    "secp192r1" },
        MBEDTLS_ECP_DP_SECP192R1,
    },
#endif /* MBEDTLS_ECP_DP_SECP192R1_ENABLED */
#if defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED)
    {
        { ADD_LEN( MBEDTLS_OID_EC_GRP_SECP224R1 ), "",    "secp224r1" },
        MBEDTLS_ECP_DP_SECP224R1,
    },
#endif /* MBEDTLS_ECP_DP_SECP224R1_ENABLED */
#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
    {
        { ADD_LEN( MBEDTLS_OID_EC_GRP_SECP256R1 ), "",    "secp256r1" },
        MBEDTLS_ECP_DP_SECP256R1,
    },
#endif /* MBEDTLS_ECP_DP_SECP256R1_ENABLED */
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
    {
        { ADD_LEN( MBEDTLS_OID_EC_GRP_SECP384R1 ), "",    "secp384r1" },
        MBEDTLS_ECP_DP_SECP384R1,
    },
#endif /* MBEDTLS_ECP_DP_SECP384R1_ENABLED */
#if defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)
    {
        { ADD_LEN( MBEDTLS_OID_EC_GRP_SECP521R1 ), "",    "secp521r1" },
        MBEDTLS_ECP_DP_SECP521R1,
    },
#endif /* MBEDTLS_ECP_DP_SECP521R1_ENABLED */
#if defined(MBEDTLS_ECP_DP_SECP192K1_ENABLED)
    {
        { ADD_LEN( MBEDTLS_OID_EC_GRP_SECP192K1 ), "",    "secp192k1" },
        MBEDTLS_ECP_DP_SECP192K1,
    },
#endif /* MBEDTLS_ECP_DP_SECP192K1_ENABLED */
#if defined(MBEDTLS_ECP_DP_SECP224K1_ENABLED)
    {
        { ADD_LEN( MBEDTLS_OID_EC_GRP_SECP224K1 ), "",    "secp224k1" },
        MBEDTLS_ECP_DP_SECP224K1,
    },
#endif /* MBEDTLS_ECP_DP_SECP224K1_ENABLED */
#if defined(MBEDTLS_ECP_DP_SECP256K1_ENABLED)
    {
        { ADD_LEN( MBEDTLS_OID_EC_GRP_SECP256K1 ), "",    "secp256k1" },
        MBEDTLS_ECP_DP_SECP256K1,
    },
#endif /* MBEDTLS_ECP_DP_SECP256K1_ENABLED */
#if defined(MBEDTLS_ECP_DP_BP256R1_ENABLED)
    {
        { ADD_LEN( MBEDTLS_OID_EC_GRP_BP256R1 ),   "","brainpool256r1" },
        MBEDTLS_ECP_DP_BP256R1,
    },
#endif /* MBEDTLS_ECP_DP_BP256R1_ENABLED */
#if defined(MBEDTLS_ECP_DP_BP384R1_ENABLED)
    {
        { ADD_LEN( MBEDTLS_OID_EC_GRP_BP384R1 ),   "","brainpool384r1" },
        MBEDTLS_ECP_DP_BP384R1,
    },
#endif /* MBEDTLS_ECP_DP_BP384R1_ENABLED */
#if defined(MBEDTLS_ECP_DP_BP512R1_ENABLED)
    {
        { ADD_LEN( MBEDTLS_OID_EC_GRP_BP512R1 ),   "","brainpool512r1" },
        MBEDTLS_ECP_DP_BP512R1,
    },
#endif /* MBEDTLS_ECP_DP_BP512R1_ENABLED */
    {
        { NULL, 0, NULL, NULL },
        MBEDTLS_ECP_DP_NONE,
    },
};

FN_OID_TYPED_FROM_ASN1(oid_ecp_grp_t, grp_id, oid_ecp_grp)
FN_OID_GET_ATTR1(mbedtls_oid_get_ec_grp, oid_ecp_grp_t, grp_id, mbedtls_ecp_group_id, grp_id)
FN_OID_GET_OID_BY_ATTR1(mbedtls_oid_get_oid_by_ec_grp, oid_ecp_grp_t, oid_ecp_grp, mbedtls_ecp_group_id, grp_id)
#endif /* MBEDTLS_ECP_C */

#if defined(MBEDTLS_CIPHER_C)
/*
 * For PKCS#5 PBES2 encryption algorithm
 */
typedef struct {
    mbedtls_oid_descriptor_t    descriptor;
    mbedtls_cipher_type_t       cipher_alg;
} oid_cipher_alg_t;

FLASH_RDATA static const oid_cipher_alg_t oid_cipher_alg[] =
{
    {
        { ADD_LEN( MBEDTLS_OID_DES_CBC ),              "",       "DES-CBC" },
        MBEDTLS_CIPHER_DES_CBC,
    },
    {
        { ADD_LEN( MBEDTLS_OID_DES_EDE3_CBC ),         "", "DES-EDE3-CBC" },
        MBEDTLS_CIPHER_DES_EDE3_CBC,
    },
    {
        { NULL, 0, NULL, NULL },
        MBEDTLS_CIPHER_NONE,
    },
};

FN_OID_TYPED_FROM_ASN1(oid_cipher_alg_t, cipher_alg, oid_cipher_alg)
FN_OID_GET_ATTR1(mbedtls_oid_get_cipher_alg, oid_cipher_alg_t, cipher_alg, mbedtls_cipher_type_t, cipher_alg)
#endif /* MBEDTLS_CIPHER_C */

#if defined(MBEDTLS_MD_C)
/*
 * For digestAlgorithm
 */
typedef struct {
    mbedtls_oid_descriptor_t    descriptor;
    mbedtls_md_type_t           md_alg;
} oid_md_alg_t;

FLASH_RDATA  static const oid_md_alg_t oid_md_alg[]  =
{
#if defined(MBEDTLS_MD2_C)
    {
        { ADD_LEN( MBEDTLS_OID_DIGEST_ALG_MD2 ),       "",       "MD2" },
        MBEDTLS_MD_MD2,
    },
#endif /* MBEDTLS_MD2_C */
#if defined(MBEDTLS_MD4_C)
    {
        { ADD_LEN( MBEDTLS_OID_DIGEST_ALG_MD4 ),       "",       "MD4" },
        MBEDTLS_MD_MD4,
    },
#endif /* MBEDTLS_MD4_C */
#if defined(MBEDTLS_MD5_C)
    {
        { ADD_LEN( MBEDTLS_OID_DIGEST_ALG_MD5 ),       "",       "MD5" },
        MBEDTLS_MD_MD5,
    },
#endif /* MBEDTLS_MD5_C */
#if defined(MBEDTLS_SHA1_C)
    {
        { ADD_LEN( MBEDTLS_OID_DIGEST_ALG_SHA1 ),      "",      "SHA-1" },
        MBEDTLS_MD_SHA1,
    },
#endif /* MBEDTLS_SHA1_C */
#if defined(MBEDTLS_SHA256_C)
    {
        { ADD_LEN( MBEDTLS_OID_DIGEST_ALG_SHA224 ),    "",    "SHA-224" },
        MBEDTLS_MD_SHA224,
    },
    {
        { ADD_LEN( MBEDTLS_OID_DIGEST_ALG_SHA256 ),    "",    "SHA-256" },
        MBEDTLS_MD_SHA256,
    },
#endif /* MBEDTLS_SHA256_C */
#if defined(MBEDTLS_SHA512_C)
    {
        { ADD_LEN( MBEDTLS_OID_DIGEST_ALG_SHA384 ),    "",    "SHA-384" },
        MBEDTLS_MD_SHA384,
    },
    {
        { ADD_LEN( MBEDTLS_OID_DIGEST_ALG_SHA512 ),    "",    "SHA-512" },
        MBEDTLS_MD_SHA512,
    },
#endif /* MBEDTLS_SHA512_C */
    {
        { NULL, 0, NULL, NULL },
        MBEDTLS_MD_NONE,
    },
};

FN_OID_TYPED_FROM_ASN1(oid_md_alg_t, md_alg, oid_md_alg)
FN_OID_GET_ATTR1(mbedtls_oid_get_md_alg, oid_md_alg_t, md_alg, mbedtls_md_type_t, md_alg)
FN_OID_GET_OID_BY_ATTR1(mbedtls_oid_get_oid_by_md, oid_md_alg_t, oid_md_alg, mbedtls_md_type_t, md_alg)
#endif /* MBEDTLS_MD_C */

#if defined(MBEDTLS_PKCS12_C)
/*
 * For PKCS#12 PBEs
 */
typedef struct {
    mbedtls_oid_descriptor_t    descriptor;
    mbedtls_md_type_t           md_alg;
    mbedtls_cipher_type_t       cipher_alg;
} oid_pkcs12_pbe_alg_t;

FLASH_RDATA static const oid_pkcs12_pbe_alg_t oid_pkcs12_pbe_alg[] =
{
    {
        { ADD_LEN( MBEDTLS_OID_PKCS12_PBE_SHA1_DES3_EDE_CBC ), "", "PBE SHA1 and 3-Key 3DES" },
        MBEDTLS_MD_SHA1,      MBEDTLS_CIPHER_DES_EDE3_CBC,
    },
    {
        { ADD_LEN( MBEDTLS_OID_PKCS12_PBE_SHA1_DES2_EDE_CBC ), "", "PBE SHA1 and 2-Key 3DES" },
        MBEDTLS_MD_SHA1,      MBEDTLS_CIPHER_DES_EDE_CBC,
    },
    {
        { NULL, 0, NULL, NULL },
        MBEDTLS_MD_NONE, MBEDTLS_CIPHER_NONE,
    },
};

FN_OID_TYPED_FROM_ASN1(oid_pkcs12_pbe_alg_t, pkcs12_pbe_alg, oid_pkcs12_pbe_alg)
FN_OID_GET_ATTR2(mbedtls_oid_get_pkcs12_pbe_alg, oid_pkcs12_pbe_alg_t, pkcs12_pbe_alg, mbedtls_md_type_t, md_alg, mbedtls_cipher_type_t, cipher_alg)
#endif /* MBEDTLS_PKCS12_C */

#define OID_SAFE_SNPRINTF                               \
    do {                                                \
        if( ret < 0 || (size_t) ret >= n )              \
            return( MBEDTLS_ERR_OID_BUF_TOO_SMALL );    \
                                                        \
        n -= (size_t) ret;                              \
        p += (size_t) ret;                              \
    } while( 0 )

/* Return the x.y.z.... style numeric string for the given OID */
FLASH_FUNC int mbedtls_oid_get_numeric_string( char *buf, size_t size,
                            const mbedtls_asn1_buf *oid )
{
    int ret;
    size_t i, n;
    unsigned int value;
    char *p;

    p = buf;
    n = size;

    /* First byte contains first two dots */
    if( oid->len > 0 )
    {
        ret = mbedtls_snprintf( p, n, "%d.%d", oid->p[0] / 40, oid->p[0] % 40 );
        OID_SAFE_SNPRINTF;
    }

    value = 0;
    for( i = 1; i < oid->len; i++ )
    {
        /* Prevent overflow in value. */
        if( ( ( value << 7 ) >> 7 ) != value )
            return( MBEDTLS_ERR_OID_BUF_TOO_SMALL );

        value <<= 7;
        value += oid->p[i] & 0x7F;

        if( !( oid->p[i] & 0x80 ) )
        {
            /* Last byte */
            ret = mbedtls_snprintf( p, n, ".%d", value );
            OID_SAFE_SNPRINTF;
            value = 0;
        }
    }

    return( (int) ( size - n ) );
}

#endif /* MBEDTLS_OID_C */
