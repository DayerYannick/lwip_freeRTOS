/*
 * @brief md5 alternative functions to use the stm32f4 hardware on the ARMEBS4 board.
 *
 * @author Dayer Yannick
 *
 *	This file uses the heivs "driver" to access the hardware hash module of
 * the stm32f417 on the ARMEBS4 board.
 *
 *	TODO ensure thread-safe with a mutex (maybe)
 */

#include "heivs/config.h"
#if USE_MBEDTLS

#if !defined(POLARSSL_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#include <stdio.h>



#if defined(POLARSSL_MD5_C)

#include "mbedtls/md5_alt.h"

//#include "heivs/hash_stm32_md5.h"
#include "stm32/stm32f4xx_rcc.h"
#include "stm32/stm32f4xx_hash.h"
#include "freertos/FreeRTOS.h"


/* Implementation that should never be optimized out by the compiler */
static void polarssl_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}

#if defined(POLARSSL_MD5_ALT)


void md5_init( md5_context *ctx ) {

	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_HASH, ENABLE);
	memset( ctx, 0, sizeof( md5_context ) );

	//hash_init(&hash_stm32_md5, ctx->state, sizeof(ctx->state));
}

void md5_free( md5_context *ctx ) {
    if( ctx == NULL )
        return;

    polarssl_zeroize( ctx, sizeof( md5_context ) );
}

/*
 * MD5 context setup
 */
void md5_starts( md5_context *ctx ) {

	return;
}

/*
 * MD5 process buffer
 */
void md5_update( md5_context *ctx, const unsigned char *input, size_t ilen ) {
	/*
	uint8_t* constResp;
	constResp = pvPortMalloc(ilen);
	memcpy(constResp, input, ilen);

	HASH_MD5(constResp, ilen, ctx->buffer);
	//hash_run(&hash_stm32_md5, ctx->state, (void*)input, ilen);
	vPortFree(constResp);
	*/
	HASH_MD5((unsigned char*) input, ilen, ctx->buffer);
}

/*
 * MD5 final digest
 */
void md5_finish( md5_context *ctx, unsigned char output[16] ) {
	memcpy(output, ctx->buffer, 16);
}


void md5_process( md5_context *ctx, const unsigned char data[64] ) {
	printf("md5_process should not be called (Internal use).");
}


/*
 * MD5 HMAC context setup
 */
void md5_hmac_starts( md5_context *ctx, const unsigned char *key, size_t keylen ) {
    if( keylen > 64 )
    {
    	md5_context cty;
    	md5_init(&cty);
    	md5_starts(&cty);
    	md5_update(&cty, key, keylen);
    	md5_finish(&cty, ctx->key);
    	md5_free(&cty);
    	ctx->keyLen = 16;
    }
    else {
    	memcpy(ctx->key, key, keylen);
    	ctx->keyLen = keylen;
    }

}

/*
 * MD5 HMAC process buffer
 */
void md5_hmac_update( md5_context *ctx, const unsigned char *input, size_t ilen ) {
	/*uint8_t* constResp;
	constResp = pvPortMalloc(ilen);
	memcpy(constResp, input, ilen);

	HMAC_MD5(ctx->key, ctx->keyLen, constResp, ilen, ctx->buffer);

	vPortFree(constResp);
	*/
	HMAC_MD5(ctx->key, ctx->keyLen, (unsigned char*) input, ilen, ctx->buffer);

}

/*
 * MD5 HMAC final digest
 */
void md5_hmac_finish( md5_context *ctx, unsigned char output[16] ) {
	memcpy(output, ctx->buffer, 16);
}

/*
 * MD5 HMAC context reset
 */
void md5_hmac_reset( md5_context *ctx ) {
	return;
}

#endif /* POLARSSL_MD5_ALT */


#endif /* POLARSSL_MD5_C */

#endif /* USE_MBEDTLS */
