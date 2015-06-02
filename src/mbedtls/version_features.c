/*
 *  Version feature information
 *
 *  Copyright (C) 2006-2014, ARM Limited, All Rights Reserved
 *
 *  This file is part of mbed TLS (https://polarssl.org)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
 
#include "heivs/config"
#if USE_MBEDTLS

#if !defined(POLARSSL_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#if defined(POLARSSL_VERSION_C)

#include "mbedtls/version.h"

#include <string.h>

#if defined(_MSC_VER) && !defined strcasecmp && !defined(EFIX64) && \
    !defined(EFI32)
#define strcasecmp _stricmp
#endif

const char *features[] = {
#if defined(POLARSSL_VERSION_FEATURES)
#if defined(POLARSSL_HAVE_INT8)
    "POLARSSL_HAVE_INT8",
#endif /* POLARSSL_HAVE_INT8 */
#if defined(POLARSSL_HAVE_INT16)
    "POLARSSL_HAVE_INT16",
#endif /* POLARSSL_HAVE_INT16 */
#if defined(POLARSSL_HAVE_LONGLONG)
    "POLARSSL_HAVE_LONGLONG",
#endif /* POLARSSL_HAVE_LONGLONG */
#if defined(POLARSSL_HAVE_ASM)
    "POLARSSL_HAVE_ASM",
#endif /* POLARSSL_HAVE_ASM */
#if defined(POLARSSL_HAVE_SSE2)
    "POLARSSL_HAVE_SSE2",
#endif /* POLARSSL_HAVE_SSE2 */
#if defined(POLARSSL_HAVE_TIME)
    "POLARSSL_HAVE_TIME",
#endif /* POLARSSL_HAVE_TIME */
#if defined(POLARSSL_HAVE_IPV6)
    "POLARSSL_HAVE_IPV6",
#endif /* POLARSSL_HAVE_IPV6 */
#if defined(POLARSSL_PLATFORM_MEMORY)
    "POLARSSL_PLATFORM_MEMORY",
#endif /* POLARSSL_PLATFORM_MEMORY */
#if defined(POLARSSL_PLATFORM_NO_STD_FUNCTIONS)
    "POLARSSL_PLATFORM_NO_STD_FUNCTIONS",
#endif /* POLARSSL_PLATFORM_NO_STD_FUNCTIONS */
#if defined(POLARSSL_PLATFORM_PRINTF_ALT)
    "POLARSSL_PLATFORM_PRINTF_ALT",
#endif /* POLARSSL_PLATFORM_PRINTF_ALT */
#if defined(POLARSSL_PLATFORM_FPRINTF_ALT)
    "POLARSSL_PLATFORM_FPRINTF_ALT",
#endif /* POLARSSL_PLATFORM_FPRINTF_ALT */
#if defined(POLARSSL_TIMING_ALT)
    "POLARSSL_TIMING_ALT",
#endif /* POLARSSL_TIMING_ALT */
#if defined(POLARSSL_AES_ALT)
    "POLARSSL_AES_ALT",
#endif /* POLARSSL_AES_ALT */
#if defined(POLARSSL_ARC4_ALT)
    "POLARSSL_ARC4_ALT",
#endif /* POLARSSL_ARC4_ALT */
#if defined(POLARSSL_BLOWFISH_ALT)
    "POLARSSL_BLOWFISH_ALT",
#endif /* POLARSSL_BLOWFISH_ALT */
#if defined(POLARSSL_CAMELLIA_ALT)
    "POLARSSL_CAMELLIA_ALT",
#endif /* POLARSSL_CAMELLIA_ALT */
#if defined(POLARSSL_DES_ALT)
    "POLARSSL_DES_ALT",
#endif /* POLARSSL_DES_ALT */
#if defined(POLARSSL_XTEA_ALT)
    "POLARSSL_XTEA_ALT",
#endif /* POLARSSL_XTEA_ALT */
#if defined(POLARSSL_MD2_ALT)
    "POLARSSL_MD2_ALT",
#endif /* POLARSSL_MD2_ALT */
#if defined(POLARSSL_MD4_ALT)
    "POLARSSL_MD4_ALT",
#endif /* POLARSSL_MD4_ALT */
#if defined(POLARSSL_MD5_ALT)
    "POLARSSL_MD5_ALT",
#endif /* POLARSSL_MD5_ALT */
#if defined(POLARSSL_RIPEMD160_ALT)
    "POLARSSL_RIPEMD160_ALT",
#endif /* POLARSSL_RIPEMD160_ALT */
#if defined(POLARSSL_SHA1_ALT)
    "POLARSSL_SHA1_ALT",
#endif /* POLARSSL_SHA1_ALT */
#if defined(POLARSSL_SHA256_ALT)
    "POLARSSL_SHA256_ALT",
#endif /* POLARSSL_SHA256_ALT */
#if defined(POLARSSL_SHA512_ALT)
    "POLARSSL_SHA512_ALT",
#endif /* POLARSSL_SHA512_ALT */
#if defined(POLARSSL_AES_ROM_TABLES)
    "POLARSSL_AES_ROM_TABLES",
#endif /* POLARSSL_AES_ROM_TABLES */
#if defined(POLARSSL_CIPHER_MODE_CBC)
    "POLARSSL_CIPHER_MODE_CBC",
#endif /* POLARSSL_CIPHER_MODE_CBC */
#if defined(POLARSSL_CIPHER_MODE_CFB)
    "POLARSSL_CIPHER_MODE_CFB",
#endif /* POLARSSL_CIPHER_MODE_CFB */
#if defined(POLARSSL_CIPHER_MODE_CTR)
    "POLARSSL_CIPHER_MODE_CTR",
#endif /* POLARSSL_CIPHER_MODE_CTR */
#if defined(POLARSSL_CIPHER_NULL_CIPHER)
    "POLARSSL_CIPHER_NULL_CIPHER",
#endif /* POLARSSL_CIPHER_NULL_CIPHER */
#if defined(POLARSSL_CIPHER_PADDING_PKCS7)
    "POLARSSL_CIPHER_PADDING_PKCS7",
#endif /* POLARSSL_CIPHER_PADDING_PKCS7 */
#if defined(POLARSSL_CIPHER_PADDING_ONE_AND_ZEROS)
    "POLARSSL_CIPHER_PADDING_ONE_AND_ZEROS",
#endif /* POLARSSL_CIPHER_PADDING_ONE_AND_ZEROS */
#if defined(POLARSSL_CIPHER_PADDING_ZEROS_AND_LEN)
    "POLARSSL_CIPHER_PADDING_ZEROS_AND_LEN",
#endif /* POLARSSL_CIPHER_PADDING_ZEROS_AND_LEN */
#if defined(POLARSSL_CIPHER_PADDING_ZEROS)
    "POLARSSL_CIPHER_PADDING_ZEROS",
#endif /* POLARSSL_CIPHER_PADDING_ZEROS */
#if defined(POLARSSL_ENABLE_WEAK_CIPHERSUITES)
    "POLARSSL_ENABLE_WEAK_CIPHERSUITES",
#endif /* POLARSSL_ENABLE_WEAK_CIPHERSUITES */
#if defined(POLARSSL_REMOVE_ARC4_CIPHERSUITES)
    "POLARSSL_REMOVE_ARC4_CIPHERSUITES",
#endif /* POLARSSL_REMOVE_ARC4_CIPHERSUITES */
#if defined(POLARSSL_ECP_DP_SECP192R1_ENABLED)
    "POLARSSL_ECP_DP_SECP192R1_ENABLED",
#endif /* POLARSSL_ECP_DP_SECP192R1_ENABLED */
#if defined(POLARSSL_ECP_DP_SECP224R1_ENABLED)
    "POLARSSL_ECP_DP_SECP224R1_ENABLED",
#endif /* POLARSSL_ECP_DP_SECP224R1_ENABLED */
#if defined(POLARSSL_ECP_DP_SECP256R1_ENABLED)
    "POLARSSL_ECP_DP_SECP256R1_ENABLED",
#endif /* POLARSSL_ECP_DP_SECP256R1_ENABLED */
#if defined(POLARSSL_ECP_DP_SECP384R1_ENABLED)
    "POLARSSL_ECP_DP_SECP384R1_ENABLED",
#endif /* POLARSSL_ECP_DP_SECP384R1_ENABLED */
#if defined(POLARSSL_ECP_DP_SECP521R1_ENABLED)
    "POLARSSL_ECP_DP_SECP521R1_ENABLED",
#endif /* POLARSSL_ECP_DP_SECP521R1_ENABLED */
#if defined(POLARSSL_ECP_DP_SECP192K1_ENABLED)
    "POLARSSL_ECP_DP_SECP192K1_ENABLED",
#endif /* POLARSSL_ECP_DP_SECP192K1_ENABLED */
#if defined(POLARSSL_ECP_DP_SECP224K1_ENABLED)
    "POLARSSL_ECP_DP_SECP224K1_ENABLED",
#endif /* POLARSSL_ECP_DP_SECP224K1_ENABLED */
#if defined(POLARSSL_ECP_DP_SECP256K1_ENABLED)
    "POLARSSL_ECP_DP_SECP256K1_ENABLED",
#endif /* POLARSSL_ECP_DP_SECP256K1_ENABLED */
#if defined(POLARSSL_ECP_DP_BP256R1_ENABLED)
    "POLARSSL_ECP_DP_BP256R1_ENABLED",
#endif /* POLARSSL_ECP_DP_BP256R1_ENABLED */
#if defined(POLARSSL_ECP_DP_BP384R1_ENABLED)
    "POLARSSL_ECP_DP_BP384R1_ENABLED",
#endif /* POLARSSL_ECP_DP_BP384R1_ENABLED */
#if defined(POLARSSL_ECP_DP_BP512R1_ENABLED)
    "POLARSSL_ECP_DP_BP512R1_ENABLED",
#endif /* POLARSSL_ECP_DP_BP512R1_ENABLED */
#if defined(POLARSSL_ECP_DP_M221_ENABLED)
    "POLARSSL_ECP_DP_M221_ENABLED",
#endif /* POLARSSL_ECP_DP_M221_ENABLED */
#if defined(POLARSSL_ECP_DP_M255_ENABLED)
    "POLARSSL_ECP_DP_M255_ENABLED",
#endif /* POLARSSL_ECP_DP_M255_ENABLED */
#if defined(POLARSSL_ECP_DP_M383_ENABLED)
    "POLARSSL_ECP_DP_M383_ENABLED",
#endif /* POLARSSL_ECP_DP_M383_ENABLED */
#if defined(POLARSSL_ECP_DP_M511_ENABLED)
    "POLARSSL_ECP_DP_M511_ENABLED",
#endif /* POLARSSL_ECP_DP_M511_ENABLED */
#if defined(POLARSSL_ECP_NIST_OPTIM)
    "POLARSSL_ECP_NIST_OPTIM",
#endif /* POLARSSL_ECP_NIST_OPTIM */
#if defined(POLARSSL_ECDSA_DETERMINISTIC)
    "POLARSSL_ECDSA_DETERMINISTIC",
#endif /* POLARSSL_ECDSA_DETERMINISTIC */
#if defined(POLARSSL_KEY_EXCHANGE_PSK_ENABLED)
    "POLARSSL_KEY_EXCHANGE_PSK_ENABLED",
#endif /* POLARSSL_KEY_EXCHANGE_PSK_ENABLED */
#if defined(POLARSSL_KEY_EXCHANGE_DHE_PSK_ENABLED)
    "POLARSSL_KEY_EXCHANGE_DHE_PSK_ENABLED",
#endif /* POLARSSL_KEY_EXCHANGE_DHE_PSK_ENABLED */
#if defined(POLARSSL_KEY_EXCHANGE_ECDHE_PSK_ENABLED)
    "POLARSSL_KEY_EXCHANGE_ECDHE_PSK_ENABLED",
#endif /* POLARSSL_KEY_EXCHANGE_ECDHE_PSK_ENABLED */
#if defined(POLARSSL_KEY_EXCHANGE_RSA_PSK_ENABLED)
    "POLARSSL_KEY_EXCHANGE_RSA_PSK_ENABLED",
#endif /* POLARSSL_KEY_EXCHANGE_RSA_PSK_ENABLED */
#if defined(POLARSSL_KEY_EXCHANGE_RSA_ENABLED)
    "POLARSSL_KEY_EXCHANGE_RSA_ENABLED",
#endif /* POLARSSL_KEY_EXCHANGE_RSA_ENABLED */
#if defined(POLARSSL_KEY_EXCHANGE_DHE_RSA_ENABLED)
    "POLARSSL_KEY_EXCHANGE_DHE_RSA_ENABLED",
#endif /* POLARSSL_KEY_EXCHANGE_DHE_RSA_ENABLED */
#if defined(POLARSSL_KEY_EXCHANGE_ECDHE_RSA_ENABLED)
    "POLARSSL_KEY_EXCHANGE_ECDHE_RSA_ENABLED",
#endif /* POLARSSL_KEY_EXCHANGE_ECDHE_RSA_ENABLED */
#if defined(POLARSSL_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED)
    "POLARSSL_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED",
#endif /* POLARSSL_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED */
#if defined(POLARSSL_KEY_EXCHANGE_ECDH_ECDSA_ENABLED)
    "POLARSSL_KEY_EXCHANGE_ECDH_ECDSA_ENABLED",
#endif /* POLARSSL_KEY_EXCHANGE_ECDH_ECDSA_ENABLED */
#if defined(POLARSSL_KEY_EXCHANGE_ECDH_RSA_ENABLED)
    "POLARSSL_KEY_EXCHANGE_ECDH_RSA_ENABLED",
#endif /* POLARSSL_KEY_EXCHANGE_ECDH_RSA_ENABLED */
#if defined(POLARSSL_PK_PARSE_EC_EXTENDED)
    "POLARSSL_PK_PARSE_EC_EXTENDED",
#endif /* POLARSSL_PK_PARSE_EC_EXTENDED */
#if defined(POLARSSL_ERROR_STRERROR_BC)
    "POLARSSL_ERROR_STRERROR_BC",
#endif /* POLARSSL_ERROR_STRERROR_BC */
#if defined(POLARSSL_ERROR_STRERROR_DUMMY)
    "POLARSSL_ERROR_STRERROR_DUMMY",
#endif /* POLARSSL_ERROR_STRERROR_DUMMY */
#if defined(POLARSSL_GENPRIME)
    "POLARSSL_GENPRIME",
#endif /* POLARSSL_GENPRIME */
#if defined(POLARSSL_FS_IO)
    "POLARSSL_FS_IO",
#endif /* POLARSSL_FS_IO */
#if defined(POLARSSL_NO_DEFAULT_ENTROPY_SOURCES)
    "POLARSSL_NO_DEFAULT_ENTROPY_SOURCES",
#endif /* POLARSSL_NO_DEFAULT_ENTROPY_SOURCES */
#if defined(POLARSSL_NO_PLATFORM_ENTROPY)
    "POLARSSL_NO_PLATFORM_ENTROPY",
#endif /* POLARSSL_NO_PLATFORM_ENTROPY */
#if defined(POLARSSL_ENTROPY_FORCE_SHA256)
    "POLARSSL_ENTROPY_FORCE_SHA256",
#endif /* POLARSSL_ENTROPY_FORCE_SHA256 */
#if defined(POLARSSL_MEMORY_DEBUG)
    "POLARSSL_MEMORY_DEBUG",
#endif /* POLARSSL_MEMORY_DEBUG */
#if defined(POLARSSL_MEMORY_BACKTRACE)
    "POLARSSL_MEMORY_BACKTRACE",
#endif /* POLARSSL_MEMORY_BACKTRACE */
#if defined(POLARSSL_PKCS1_V15)
    "POLARSSL_PKCS1_V15",
#endif /* POLARSSL_PKCS1_V15 */
#if defined(POLARSSL_PKCS1_V21)
    "POLARSSL_PKCS1_V21",
#endif /* POLARSSL_PKCS1_V21 */
#if defined(POLARSSL_RSA_NO_CRT)
    "POLARSSL_RSA_NO_CRT",
#endif /* POLARSSL_RSA_NO_CRT */
#if defined(POLARSSL_SELF_TEST)
    "POLARSSL_SELF_TEST",
#endif /* POLARSSL_SELF_TEST */
#if defined(POLARSSL_SSL_AEAD_RANDOM_IV)
    "POLARSSL_SSL_AEAD_RANDOM_IV",
#endif /* POLARSSL_SSL_AEAD_RANDOM_IV */
#if defined(POLARSSL_SSL_ALERT_MESSAGES)
    "POLARSSL_SSL_ALERT_MESSAGES",
#endif /* POLARSSL_SSL_ALERT_MESSAGES */
#if defined(POLARSSL_SSL_DEBUG_ALL)
    "POLARSSL_SSL_DEBUG_ALL",
#endif /* POLARSSL_SSL_DEBUG_ALL */
#if defined(POLARSSL_SSL_ENCRYPT_THEN_MAC)
    "POLARSSL_SSL_ENCRYPT_THEN_MAC",
#endif /* POLARSSL_SSL_ENCRYPT_THEN_MAC */
#if defined(POLARSSL_SSL_EXTENDED_MASTER_SECRET)
    "POLARSSL_SSL_EXTENDED_MASTER_SECRET",
#endif /* POLARSSL_SSL_EXTENDED_MASTER_SECRET */
#if defined(POLARSSL_SSL_FALLBACK_SCSV)
    "POLARSSL_SSL_FALLBACK_SCSV",
#endif /* POLARSSL_SSL_FALLBACK_SCSV */
#if defined(POLARSSL_SSL_HW_RECORD_ACCEL)
    "POLARSSL_SSL_HW_RECORD_ACCEL",
#endif /* POLARSSL_SSL_HW_RECORD_ACCEL */
#if defined(POLARSSL_SSL_CBC_RECORD_SPLITTING)
    "POLARSSL_SSL_CBC_RECORD_SPLITTING",
#endif /* POLARSSL_SSL_CBC_RECORD_SPLITTING */
#if defined(POLARSSL_SSL_DISABLE_RENEGOTIATION)
    "POLARSSL_SSL_DISABLE_RENEGOTIATION",
#endif /* POLARSSL_SSL_DISABLE_RENEGOTIATION */
#if defined(POLARSSL_SSL_SRV_SUPPORT_SSLV2_CLIENT_HELLO)
    "POLARSSL_SSL_SRV_SUPPORT_SSLV2_CLIENT_HELLO",
#endif /* POLARSSL_SSL_SRV_SUPPORT_SSLV2_CLIENT_HELLO */
#if defined(POLARSSL_SSL_SRV_RESPECT_CLIENT_PREFERENCE)
    "POLARSSL_SSL_SRV_RESPECT_CLIENT_PREFERENCE",
#endif /* POLARSSL_SSL_SRV_RESPECT_CLIENT_PREFERENCE */
#if defined(POLARSSL_SSL_MAX_FRAGMENT_LENGTH)
    "POLARSSL_SSL_MAX_FRAGMENT_LENGTH",
#endif /* POLARSSL_SSL_MAX_FRAGMENT_LENGTH */
#if defined(POLARSSL_SSL_PROTO_SSL3)
    "POLARSSL_SSL_PROTO_SSL3",
#endif /* POLARSSL_SSL_PROTO_SSL3 */
#if defined(POLARSSL_SSL_PROTO_TLS1)
    "POLARSSL_SSL_PROTO_TLS1",
#endif /* POLARSSL_SSL_PROTO_TLS1 */
#if defined(POLARSSL_SSL_PROTO_TLS1_1)
    "POLARSSL_SSL_PROTO_TLS1_1",
#endif /* POLARSSL_SSL_PROTO_TLS1_1 */
#if defined(POLARSSL_SSL_PROTO_TLS1_2)
    "POLARSSL_SSL_PROTO_TLS1_2",
#endif /* POLARSSL_SSL_PROTO_TLS1_2 */
#if defined(POLARSSL_SSL_ALPN)
    "POLARSSL_SSL_ALPN",
#endif /* POLARSSL_SSL_ALPN */
#if defined(POLARSSL_SSL_SESSION_TICKETS)
    "POLARSSL_SSL_SESSION_TICKETS",
#endif /* POLARSSL_SSL_SESSION_TICKETS */
#if defined(POLARSSL_SSL_SERVER_NAME_INDICATION)
    "POLARSSL_SSL_SERVER_NAME_INDICATION",
#endif /* POLARSSL_SSL_SERVER_NAME_INDICATION */
#if defined(POLARSSL_SSL_TRUNCATED_HMAC)
    "POLARSSL_SSL_TRUNCATED_HMAC",
#endif /* POLARSSL_SSL_TRUNCATED_HMAC */
#if defined(POLARSSL_SSL_SET_CURVES)
    "POLARSSL_SSL_SET_CURVES",
#endif /* POLARSSL_SSL_SET_CURVES */
#if defined(POLARSSL_THREADING_ALT)
    "POLARSSL_THREADING_ALT",
#endif /* POLARSSL_THREADING_ALT */
#if defined(POLARSSL_THREADING_PTHREAD)
    "POLARSSL_THREADING_PTHREAD",
#endif /* POLARSSL_THREADING_PTHREAD */
#if defined(POLARSSL_VERSION_FEATURES)
    "POLARSSL_VERSION_FEATURES",
#endif /* POLARSSL_VERSION_FEATURES */
#if defined(POLARSSL_X509_ALLOW_EXTENSIONS_NON_V3)
    "POLARSSL_X509_ALLOW_EXTENSIONS_NON_V3",
#endif /* POLARSSL_X509_ALLOW_EXTENSIONS_NON_V3 */
#if defined(POLARSSL_X509_ALLOW_UNSUPPORTED_CRITICAL_EXTENSION)
    "POLARSSL_X509_ALLOW_UNSUPPORTED_CRITICAL_EXTENSION",
#endif /* POLARSSL_X509_ALLOW_UNSUPPORTED_CRITICAL_EXTENSION */
#if defined(POLARSSL_X509_CHECK_KEY_USAGE)
    "POLARSSL_X509_CHECK_KEY_USAGE",
#endif /* POLARSSL_X509_CHECK_KEY_USAGE */
#if defined(POLARSSL_X509_CHECK_EXTENDED_KEY_USAGE)
    "POLARSSL_X509_CHECK_EXTENDED_KEY_USAGE",
#endif /* POLARSSL_X509_CHECK_EXTENDED_KEY_USAGE */
#if defined(POLARSSL_X509_RSASSA_PSS_SUPPORT)
    "POLARSSL_X509_RSASSA_PSS_SUPPORT",
#endif /* POLARSSL_X509_RSASSA_PSS_SUPPORT */
#if defined(POLARSSL_ZLIB_SUPPORT)
    "POLARSSL_ZLIB_SUPPORT",
#endif /* POLARSSL_ZLIB_SUPPORT */
#if defined(POLARSSL_AESNI_C)
    "POLARSSL_AESNI_C",
#endif /* POLARSSL_AESNI_C */
#if defined(POLARSSL_AES_C)
    "POLARSSL_AES_C",
#endif /* POLARSSL_AES_C */
#if defined(POLARSSL_ARC4_C)
    "POLARSSL_ARC4_C",
#endif /* POLARSSL_ARC4_C */
#if defined(POLARSSL_ASN1_PARSE_C)
    "POLARSSL_ASN1_PARSE_C",
#endif /* POLARSSL_ASN1_PARSE_C */
#if defined(POLARSSL_ASN1_WRITE_C)
    "POLARSSL_ASN1_WRITE_C",
#endif /* POLARSSL_ASN1_WRITE_C */
#if defined(POLARSSL_BASE64_C)
    "POLARSSL_BASE64_C",
#endif /* POLARSSL_BASE64_C */
#if defined(POLARSSL_BIGNUM_C)
    "POLARSSL_BIGNUM_C",
#endif /* POLARSSL_BIGNUM_C */
#if defined(POLARSSL_BLOWFISH_C)
    "POLARSSL_BLOWFISH_C",
#endif /* POLARSSL_BLOWFISH_C */
#if defined(POLARSSL_CAMELLIA_C)
    "POLARSSL_CAMELLIA_C",
#endif /* POLARSSL_CAMELLIA_C */
#if defined(POLARSSL_CCM_C)
    "POLARSSL_CCM_C",
#endif /* POLARSSL_CCM_C */
#if defined(POLARSSL_CERTS_C)
    "POLARSSL_CERTS_C",
#endif /* POLARSSL_CERTS_C */
#if defined(POLARSSL_CIPHER_C)
    "POLARSSL_CIPHER_C",
#endif /* POLARSSL_CIPHER_C */
#if defined(POLARSSL_CTR_DRBG_C)
    "POLARSSL_CTR_DRBG_C",
#endif /* POLARSSL_CTR_DRBG_C */
#if defined(POLARSSL_DEBUG_C)
    "POLARSSL_DEBUG_C",
#endif /* POLARSSL_DEBUG_C */
#if defined(POLARSSL_DES_C)
    "POLARSSL_DES_C",
#endif /* POLARSSL_DES_C */
#if defined(POLARSSL_DHM_C)
    "POLARSSL_DHM_C",
#endif /* POLARSSL_DHM_C */
#if defined(POLARSSL_ECDH_C)
    "POLARSSL_ECDH_C",
#endif /* POLARSSL_ECDH_C */
#if defined(POLARSSL_ECDSA_C)
    "POLARSSL_ECDSA_C",
#endif /* POLARSSL_ECDSA_C */
#if defined(POLARSSL_ECP_C)
    "POLARSSL_ECP_C",
#endif /* POLARSSL_ECP_C */
#if defined(POLARSSL_ENTROPY_C)
    "POLARSSL_ENTROPY_C",
#endif /* POLARSSL_ENTROPY_C */
#if defined(POLARSSL_ERROR_C)
    "POLARSSL_ERROR_C",
#endif /* POLARSSL_ERROR_C */
#if defined(POLARSSL_GCM_C)
    "POLARSSL_GCM_C",
#endif /* POLARSSL_GCM_C */
#if defined(POLARSSL_HAVEGE_C)
    "POLARSSL_HAVEGE_C",
#endif /* POLARSSL_HAVEGE_C */
#if defined(POLARSSL_HMAC_DRBG_C)
    "POLARSSL_HMAC_DRBG_C",
#endif /* POLARSSL_HMAC_DRBG_C */
#if defined(POLARSSL_MD_C)
    "POLARSSL_MD_C",
#endif /* POLARSSL_MD_C */
#if defined(POLARSSL_MD2_C)
    "POLARSSL_MD2_C",
#endif /* POLARSSL_MD2_C */
#if defined(POLARSSL_MD4_C)
    "POLARSSL_MD4_C",
#endif /* POLARSSL_MD4_C */
#if defined(POLARSSL_MD5_C)
    "POLARSSL_MD5_C",
#endif /* POLARSSL_MD5_C */
#if defined(POLARSSL_MEMORY_C)
    "POLARSSL_MEMORY_C",
#endif /* POLARSSL_MEMORY_C */
#if defined(POLARSSL_MEMORY_BUFFER_ALLOC_C)
    "POLARSSL_MEMORY_BUFFER_ALLOC_C",
#endif /* POLARSSL_MEMORY_BUFFER_ALLOC_C */
#if defined(POLARSSL_NET_C)
    "POLARSSL_NET_C",
#endif /* POLARSSL_NET_C */
#if defined(POLARSSL_OID_C)
    "POLARSSL_OID_C",
#endif /* POLARSSL_OID_C */
#if defined(POLARSSL_PADLOCK_C)
    "POLARSSL_PADLOCK_C",
#endif /* POLARSSL_PADLOCK_C */
#if defined(POLARSSL_PBKDF2_C)
    "POLARSSL_PBKDF2_C",
#endif /* POLARSSL_PBKDF2_C */
#if defined(POLARSSL_PEM_PARSE_C)
    "POLARSSL_PEM_PARSE_C",
#endif /* POLARSSL_PEM_PARSE_C */
#if defined(POLARSSL_PEM_WRITE_C)
    "POLARSSL_PEM_WRITE_C",
#endif /* POLARSSL_PEM_WRITE_C */
#if defined(POLARSSL_PK_C)
    "POLARSSL_PK_C",
#endif /* POLARSSL_PK_C */
#if defined(POLARSSL_PK_PARSE_C)
    "POLARSSL_PK_PARSE_C",
#endif /* POLARSSL_PK_PARSE_C */
#if defined(POLARSSL_PK_WRITE_C)
    "POLARSSL_PK_WRITE_C",
#endif /* POLARSSL_PK_WRITE_C */
#if defined(POLARSSL_PKCS5_C)
    "POLARSSL_PKCS5_C",
#endif /* POLARSSL_PKCS5_C */
#if defined(POLARSSL_PKCS11_C)
    "POLARSSL_PKCS11_C",
#endif /* POLARSSL_PKCS11_C */
#if defined(POLARSSL_PKCS12_C)
    "POLARSSL_PKCS12_C",
#endif /* POLARSSL_PKCS12_C */
#if defined(POLARSSL_PLATFORM_C)
    "POLARSSL_PLATFORM_C",
#endif /* POLARSSL_PLATFORM_C */
#if defined(POLARSSL_RIPEMD160_C)
    "POLARSSL_RIPEMD160_C",
#endif /* POLARSSL_RIPEMD160_C */
#if defined(POLARSSL_RSA_C)
    "POLARSSL_RSA_C",
#endif /* POLARSSL_RSA_C */
#if defined(POLARSSL_SHA1_C)
    "POLARSSL_SHA1_C",
#endif /* POLARSSL_SHA1_C */
#if defined(POLARSSL_SHA256_C)
    "POLARSSL_SHA256_C",
#endif /* POLARSSL_SHA256_C */
#if defined(POLARSSL_SHA512_C)
    "POLARSSL_SHA512_C",
#endif /* POLARSSL_SHA512_C */
#if defined(POLARSSL_SSL_CACHE_C)
    "POLARSSL_SSL_CACHE_C",
#endif /* POLARSSL_SSL_CACHE_C */
#if defined(POLARSSL_SSL_CLI_C)
    "POLARSSL_SSL_CLI_C",
#endif /* POLARSSL_SSL_CLI_C */
#if defined(POLARSSL_SSL_SRV_C)
    "POLARSSL_SSL_SRV_C",
#endif /* POLARSSL_SSL_SRV_C */
#if defined(POLARSSL_SSL_TLS_C)
    "POLARSSL_SSL_TLS_C",
#endif /* POLARSSL_SSL_TLS_C */
#if defined(POLARSSL_THREADING_C)
    "POLARSSL_THREADING_C",
#endif /* POLARSSL_THREADING_C */
#if defined(POLARSSL_TIMING_C)
    "POLARSSL_TIMING_C",
#endif /* POLARSSL_TIMING_C */
#if defined(POLARSSL_VERSION_C)
    "POLARSSL_VERSION_C",
#endif /* POLARSSL_VERSION_C */
#if defined(POLARSSL_X509_USE_C)
    "POLARSSL_X509_USE_C",
#endif /* POLARSSL_X509_USE_C */
#if defined(POLARSSL_X509_CRT_PARSE_C)
    "POLARSSL_X509_CRT_PARSE_C",
#endif /* POLARSSL_X509_CRT_PARSE_C */
#if defined(POLARSSL_X509_CRL_PARSE_C)
    "POLARSSL_X509_CRL_PARSE_C",
#endif /* POLARSSL_X509_CRL_PARSE_C */
#if defined(POLARSSL_X509_CSR_PARSE_C)
    "POLARSSL_X509_CSR_PARSE_C",
#endif /* POLARSSL_X509_CSR_PARSE_C */
#if defined(POLARSSL_X509_CREATE_C)
    "POLARSSL_X509_CREATE_C",
#endif /* POLARSSL_X509_CREATE_C */
#if defined(POLARSSL_X509_CRT_WRITE_C)
    "POLARSSL_X509_CRT_WRITE_C",
#endif /* POLARSSL_X509_CRT_WRITE_C */
#if defined(POLARSSL_X509_CSR_WRITE_C)
    "POLARSSL_X509_CSR_WRITE_C",
#endif /* POLARSSL_X509_CSR_WRITE_C */
#if defined(POLARSSL_XTEA_C)
    "POLARSSL_XTEA_C",
#endif /* POLARSSL_XTEA_C */
#endif /* POLARSSL_VERSION_FEATURES */
    NULL
};

int version_check_feature( const char *feature )
{
    const char **idx = features;

    if( *idx == NULL )
        return( -2 );

    if( feature == NULL )
        return( -1 );

    while( *idx != NULL )
    {
        if( !strcasecmp( *idx, feature ) )
            return( 0 );
        idx++;
    }
    return( -1 );
}

#endif /* POLARSSL_VERSION_C */

#endif /* USE_MBEDTLS */
