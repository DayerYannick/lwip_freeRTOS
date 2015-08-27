/*
 * server.c
 *
 *	Accept incoming connection and display received messages
 *
 *  Created on: 22 juin 2015
 *      Author: yannick.dayer
 */


#include "demos/demos.h"

#if DEMO_PRESENTATION

#include <stdlib.h>
#include <ctype.h>


#define MSG_LEN_MAX 2000	// The maximum length of a message when receiving

#define ClNbMax 6	// The maximum number of simultaneous connected clients
volatile int s[ClNbMax]; // An array of socket listing all the connected clients

void server_thread(void* param);



/*============================================================================*/
/*-------------------------------- MAIN TASK ---------------------------------*/
/*============================================================================*/

void main_task(void* param) {
	int socket;
	int i;
	const int serverThreadStackSize = ((MSG_LEN_MAX/configMINIMAL_STACK_SIZE)+2) * configMINIMAL_STACK_SIZE;


/*-------------------- INIT --------------------*/

#if MY_IP_BY_DHCP

	// Init the network interface and wait for a DHCP address
	while(lwip_init_DHCP(0, MY_HOSTNAME) != 0);	// Repeat in case of error...
	lwip_wait_events(EV_LWIP_IP_ASSIGNED, portMAX_DELAY);
	#if !USE_DISPLAY
		printf("My ip: %s\n", getMyIP());
	#endif

#else	/* MY_IP_BY_DHCP */

	// Init the network interface with a given IP address
	lwip_init_static(MY_IP, MY_MASK, MY_GW, MY_HOSTNAME);

#endif	/* MY_IP_BY_DHCP */


/*---------------- START SERVER ----------------*/

	socket = simpleSocket(TCP);
	if(socket == -1)
		printf("ERROR while creating socket\n");

	for(i=0;i<ClNbMax;++i) {
		s[i] = -1;
	}

	if(simpleBind(socket, getMyIP(), TCP_PORT) == -1)
		printf("ERROR in Bind\n");

	if(simpleListen(socket) == -1)
		printf("ERROR in Listen\n");

	printf("accepting...\n");

#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("main"), "Accepting");
#endif


	while(1) {
		int tempS = simpleAccept(socket);

		if(tempS < 0)
			printf("ERROR in Accept.\n");
		else {
			for(i=0;i<ClNbMax;++i)
				if(s[i] == -1)
					break;

			if(i==ClNbMax) {
				printf("ERROR: Cannot create socket: too many clients.");
				simpleClose(tempS);
				printf("Client closed.\n");
			}
			else {
				s[i] = tempS;
				xTaskCreate(server_thread, "Client Handle", serverThreadStackSize, (void*)i, uxTaskPriorityGet(NULL), NULL);
			}
		}
	}


}	/* main_task */



/*============================================================================*/
/*------------------------------ SERVER THREAD -------------------------------*/
/*============================================================================*/

uint8_t detectEOF(const char* msg, int len);
uint8_t messageOperand(const char* msg, int len);

void server_thread(void* param) {
	char msg[MSG_LEN_MAX];
	int i = (int)param;
	int ret;
	uint8_t closeCon = 0;


	//char* msgBack;
#if USE_DISPLAY
	queueLCDMsg_t toSendLCD;
#endif


#if USE_AUDIO
	queueAUDIOMsg_t toSendAudio;

	toSendAudio.pitch = 2000;
	toSendAudio.duration = 100;
	xQueueSend(AUDIO_msgQueue, &toSendAudio, 0);
#endif

	printf("New client: %d\n", i);

#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("Client"), "connect");
#endif
	msg[MSG_LEN_MAX-1] = '\0';

	while(!closeCon) {

#if configUSE_TRACE_FACILITY
		vTracePrintF(xTraceOpenLabel("Client"), "Waiting message");
#endif

		do {
			ret = simpleRecv(s[i], (unsigned char*)msg, MSG_LEN_MAX);

			if(ret == -1) {
				printf("error with client %d\n", i);
				closeCon = 1;
				break;
			}
			else if(ret == 0) {
				closeCon = 1;
				break;
			}
			else {


#if configUSE_TRACE_FACILITY
			vTracePrintF(xTraceOpenLabel("Client"), "Sending message");
#endif

#if USE_DISPLAY
			switch(messageOperand(msg, ret)) {
			case 1:	// Hello
				toSendLCD.type = 10;
				toSendLCD.tick = xTaskGetTickCount();
				if(uxQueueSpacesAvailable(LCD_msgQueue) != 0) {
					toSendLCD.ptr = 0;
					xQueueSend(LCD_msgQueue, &toSendLCD, 0);
				}
				simpleSendStr(s[i], "Hello!\n");
				break;

			case 2:	// Chat
				toSendLCD.type = 11;
				toSendLCD.tick = xTaskGetTickCount();
				if(uxQueueSpacesAvailable(LCD_msgQueue) != 0) {
					toSendLCD.ptr = pvPortMalloc(ret-5+1);
					memcpy(toSendLCD.ptr, msg+5, ret-5);
					toSendLCD.ptr[ret-5] = '\0';
					if(xQueueSend(LCD_msgQueue, &toSendLCD, 0) != pdTRUE)
						vPortFree(toSendLCD.ptr);
				}
				break;
			case 3: // Log
				toSendLCD.type = 12;
				toSendLCD.tick = xTaskGetTickCount();
				if(uxQueueSpacesAvailable(LCD_msgQueue) != 0) {
					toSendLCD.ptr = pvPortMalloc(ret-4+1);
					//printf("%d\n", (int)toSendLCD.ptr);
					memcpy(toSendLCD.ptr, msg+4, ret-4);
					toSendLCD.ptr[ret-3] = '\0';
					if(xQueueSend(LCD_msgQueue, &toSendLCD, 0) != pdTRUE)
						vPortFree(toSendLCD.ptr);
				}
				break;
			case 0:
			default:
					toSendLCD.type = 1;
					toSendLCD.tick = xTaskGetTickCount();
					if(uxQueueSpacesAvailable(LCD_msgQueue) != 0) {
						toSendLCD.ptr = pvPortMalloc(ret+1);
						memcpy(toSendLCD.ptr, msg, ret);
						toSendLCD.ptr[ret] = '\0';
						if(xQueueSend(LCD_msgQueue, &toSendLCD, 0) != pdTRUE)
							vPortFree(toSendLCD.ptr);
					}
			}
#else
					msg[ret] = '\0';
					printf("Received: %s", msg);
#endif

#if USE_AUDIO
					xQueueSend(AUDIO_msgQueue, &toSendAudio, 0);
#endif

			}

		} while(!detectEOF(msg,ret));

	}	// while(!closeCon)

#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("Client"), "disconnect");
#endif

	simpleClose(s[i]);

	printf("Connection %d closed.\n", i);
	printf("Free mem: %d\n", xPortGetFreeHeapSize());

#if USE_AUDIO
	toSendAudio.pitch = 500;
	toSendAudio.duration = 500;
	xQueueSend(AUDIO_msgQueue, &toSendAudio, portMAX_DELAY);
#endif
	s[i] = -1;

	vTaskDelete(NULL);
}	/* server_thread */


uint8_t detectEOF(const char* msg, int len) {
	return msg[len-4] == '\r' && msg[len-3] == '\n' && msg[len-2] == '\r' && msg[len-1] == '\n';
}

uint8_t messageOperand(const char* msg, int len) {
	char op[6];
	int i;
	if(len < 4)
		return 0;

	if(len >= 4) {
		for(i=0; i<4; ++i)
			op[i] = tolower((int)msg[i]);
		if(strncmp(op, "log ", 4) == 0)
			return 3;
	}
	if(len >= 5) {
		for(i=0; i<5; ++i)
			op[i] = tolower((int)msg[i]);
		if(strncmp(op, "chat ", 5) == 0)
			return 2;
	}
	if(len >= 6) {
		for(i=0; i<6; ++i)
			op[i] = tolower((int)msg[i]);
		if(strncmp(op, "hello ", 6) == 0)
			return 1;
	}

	return 0;

}
#endif /* DEMO_PRESENTATION */

