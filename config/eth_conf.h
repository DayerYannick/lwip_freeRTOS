/****************************************************************************/
/**
 * \file		eth_conf.h
 * \brief		Ethernet configuration
 *
 * \author		marc dot pignat at hevs dot ch
 * \copyright	Copyright HES-SO Valais/Wallis 2013. All rights reserved.
 * \license		CC0 - public domain, this is a configuration file
 *
 * This file MUST be provided by every project using LWIP, in the "config/" directory.
 *
 * \see example in libheivs/include/heivs/samples/eth_conf.h.txt
 */
/****************************************************************************/

#ifndef CONFIG_ETH_CONF_H
#define CONFIG_ETH_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32/stm32f4xx.h"

/**
 * Provide our own delay for the stm32_eth* functions
 */
#define USE_Delay

#include "heivs/delay.h"
#define _eth_delay_    delay_wait_us

/**
 * Configure ethernet buffers
 */
#define ETH_RX_BUF_SIZE    ETH_MAX_PACKET_SIZE
#define ETH_TX_BUF_SIZE    ETH_MAX_PACKET_SIZE
#define ETH_RXBUFNB        8
#define ETH_TXBUFNB        8

#define USE_ENHANCED_DMA_DESCRIPTORS

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_ETH_CONF_H */
