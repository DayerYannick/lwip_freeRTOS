/*
 * ssl_client.c
 *
 *	Tries to connect to the PC and checks the certificate validity
 *
 *  Created on: 22 juin 2015
 *      Author: yannick.dayer
 */


#include "demos/demos.h"

#if DEMO_TLS_CLIENT

#define RECREATE_SOCKET 0	/* 0: create the socket once. 1: Recreate the socket for every message */
#define SLOW_SEND 0	/* 0: full speed. 1: wait 1s between messages */
#define OUTPUT_PRINTF 1	/* 0: only errors output to console, 1: print informations */


/*============================================================================*/
/*-------------------------------- MAIN TASK ---------------------------------*/
/*============================================================================*/


void main_task(void* param) {
	int socket;
	int count=0;
	int ret;
	char data[13];

#if USE_DISPLAY
	queueLCDMsg_t toSendLCD;
#endif

/*-------------------- INIT --------------------*/

#if MY_IP_BY_DHCP
	// Init the network interface and wait for a DHCP address
	while(lwip_init_DHCP(0, MY_HOSTNAME) != 0);	// Repeat in case of error...
	lwip_wait_events(EV_LWIP_IP_ASSIGNED, portMAX_DELAY);
#if !USE_DISPLAY
	// Display our IP on the console if the LCD display is not used
	printf("My ip: %s\n", getMyIP());
#endif	/* !USE_DISPLAY */

#else	/* MY_IP_BY_DHCP */

	// Init the network interface with a given IP address
	lwip_init_static(MY_IP, MY_MASK, MY_GW, MY_HOSTNAME);

#endif	/* MY_IP_BY_DHCP */


/*---------------- START CLIENT ----------------*/

	while(1) {

		if(OUTPUT_PRINTF)
			printf("creating socket.\n");
#if configUSE_TRACE_FACILITY
			vTracePrintF(xTraceOpenLabel("Client"), "Creating socket");
#endif
		// Create a socket to handle the connection
		socket = secureSocket(TCP);
		if(socket < 0) {
			printf("Error on socket creation.\n");
		}
		else {
			if(OUTPUT_PRINTF)
				printf("Trying connection to IP.\n");
			// Connect the socket to the port TCP_PORT on PC_IP
			if( (ret = secureConnect(socket, PC_IP, TCP_PORT)) < 0) {
				printf("Error on connect.\n");
			}
			else {	// Connect successful


				do {	// Loop to send messages repeatedly

					// Create the message
					sprintf(data, " msg %6d\n", count);
#if USE_DISPLAY
					// Send a copy to the
					toSendLCD.type = 0;
					toSendLCD.tick = xTaskGetTickCount();
					toSendLCD.ptr = pvPortMalloc(sizeof(data));
					memcpy(toSendLCD.ptr, data, sizeof(data));
					if(xQueueSend(LCD_msgQueue, &toSendLCD, 0) != pdTRUE)
						vPortFree(toSendLCD.ptr);
#endif	/* USE_DISPLAY */

#if configUSE_TRACE_FACILITY
			vTracePrintF(xTraceOpenLabel("Client msg"), "%d", count);
#endif	/* configUSE_TRACE_FACILITY */

					// Send the message to the host
					if( (ret = secureSendStr(socket, data)) < 0) {
						printf("Error on send.\n");
					}
#if SLOW_SEND
						// Wait some time
						vTaskDelay(1*configTICK_RATE_HZ);

#endif	/* SLOW_SEND */

#if !USE_DISPLAY
						printf("Sending message: %s\n", (char*)data);
#endif	/* !USE_DISPLAY */

					// Change the content of the next message
					if(++count > 999999)
						count = 0;

				} while(!RECREATE_SOCKET && ret > 0);
			}

#if configUSE_TRACE_FACILITY
			vTracePrintF(xTraceOpenLabel("Client"), "Closing socket");
#endif
			// Close the socket to free the memory
			if( (ret=secureClose(socket)) < 0)
				printf("ERROR while closing socket: %d.\n", ret);

			printf("Socket Closed.\n");
		}

		printf("\n");

	}	// While(1)
}

#endif /* DEMO_SSL_CLIENT */
