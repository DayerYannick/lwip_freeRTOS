/**
 * @file md5_alt.h
 *
 * @brief MD5 message digest algorithm (hash function), alternative implementation for the ARMEBS4 board
 *
 * @author Dayer Yannick
 *
 */

#ifndef POLARSSL_MD5_ALT_H
#define POLARSSL_MD5_ALT_H

#if !defined(POLARSSL_CONFIG_FILE)
#include "config/mbedTLSConfig.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#include <string.h>

#if defined(_MSC_VER) && !defined(EFIX64) && !defined(EFI32)
#include <basetsd.h>
typedef UINT32 uint32_t;
#else
#include <inttypes.h>
#endif

#if defined(POLARSSL_MD5_ALT)

#ifdef __cplusplus
extern "C" {
#endif


/**
 * \brief          MD5 context structure
 */
typedef struct
{
    unsigned char buffer[16];	/* holds the digest until finish() is called */
    uint8_t key[64];
    uint8_t keyLen;
}
md5_context;

/**
 * \brief          Initialize MD5 context
 *
 * \param ctx      MD5 context to be initialized
 */
void md5_init( md5_context *ctx );

/**
 * \brief          Clear MD5 context
 *
 * \param ctx      MD5 context to be cleared
 */
void md5_free( md5_context *ctx );

/**
 * \brief          MD5 context setup
 *
 * \param ctx      context to be initialized
 */
void md5_starts( md5_context *ctx );

/**
 * \brief          MD5 process buffer
 *
 * \param ctx      MD5 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void md5_update( md5_context *ctx, const unsigned char *input, size_t ilen );

/**
 * \brief          MD5 final digest
 *
 * \param ctx      MD5 context
 * \param output   MD5 checksum result
 */
void md5_finish( md5_context *ctx, unsigned char output[16] );

/* Internal use */
void md5_process( md5_context *ctx, const unsigned char data[64] );



/* MD5 HMAC */

/* Internal use */
void md5_process( md5_context *ctx, const unsigned char data[64] );


/**
 * \brief          MD5 HMAC context setup
 *
 * \param ctx      HMAC context to be initialized
 * \param key      HMAC secret key
 * \param keylen   length of the HMAC key
 */
void md5_hmac_starts( md5_context *ctx,
                      const unsigned char *key, size_t keylen );

/**
 * \brief          MD5 HMAC process buffer
 *
 * \param ctx      HMAC context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void md5_hmac_update( md5_context *ctx,
                      const unsigned char *input, size_t ilen );

/**
 * \brief          MD5 HMAC final digest
 *
 * \param ctx      HMAC context
 * \param output   MD5 HMAC checksum result
 */
void md5_hmac_finish( md5_context *ctx, unsigned char output[16] );

/**
 * \brief          MD5 HMAC context reset
 *
 * \param ctx      HMAC context to be reset
 */
void md5_hmac_reset( md5_context *ctx );

#ifdef __cplusplus
}
#endif

#endif /* defined(POLARSSL_MD5_ALT) */


#endif /* md5.h */
