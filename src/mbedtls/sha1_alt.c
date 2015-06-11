/*
 * Uses the hardware CRYP module of the stm32f417 on the ARMEBS4 board.
 *
 * @author Dayer Yannick
 */

#include "heivs/config.h"
#if USE_MBEDTLS

#if !defined(POLARSSL_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#if defined(POLARSSL_SHA1_C)

#include "mbedtls/sha1_alt.h"

#if defined(POLARSSL_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#define polarssl_printf printf
#endif


#if defined(POLARSSL_SHA1_ALT)

#include "freertos/FreeRTOS.h"

/* Implementation that should never be optimized out by the compiler */
static void polarssl_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}

#include "stm32/stm32f4xx_rcc.h"
#include "stm32/stm32f4xx_hash.h"

void sha1_init( sha1_context *ctx )
{
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_HASH, ENABLE);
    memset( ctx, 0, sizeof( sha1_context ) );
}

void sha1_free( sha1_context *ctx )
{
    if( ctx == NULL )
        return;

    polarssl_zeroize( ctx, sizeof( sha1_context ) );
}

/*
 * SHA-1 context setup
 */
void sha1_starts( sha1_context *ctx )
{
	return;
}

/*
 * SHA-1 process buffer
 */
void sha1_update( sha1_context *ctx, const unsigned char *input, size_t ilen )
{
	unsigned char* constRespect;			// Used to respect the const qualifier :(
	constRespect = pvPortMalloc(ilen);		//
	memcpy(constRespect, input, ilen);		//

	if(HASH_SHA1(constRespect, ilen, ctx->buffer) == ERROR)	// Stores the digest in ctx.buffer
		printf("ERROR in HASH_SHA1 function.");

	vPortFree(constRespect);
}

/*
 * SHA-1 final digest
 */
void sha1_finish( sha1_context *ctx, unsigned char output[20] )
{
	memcpy(output, ctx->buffer, 20);	// Restore the digest from ctx.buffer
}

void sha1_process( sha1_context *ctx, const unsigned char data[64] )
{
	printf("sha1_process should not be called (Internal use).");
}


/*
 * HMAC SHA-1
 */

/*
 * SHA-1 HMAC context setup
 */
void sha1_hmac_starts( sha1_context *ctx, const unsigned char *key,
                       size_t keylen )
{

    if( keylen > 64 )	// Key shortened if greater than BLOCK SIZE (64 bytes)
    {
    	sha1_context cty;
    	sha1_init(&cty);
    	sha1_starts(&cty);
        sha1_update(&cty, key, keylen);	// Get the 20 char digest of the key
        sha1_finish(&cty, ctx->key);	// Store the digest in ctx.key
        sha1_free(&cty);
        ctx->keyLen = 20;
    }
    else {
    	ctx->keyLen = keylen;			// Use the key as is
		memcpy(ctx->key, key, keylen);	// Store the key in ctx.key
    }

}

/*
 * SHA-1 HMAC process buffer
 */
void sha1_hmac_update( sha1_context *ctx, const unsigned char *input,
                       size_t ilen )
{
	unsigned char* constRespect;		// Used to respect the const qualifier :(
	constRespect = pvPortMalloc(ilen);	//
	memcpy(constRespect, input, ilen);	//

	HMAC_SHA1(ctx->key, ctx->keyLen, constRespect, ilen, ctx->buffer);

	vPortFree(constRespect);
}

/*
 * SHA-1 HMAC final digest
 */
void sha1_hmac_finish( sha1_context *ctx, unsigned char output[20] )
{
	memcpy(output, ctx->buffer, 20);
}

/*
 * SHA1 HMAC context reset
 */
void sha1_hmac_reset( sha1_context *ctx )
{
	return;
}

#endif /* !POLARSSL_SHA1_ALT */


#endif /* POLARSSL_SHA1_C */

#endif /* USE_MBEDTLS */
