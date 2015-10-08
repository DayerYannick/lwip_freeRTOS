#ifndef LWIP_WRAPPER_IGMP_H
#define LWIP_WRAPPER_IGMP_H

#include "config/lwip_opt.h"
#if !LWIP_V6
#include "lwip/ipv4/lwip/igmp.h"
#else
#include "lwip/ipv6/lwip/igmp.h"
#endif

#endif /* LWIP_WRAPPER_IGMP_H */
