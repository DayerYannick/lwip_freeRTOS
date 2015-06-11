/**
 * @file threading.h
 *
 * @author Dayer Yannick
 */
#ifndef POLARSSL_THREADING_ALT_H
#define POLARSSL_THREADING_ALT_H

#if !defined(POLARSSL_CONFIG_FILE)
#include "config/mbedTLSConfig.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef SemaphoreHandle_t threading_mutex_t;


int polarssl_mutex_init_func( threading_mutex_t *mutex );
int polarssl_mutex_free_func( threading_mutex_t *mutex );
int polarssl_mutex_lock_func( threading_mutex_t *mutex );
int polarssl_mutex_unlock_func( threading_mutex_t *mutex );

#ifdef __cplusplus
}
#endif

#endif /* threading_alt.h */
