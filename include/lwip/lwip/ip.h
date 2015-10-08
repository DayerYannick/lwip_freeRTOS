#ifndef LWIP_WRAPPER_IP_H
#define LWIP_WRAPPER_IP_H

#include "config/lwip_opt.h"
#if !LWIP_V6
#include "lwip/ipv4/lwip/ip.h"
#else
#include "lwip/ipv6/lwip/ip.h"
#endif

#endif /* LWIP_WRAPPER_IP_H */
