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

#include "heivs/hash_stm32_md5.h"


/* Implementation that should never be optimized out by the compiler */
static void polarssl_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}

#if defined(POLARSSL_MD5_ALT)


void md5_init( md5_context *ctx ) {

	memset( ctx, 0, sizeof( md5_context ) );

	hash_init(&hash_stm32_md5, ctx->state, sizeof(ctx->state));
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
    ctx->total[0] = 0;
    ctx->total[1] = 0;
}

/*
 * MD5 process buffer
 */
void md5_update( md5_context *ctx, const unsigned char *input, size_t ilen ) {
	hash_run(&hash_stm32_md5, ctx->state, (void*)input, ilen);
}

/*
 * MD5 final digest
 */
void md5_finish( md5_context *ctx, unsigned char output[16] ) {
	hash_finish(&hash_stm32_md5, ctx->state);	// hash_finish returns the data in the state pointer
	memcpy(output, ctx->state, 16);
}


void md5_process( md5_context *ctx, const unsigned char data[64] ) {
	printf("md5 process should not be called (Internal use).");
}

#endif /* POLARSSL_MD5_ALT */


#endif /* POLARSSL_MD5_C */

#endif /* USE_MBEDTLS */
