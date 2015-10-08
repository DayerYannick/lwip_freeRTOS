#ifndef LWIP_WRAPPER_ICMP_H
#define LWIP_WRAPPER_ICMP_H

#include "config/lwip_opt.h"
#if !LWIP_V6
#include "lwip/ipv4/lwip/icmp.h"
#else
#include "lwip/lwip/icmp.h"
#endif

#endif /* LWIP_WRAPPER_ICMP_H */
