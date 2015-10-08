#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__


#include "lwip/lwip/err.h"
#include "lwip/lwip/netif.h"

err_t ethernetif_init(struct netif *netif);
err_t ethernetif_input(struct netif *netif);

#endif
