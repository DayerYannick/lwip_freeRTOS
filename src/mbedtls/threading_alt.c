/*
 *  @author Dayer Yannick
 */

#include "heivs/config.h"
#if USE_MBEDTLS

#if !defined(POLARSSL_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#if defined(POLARSSL_THREADING_C)

#include "mbedtls/threading_alt.h"

#if defined(POLARSSL_THREADING_ALT)

int polarssl_mutex_init_func( threading_mutex_t *mutex ) {
	*mutex = NULL;
	*mutex = xSemaphoreCreateMutexNamed("mbedtls mutex");

	if(*mutex == NULL)
		return -1;
	return 0;
}

int polarssl_mutex_free_func( threading_mutex_t *mutex ) {
	vSemaphoreDelete(*mutex);
	*mutex = NULL;
	return 0;
}

int polarssl_mutex_lock_func( threading_mutex_t *mutex ) {
	xSemaphoreTake(*mutex, portMAX_DELAY);
	return 0;
}

int polarssl_mutex_unlock_func( threading_mutex_t *mutex ) {
	xSemaphoreGive(*mutex);
	return 0;
}

#endif /* POLARSSL_THREADING_ALT_C */

#endif /* POLARSSL_THREADING_C */

#endif /* USE_MBEDTLS */
