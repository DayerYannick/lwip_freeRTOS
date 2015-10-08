#ifndef __SYS_ARCH_WRAPPER_H__
#define __SYS_ARCH_WRAPPER_H__


#include "lwip/lwip/err.h"
#include "lwip/lwip/netif.h"

#include "heivs/config.h"
#if USE_FREERTOS
#include "lwip/arch/FreeRTOS/sys_arch.h"
#else
#include "lwip/arch/Standalone/sys_arch.h"
#endif

#endif
