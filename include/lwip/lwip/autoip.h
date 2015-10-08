#ifndef LWIP_WRAPPER_AUTOIP_H
#define LWIP_WRAPPER_AUTOIP_H

#include "config/lwip_opt.h"
#if !LWIP_V6
#include "lwip/ipv4/lwip/autoip.h"
#else
#include "lwip/ipv6/lwip/autoip.h"
#endif

#endif /* LWIP_WRAPPER_AUTOIP_H */
