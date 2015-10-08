#ifndef LWIP_WRAPPER_IP_FRAG_H
#define LWIP_WRAPPER_IP_FRAG_H

#include "config/lwip_opt.h"
#if !LWIP_V6
#include "lwip/ipv4/lwip/ip_frag.h"
#else
#include "lwip/ipv6/lwip/ip_frag.h"
#endif

#endif /* LWIP_WRAPPER_IP_FRAG_H */
