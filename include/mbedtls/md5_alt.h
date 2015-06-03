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


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          MD5 context structure
 */
typedef struct
{
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[4];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */

    unsigned char ipad[64];     /*!< HMAC: inner padding        */
    unsigned char opad[64];     /*!< HMAC: outer padding        */
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

#ifdef __cplusplus
}
#endif



#endif /* md5.h */
