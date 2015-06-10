/*
 * Uses the CRYP module of the stm32f417 on the ARMEBS4 board
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

#if defined(POLARSSL_DES_C)

#include "mbedtls/des_alt.h"

#if defined(POLARSSL_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#define polarssl_printf printf
#endif

#if defined(POLARSSL_DES_ALT)

#include "stm32/stm32f4xx_rcc.h"
#include "stm32/stm32f4xx_cryp.h"

#include "freertos/FreeRTOS.h"

/* Implementation that should never be optimized out by the compiler */
static void polarssl_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}

void des_init( des_context *ctx )
{
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_CRYP, ENABLE);
    memset( ctx, 0, sizeof( des_context ) );
}

void des_free( des_context *ctx )
{
    if( ctx == NULL )
        return;

    polarssl_zeroize( ctx, sizeof( des_context ) );
}

void des3_init( des3_context *ctx )
{
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_CRYP, ENABLE);
    memset( ctx, 0, sizeof( des3_context ) );
}

void des3_free( des3_context *ctx )
{
    if( ctx == NULL )
        return;

    polarssl_zeroize( ctx, sizeof( des3_context ) );
}

static const unsigned char odd_parity_table[128] = { 1,  2,  4,  7,  8,
        11, 13, 14, 16, 19, 21, 22, 25, 26, 28, 31, 32, 35, 37, 38, 41, 42, 44,
        47, 49, 50, 52, 55, 56, 59, 61, 62, 64, 67, 69, 70, 73, 74, 76, 79, 81,
        82, 84, 87, 88, 91, 93, 94, 97, 98, 100, 103, 104, 107, 109, 110, 112,
        115, 117, 118, 121, 122, 124, 127, 128, 131, 133, 134, 137, 138, 140,
        143, 145, 146, 148, 151, 152, 155, 157, 158, 161, 162, 164, 167, 168,
        171, 173, 174, 176, 179, 181, 182, 185, 186, 188, 191, 193, 194, 196,
        199, 200, 203, 205, 206, 208, 211, 213, 214, 217, 218, 220, 223, 224,
        227, 229, 230, 233, 234, 236, 239, 241, 242, 244, 247, 248, 251, 253,
        254 };

void des_key_set_parity( unsigned char key[DES_KEY_SIZE] )
{
    int i;

    for( i = 0; i < DES_KEY_SIZE; i++ )
        key[i] = odd_parity_table[key[i] / 2];
}

/*
 * Check the given key's parity, returns 1 on failure, 0 on SUCCESS
 */
int des_key_check_key_parity( const unsigned char key[DES_KEY_SIZE] )
{
    int i;

    for( i = 0; i < DES_KEY_SIZE; i++ )
        if( key[i] != odd_parity_table[key[i] / 2] )
            return( 1 );

    return( 0 );
}

/*
 * Table of weak and semi-weak keys
 *
 * Source: http://en.wikipedia.org/wiki/Weak_key
 *
 * Weak:
 * Alternating ones + zeros (0x0101010101010101)
 * Alternating 'F' + 'E' (0xFEFEFEFEFEFEFEFE)
 * '0xE0E0E0E0F1F1F1F1'
 * '0x1F1F1F1F0E0E0E0E'
 *
 * Semi-weak:
 * 0x011F011F010E010E and 0x1F011F010E010E01
 * 0x01E001E001F101F1 and 0xE001E001F101F101
 * 0x01FE01FE01FE01FE and 0xFE01FE01FE01FE01
 * 0x1FE01FE00EF10EF1 and 0xE01FE01FF10EF10E
 * 0x1FFE1FFE0EFE0EFE and 0xFE1FFE1FFE0EFE0E
 * 0xE0FEE0FEF1FEF1FE and 0xFEE0FEE0FEF1FEF1
 *
 */

#define WEAK_KEY_COUNT 16

static const unsigned char weak_key_table[WEAK_KEY_COUNT][DES_KEY_SIZE] =
{
    { 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 },
    { 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE },
    { 0x1F, 0x1F, 0x1F, 0x1F, 0x0E, 0x0E, 0x0E, 0x0E },
    { 0xE0, 0xE0, 0xE0, 0xE0, 0xF1, 0xF1, 0xF1, 0xF1 },

    { 0x01, 0x1F, 0x01, 0x1F, 0x01, 0x0E, 0x01, 0x0E },
    { 0x1F, 0x01, 0x1F, 0x01, 0x0E, 0x01, 0x0E, 0x01 },
    { 0x01, 0xE0, 0x01, 0xE0, 0x01, 0xF1, 0x01, 0xF1 },
    { 0xE0, 0x01, 0xE0, 0x01, 0xF1, 0x01, 0xF1, 0x01 },
    { 0x01, 0xFE, 0x01, 0xFE, 0x01, 0xFE, 0x01, 0xFE },
    { 0xFE, 0x01, 0xFE, 0x01, 0xFE, 0x01, 0xFE, 0x01 },
    { 0x1F, 0xE0, 0x1F, 0xE0, 0x0E, 0xF1, 0x0E, 0xF1 },
    { 0xE0, 0x1F, 0xE0, 0x1F, 0xF1, 0x0E, 0xF1, 0x0E },
    { 0x1F, 0xFE, 0x1F, 0xFE, 0x0E, 0xFE, 0x0E, 0xFE },
    { 0xFE, 0x1F, 0xFE, 0x1F, 0xFE, 0x0E, 0xFE, 0x0E },
    { 0xE0, 0xFE, 0xE0, 0xFE, 0xF1, 0xFE, 0xF1, 0xFE },
    { 0xFE, 0xE0, 0xFE, 0xE0, 0xFE, 0xF1, 0xFE, 0xF1 }
};

int des_key_check_weak( const unsigned char key[DES_KEY_SIZE] )
{
    int i;

    for( i = 0; i < WEAK_KEY_COUNT; i++ )
        if( memcmp( weak_key_table[i], key, DES_KEY_SIZE) == 0 )
            return( 1 );

    return( 0 );
}

/*
 * DES key schedule (56-bit, encryption)
 */
int des_setkey_enc( des_context *ctx, const unsigned char key[DES_KEY_SIZE] )
{
	memcpy(ctx->key, key, 8);
    return 0;
}

/*
 * DES key schedule (56-bit, decryption)
 */
int des_setkey_dec( des_context *ctx, const unsigned char key[DES_KEY_SIZE] )
{
	memcpy(ctx->key, key, 8);
    return 0;
}

/*
 * Triple-DES key schedule (112-bit, encryption)
 */
int des3_set2key_enc( des3_context *ctx,
                      const unsigned char key[DES_KEY_SIZE * 2] )
{
	memcpy(ctx->key, key, 16);		// [0-7]:Key1, [8-15]:Key2
	memcpy(ctx->key+16, key, 8);	// [16-23]:Key3 = Key1
    return 0;
}

/*
 * Triple-DES key schedule (112-bit, decryption)
 */
int des3_set2key_dec( des3_context *ctx,
                      const unsigned char key[DES_KEY_SIZE * 2] )
{
	memcpy(ctx->key, key, 16);		// [0-7]:Key1, [8-15]:Key2
	memcpy(ctx->key+16, key, 8);	// [16-23]:Key3 = Key1
    return 0;
}

/*
 * Triple-DES key schedule (168-bit, encryption)
 */
int des3_set3key_enc( des3_context *ctx,
                      const unsigned char key[DES_KEY_SIZE * 3] )
{
	memcpy(ctx->key, key, 24);

    return 0;
}

/*
 * Triple-DES key schedule (168-bit, decryption)
 */
int des3_set3key_dec( des3_context *ctx,
                      const unsigned char key[DES_KEY_SIZE * 3] )
{
	memcpy(ctx->key, key, 24);

    return 0;
}

/*
 * DES-ECB block encryption/decryption
 */
int des_crypt_ecb( des_context *ctx,
                    const unsigned char input[8],
                    unsigned char output[8] )
{
	uint8_t constResp[8];
	memcpy(constResp, input, 8);

	CRYP_DES_ECB(0, ctx->key, constResp, 8, output);

    return 0;
}

#if defined(POLARSSL_CIPHER_MODE_CBC)
/*
 * DES-CBC buffer encryption/decryption
 */
int des_crypt_cbc( des_context *ctx,
                    int mode,
                    size_t length,
                    unsigned char iv[8],
                    const unsigned char *input,
                    unsigned char *output )
{
	uint8_t* constResp;
	constResp = pvPortMalloc(length);
	memcpy(constResp, input, length);

    if( length % 8 )
        return( POLARSSL_ERR_DES_INVALID_INPUT_LENGTH );

    CRYP_DES_CBC(mode, ctx->key, iv, constResp, length, output);

    vPortFree(constResp);

    return 0;
}
#endif /* POLARSSL_CIPHER_MODE_CBC */

/*
 * 3DES-ECB block encryption/decryption
 */
int des3_crypt_ecb( des3_context *ctx,
                     const unsigned char input[8],
                     unsigned char output[8] )
{
	uint8_t constResp[8];
	memcpy(constResp, input, 8);

	CRYP_TDES_ECB(0, ctx->key, constResp, 8, output);
    return 0;
}

#if defined(POLARSSL_CIPHER_MODE_CBC)
/*
 * 3DES-CBC buffer encryption/decryption
 */
int des3_crypt_cbc( des3_context *ctx,
                     int mode,
                     size_t length,
                     unsigned char iv[8],
                     const unsigned char *input,
                     unsigned char *output )
{
   uint8_t* constResp;
   constResp = pvPortMalloc(length);

   CRYP_TDES_CBC(mode, ctx->key, iv, constResp, length, output);

   vPortFree(constResp);

    return 0;
}
#endif /* POLARSSL_CIPHER_MODE_CBC */

#endif /* POLARSSL_DES_ALT */

#endif /* POLARSSL_DES_C */

#endif /* USE_MBEDTLS */
