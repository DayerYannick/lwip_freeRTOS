#ifndef LWIP_WRAPPER_INET_H
#define LWIP_WRAPPER_INET_H

#include "config/lwip_opt.h"
#if !LWIP_V6
#include "lwip/ipv4/lwip/inet.h"
#else
#include "lwip/ipv6/lwip/inet.h"
#endif

#endif /* LWIP_WRAPPER_INET_H */
