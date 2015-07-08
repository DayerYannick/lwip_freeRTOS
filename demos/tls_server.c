/*
 * ssl_server.c
 *
 *  Created on: 22 juin 2015
 *      Author: yannick.dayer
 */


#include "demos/demos.h"

#if DEMO_TLS_SERVER

#include <stdlib.h>

#define MSG_LEN_MAX 2000	// The maximum length of a message when receiving

#define ClNbMax 3	// The maximum number of simultaneous connected clients
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

	socket = secureSocket();
	if(socket == -1)
		printf("ERROR while creating socket\n");

	if(secureBind(socket, getMyIP(), TCP_PORT) == -1)
		printf("ERROR in Bind\n");
	if(secureListen(socket) == -1)
		printf("ERROR in Listen\n");

	printf("accepting...\n");

#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("main"), "Accepting");
#endif

	for(i=0;i<ClNbMax;++i) {
		s[i] = -1;
	}
	while(1) {
		int tempS = secureAccept(socket);

		if(tempS < 0)
			printf("ERROR in Accept.\n");
		else {
			for(i=0;i<ClNbMax;++i)
				if(s[i] == -1)
					break;

			if(i==ClNbMax) {
				printf("ERROR: Cannot create socket: too many clients.");
				secureClose(tempS);
				printf("Client closed.\n");
			}
			else {
				s[i] = tempS;
				xTaskCreate(server_thread, "Client Handle", serverThreadStackSize, (void*)i, uxTaskPriorityGet(NULL), NULL);
				// TODO remove
				vTaskDelay(100000);
			}
		}
	}


}	/* main_task */



/*============================================================================*/
/*------------------------------ SERVER THREAD -------------------------------*/
/*============================================================================*/

uint8_t detectEOF(const char* msg, int len);

void server_thread(void* param) {
	char msg[MSG_LEN_MAX];
	char msgBack[MSG_LEN_MAX+4];
	int i = (int)param;
	int ret;
	//char traceMsg[5];
	int traceNb;
	uint8_t firstSegment=1;
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
	printf("Free mem: %d\n", xPortGetFreeHeapSize());

#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("Client"), "connect");
#endif
	msg[MSG_LEN_MAX-1] = '\0';

	while(!closeCon) {

		firstSegment = 1;
#if configUSE_TRACE_FACILITY
		vTracePrintF(xTraceOpenLabel("Client"), "Waiting message");
#endif

		do {

			ret = secureRecv(s[i], (unsigned char*)msg, MSG_LEN_MAX);

			//printf("Received %d char.\n", ret);

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
			if(firstSegment) {
				traceNb = atoi(msg+4);
				vTracePrintF(xTraceOpenLabel("Client msg"), "%d", traceNb);
			}
#endif


#if configUSE_TRACE_FACILITY
			vTracePrintF(xTraceOpenLabel("Client"), "Sending message");
#endif

			if(firstSegment) {
				//int err;
				msgBack[0] = 'O';
				msgBack[1] = 'K';
				msgBack[2] = '!';
				msgBack[3] = ' ';
				memcpy(msgBack+4, msg, ret);
				//printf("Sending %d char.\n", ret+4);
				/*err = */secureSend(s[i], (unsigned char*)msgBack, ret+4);
				//printf("returned %d.\n", err);
			}
			else {
				//int err;
				//printf("Sending %d char.\n", ret);
				/*err = */secureSend(s[i], (unsigned char*)msg, ret);
				//printf("returned %d.\n", err);
			}

			//printf("Free mem: %d\n", xPortGetFreeHeapSize());

				if(firstSegment) {
#if USE_DISPLAY
					toSendLCD.type = 1;
					toSendLCD.tick = xTaskGetTickCount();
					if(uxQueueSpacesAvailable(LCD_msgQueue) != 0) {
						toSendLCD.ptr = pvPortMalloc(ret+1);
						memcpy(toSendLCD.ptr, msg, ret);
						toSendLCD.ptr[ret] = '\0';
						if(xQueueSend(LCD_msgQueue, &toSendLCD, 0) != pdTRUE)
							vPortFree(toSendLCD.ptr);
					}
#endif
				}

#if USE_AUDIO
				if(firstSegment)
					xQueueSend(AUDIO_msgQueue, &toSendAudio, 0);
#endif

			}
		firstSegment = 0;

		} while(!detectEOF(msg,ret));

	}	// while(!closeCon)

#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("Client"), "disconnect");
#endif

	secureClose(s[i]);

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


#endif /* DEMO_SSL_SERVER */
