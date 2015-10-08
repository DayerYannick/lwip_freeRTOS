#ifndef __ETHERNETIF_WRAPPER_H__
#define __ETHERNETIF_WRAPPER_H__


#include "lwip/lwip/err.h"
#include "lwip/lwip/netif.h"

#include "heivs/config.h"
#if USE_FREERTOS
#include "lwip/arch/FreeRTOS/ethernetif.h"
#else
#include "lwip/arch/Standalone/ethernetif.h"
#endif

#endif 
