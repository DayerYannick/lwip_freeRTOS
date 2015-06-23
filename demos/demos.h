/*
 * demos.h
 *
 *  Created on: 22 juin 2015
 *      Author: yannick.dayer
 */

#ifndef DEMOS_H_
#define DEMOS_H_

#include "lwip/lwip.h"
#include "freertos/FreeRTOS.h"


#define MY_IP_BY_DHCP 1	/* 0: use MY_IP / 1: use DHCP */


#if !MY_IP_BY_DHCP
#define MY_IP "192.168.1.5"
#define MY_MASK "255.255.255.0"
#define MY_GW "192.168.1.1"
#define PC_IP "192.168.1.2"
#else
#define PC_IP "153.109.5.178"
#endif

#define MY_HOSTNAME "lwip_dayer"

#define TCP_PORT 5001 /*4433*/


/*============================================================================*/
/*------------------------------ DEMO SELECTION ------------------------------*/
/*============================================================================*/
/* Set one of the macro to 1 to activate the demo */
#define DEMO_CLIENT 		0	/* Connect to PC_IP and send messages */
#define DEMO_SERVER 		1	/* Accept incoming connections */
#define DEMO_HTTP_CLIENT	0	/* Send a request to http://www.hevs.ch */
#define DEMO_HTTP_SERVER	0	/* A (very) simple HTTP server... */

#define DEMO_TLS_CLIENT		0	/* Connection to a secured server via TLS */
#define DEMO_TLS_SERVER		0	/* Allows a client to connect via TLS */
#define DEMO_HTTPS_SERVER	0	/* Send a request to https://www.google.ch */


#define USE_DISPLAY			1
#define USE_AUDIO			0




void main_task(void* param);




#if USE_DISPLAY
#include "ugfx/gfx.h"
void lcd_task(void* param);

QueueHandle_t LCD_msgQueue;
typedef struct {
	uint8_t type;
	char* ptr;
	unsigned int tick;
} queueLCDMsg_t;
#endif	/* USE_DISPLAY */


#if USE_AUDIO
#include "heivs/audio.h"
void audio_task(void* param);

QueueHandle_t AUDIO_msgQueue;
typedef struct {
	unsigned int pitch;
	unsigned int duration;
} queueAUDIOMsg_t;
#endif	/* USE_AUDIO */




#endif /* DEMOS_H_ */
