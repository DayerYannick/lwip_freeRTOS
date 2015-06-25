/*
 * client.c
 *
 *  Created on: 22 juin 2015
 *      Author: yannick.dayer
 */


#include "demos/demos.h"

#if DEMO_CLIENT

#define RECREATE_SOCKET 0	/* 0: create the socket once. 1: Recreate the socket for every message */
#define SLOW_SEND 0	/* 0: full speed. 1: wait 1s between messages */


/*============================================================================*/
/*-------------------------------- MAIN TASK ---------------------------------*/
/*============================================================================*/


void main_task(void* param) {
	int socket;
	int count=0;
	int ret;
	char data[9];

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


/*---------------- START CLIENT ----------------*/

	while(1) {

		printf("creating socket.\n");
#if configUSE_TRACE_FACILITY
			vTracePrintF(xTraceOpenLabel("Client"), "Creating socket");
#endif
		socket = simpleSocket();
		if(socket < 0) {
			printf("Error on socket creation.\n");
		}
		else {

			printf("Trying connection to IP.\n");
			if( (ret = simpleConnect(socket, PC_IP, TCP_PORT)) < 0) {
				printf("Error on connect.\n");
			}
			else {	// No error on connect
				do {
					sprintf(data, "msg %3d\n", count);

#if configUSE_TRACE_FACILITY
			vTracePrintF(xTraceOpenLabel("Client msg"), "%d", count);
#endif
					if(++count > 999)
						count = 0;
					//printf("Client sending to IP: %s", data);
					if( (ret = simpleSendStr(socket, data)) < 0) {
						printf("Error on send.\n");
					}
					if(SLOW_SEND) {
						vTaskDelay(1*configTICK_RATE_HZ);
					}
				} while(!RECREATE_SOCKET && ret > 0);
			}

			simple_shutdown(socket, 2);

#if configUSE_TRACE_FACILITY
			vTracePrintF(xTraceOpenLabel("Client"), "Closing socket");
#endif
			if( (ret=simpleClose(socket)) < 0)
				printf("ERROR while closing socket: %d.\n", ret);

			printf("Socket Closed.\n");
			printf("(free mem = %d)\n", xPortGetFreeHeapSize());

		}

		printf("\n");

	}	// While(1)
}



#endif /* DEMO_CLIENT */
