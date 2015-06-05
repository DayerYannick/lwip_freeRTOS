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

#if defined(POLARSSL_AES_C)

#include "mbedtls/aes_alt.h"
#if defined(POLARSSL_PADLOCK_C)
#include "mbedtls/padlock.h"
#endif
#if defined(POLARSSL_AESNI_C)
#include "mbedtls/aesni.h"
#endif

#if defined(POLARSSL_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#define polarssl_printf printf
#endif

#if defined(POLARSSL_AES_ALT)

#include "stm32/stm32f4xx_rcc.h"
#include "stm32/stm32f4xx_cryp.h"

/* Implementation that should never be optimized out by the compiler */
static void polarssl_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}


#if defined(POLARSSL_PADLOCK_C) &&                      \
    ( defined(POLARSSL_HAVE_X86) || defined(PADLOCK_ALIGN16) )
static int aes_padlock_ace = -1;
#endif




void aes_init( aes_context *ctx )
{
	printf("AES init.\n");
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_CRYP, ENABLE);
    memset( ctx, 0, sizeof( aes_context ) );
}

void aes_free( aes_context *ctx )
{
	printf("AES free.\n");

	//RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_CRYP, DISABLE); NO! if another ctx is active... at least, test that.

    if( ctx == NULL )
        return;

    polarssl_zeroize( ctx, sizeof( aes_context ) );
}

/*
 * AES key schedule (encryption)
 */
int aes_setkey_enc( aes_context *ctx, const unsigned char *key,
                    unsigned int keysize )
{
	printf("AES: set key enc: %x %x %x...\n", key[0], key[1], key[2]);
	ctx->dir = AES_ENCRYPT;
	ctx->keySize = keysize;
	memcpy(ctx->key, key, keysize);

    return( 0 );
}

/*
 * AES key schedule (decryption)
 */
int aes_setkey_dec( aes_context *ctx, const unsigned char *key,
                    unsigned int keysize )
{
	printf("AES: set key dec: %x %x %x...\n", key[0], key[1], key[2]);
	ctx->dir = AES_DECRYPT;
	ctx->keySize = keysize;
	memcpy(ctx->key, key, keysize);

    return( 0 );
}

/*
 * AES-ECB block encryption/decryption
 */
int aes_crypt_ecb( aes_context *ctx,
                    int mode,
                    const unsigned char input[16],
                    unsigned char output[16] )
{

	if(mode == AES_ENCRYPT) {
#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("TLS AES"), "Encrypt ECB");
#endif
		//printf("AES ecb: encrypt: %x %x %x...\n", ctx->key[0], ctx->key[1], ctx->key[2]);
	}
	else {
#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("TLS AES"), "Decrypt ECB");
#endif
		//printf("AES ecb: decrypt: %x %x %x...\n", ctx->key[0], ctx->key[1], ctx->key[2]);
	}

	if(ctx->dir != mode)
		printf("ERROR!! dir != mode. need 2 keys per aes_context\n");

	if( CRYP_AES_ECB(mode, ctx->key, ctx->keySize, (uint8_t*)input, 16, (uint8_t*)output) == ERROR ) {
		printf("ERROR in CRYP_AES_ECB.\n");
		return -1;
	}

#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("TLS AES"), "End encrypt/decrypt");
#endif
	//printf("AES ecb: crypt END.\n");


    return 0;
}

#if defined(POLARSSL_CIPHER_MODE_CBC)
/*
 * AES-CBC buffer encryption/decryption
 */
int aes_crypt_cbc( aes_context *ctx,
                    int mode,
                    size_t length,
                    unsigned char iv[16],
                    const unsigned char *input,
                    unsigned char *output )
{
	if(mode == AES_ENCRYPT)
		printf("AES cbc: crypt: %x %x %x...\n", ctx->key[0], ctx->key[1], ctx->key[2]);
	else
		printf("AES cbc: decrypt: %x %x %x...\n", ctx->key[0], ctx->key[1], ctx->key[2]);

	if(ctx->dir != mode)
		printf("ERROR!! dir != mode. need 2 keys per aes_context\n");

	if( CRYP_AES_CBC(mode, (uint8_t*)iv, ctx->key, ctx->keySize, (uint8_t*)input, length, (uint8_t*)output) == ERROR ) {
		printf("ERROR in CRYP_AES_CBC.\n");
		return -1;
	}

	printf("AES cbc: crypt/decrypt END.\n");

	return 0;
}
#endif /* POLARSSL_CIPHER_MODE_CBC */

#if defined(POLARSSL_CIPHER_MODE_CFB)
/*
 * AES-CFB128 buffer encryption/decryption
 */
int aes_crypt_cfb128( aes_context *ctx,
                       int mode,
                       size_t length,
                       size_t *iv_off,
                       unsigned char iv[16],
                       const unsigned char *input,
                       unsigned char *output )
{
    int c;
    size_t n = *iv_off;

    if( mode == AES_DECRYPT )
    {
        while( length-- )
        {
            if( n == 0 )
                aes_crypt_ecb( ctx, AES_ENCRYPT, iv, iv );

            c = *input++;
            *output++ = (unsigned char)( c ^ iv[n] );
            iv[n] = (unsigned char) c;

            n = ( n + 1 ) & 0x0F;
        }
    }
    else
    {
        while( length-- )
        {
            if( n == 0 )
                aes_crypt_ecb( ctx, AES_ENCRYPT, iv, iv );

            iv[n] = *output++ = (unsigned char)( iv[n] ^ *input++ );

            n = ( n + 1 ) & 0x0F;
        }
    }

    *iv_off = n;

    return( 0 );
}

/*
 * AES-CFB8 buffer encryption/decryption
 */
#include <stdio.h>
int aes_crypt_cfb8( aes_context *ctx,
                       int mode,
                       size_t length,
                       unsigned char iv[16],
                       const unsigned char *input,
                       unsigned char *output )
{
    unsigned char c;
    unsigned char ov[17];

    while( length-- )
    {
        memcpy( ov, iv, 16 );
        aes_crypt_ecb( ctx, AES_ENCRYPT, iv, iv );

        if( mode == AES_DECRYPT )
            ov[16] = *input;

        c = *output++ = (unsigned char)( iv[0] ^ *input++ );

        if( mode == AES_ENCRYPT )
            ov[16] = c;

        memcpy( iv, ov + 1, 16 );
    }

    return( 0 );
}
#endif /*POLARSSL_CIPHER_MODE_CFB */

#if defined(POLARSSL_CIPHER_MODE_CTR)
/*
 * AES-CTR buffer encryption/decryption
 */
int aes_crypt_ctr( aes_context *ctx,
                       size_t length,
                       size_t *nc_off,
                       unsigned char nonce_counter[16],
                       unsigned char stream_block[16],
                       const unsigned char *input,
                       unsigned char *output )
{
	printf("AES ctr: crypt/decrypt: %x %x %x...\n", ctx->key[0], ctx->key[1], ctx->key[2]);

	if( CRYP_AES_CTR(ctx->dir, nonce_counter, ctx->key, ctx->keySize, (uint8_t*)input, length, (uint8_t*)output) == ERROR ) {
		printf("ERROR in CRYP_AES_CTR.\n");
		return -1;
	}

	printf("AES ctr: crypt/decrypt END.\n");

    return 0;
}
#endif /* POLARSSL_CIPHER_MODE_CTR */

#endif /* POLARSSL_AES_ALT */



#endif /* POLARSSL_AES_C */

#endif /* USE_MBEDTLS */
