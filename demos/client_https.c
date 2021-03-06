/*
 * client_https.c
 *
 *  Created on: 1 juil. 2015
 *      Author: yannick.dayer
 */



#include "demos/demos.h"

#if DEMO_HTTPS_CLIENT

#define RECREATE_SOCKET 0	/* 0: create the socket once.
							   1: Recreate the socket for every message */
#define SLOW_SEND 0	/* 0: full speed.
					   1: wait 1s between messages */


/*============================================================================*/
/*-------------------------------- MAIN TASK ---------------------------------*/
/*============================================================================*/


void main_task(void* param) {
	int socket;
	int count=0;
	int ret;
	char RData[200];

#if USE_DISPLAY
	//queueLCDMsg_t toSendLCD;
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
			printf("Trying connection to IP.\n");
			// Connect the socket to the port TCP_PORT on PC_IP
			if( (ret = secureConnectDNS(socket, "www.google.ch", 443)) < 0) {
				printf("Error on connect (www.google.ch : 443).\n");
			}
			else {	// Connect successful
				printf("Connected.\n");

				//do {	// Loop to send messages repeatedly

#if configUSE_TRACE_FACILITY
			vTracePrintF(xTraceOpenLabel("Client msg"), "%d", count);
#endif	/* configUSE_TRACE_FACILITY */

					// Send the message to the host
					if( (ret = secureSendStr(socket,
									"GET /images/srpr/logo8w.png HTTP/1.1\r\n"
									"Host: www.google.ch\r\n")) < 0) {

						printf("Error on send.\n");
					}
					else {
						printf("Request sent.\n");
						do {
							ret = secureRecv(socket, (unsigned char*)RData, sizeof(RData));
							if(ret < 0) {
								printf("\nError on recv.\n");
							}
							else if(ret == 0) {
								printf("\nEnd connection received.\n");
							}
							else {
								printf("%.*s", ret, RData);
							}
						} while(ret > 0);
					}
#if SLOW_SEND
						// Wait some time
						vTaskDelay(1*configTICK_RATE_HZ);
#if !USE_DISPLAY
						// We can use printf with SLOW_SEND
						printf("Sending message: %s\n"(const char*)RData);
#endif	/* !USE_DISPLAY */

#endif	/* SLOW_SEND */

					// Change the content of the next message
					if(++count > 999999)
						count = 0;

				//} while(!RECREATE_SOCKET && ret > 0);
			}

			// Send the remaining packets
			//simple_shutdown(socket, 2);

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

#endif /* DEMO_HTTPS_CLIENT */
