/*
 * demos.h
 *
 *	Sets different parameters for the demos
 *
 *
 *  Created on: 22 juin 2015
 *      Author: yannick.dayer
 */

#ifndef DEMOS_H_
#define DEMOS_H_

#include "lwip/lwip.h"
#include "freertos/FreeRTOS.h"



/*============================================================================*/
/*------------------------------ ADDRESS CONFIG ------------------------------*/
/*============================================================================*/

#define MY_IP_BY_DHCP 1	/* 0: use MY_IP / 1: use DHCP */


#if !MY_IP_BY_DHCP
#define MY_IP "192.168.1.5"
#define MY_MASK "255.255.255.0"
#define MY_GW "192.168.1.1"
#define PC_IP "192.168.1.2"
#else
#define PC_IP "153.109.5.95"
#endif

#define MY_HOSTNAME "lwip_dayer"

#define TCP_PORT 4433



/*============================================================================*/
/*------------------------------ DEMO SELECTION ------------------------------*/
/*============================================================================*/
/* Set ONE of these macros to 1 to activate the demo, the others to 0 */
#define DEMO_CLIENT 		1	/* Connects to PC_IP and send messages */
#define DEMO_SERVER 		0	/* Accepts incoming connections */
#define DEMO_HTTP_CLIENT	0	/* Sends a request to http://www.hevs.ch */		/* TODO Display picture */

#define DEMO_CLIENT_UDP		0	/* Sends messages really fast */

#define DEMO_TLS_CLIENT		0	/* Connection to a secured server via TLS */
#define DEMO_TLS_SERVER		0	/* Allows clients to connect via TLS */
#define DEMO_HTTPS_CLIENT	0	/* Send a request to https://www.google.ch */	/* TODO  */



/*============================================================================*/
/*----------------------------- DISPLAY & AUDIO ------------------------------*/
/*============================================================================*/
/* Set the output options */
#define USE_DISPLAY			1	/* 1 to display infos (console, messages, ...)*/
#define USE_AUDIO			0	/* 1 to play sound on important events */






// Declarations

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

typedef struct imgStruct {
	size_t size;
	char* img;
} imgStruct_t;



#endif /* DEMOS_H_ */
