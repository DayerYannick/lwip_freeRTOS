/*
 * client.c
 *
 *  Created on: 22 juin 2015
 *      Author: yannick.dayer
 */


#include "demos/demos.h"

#if DEMO_CLIENT

void client_task(void* param);



/*============================================================================*/
/*-------------------------------- MAIN TASK ---------------------------------*/
/*============================================================================*/


void main_task(void* param) {


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
	xTaskCreate(client_task,	// The code to execute
				"Client task",	// A name for the trace functionnality
				configMINIMAL_STACK_SIZE*8,	// A decent stack size
				NULL,			// no parameters
				uxTaskPriorityGet(NULL),	// The priority of the actual task
				NULL );			// No handler

	vTaskDelete(NULL);	// Delete the actual task
}


/*============================================================================*/
/*------------------------------- CLIENT TASK --------------------------------*/
/*============================================================================*/

void client_task(void* param) {
	int socket;
	int count=0;
	int ret;
	char data[9];

	vTaskDelay(1000);

	while(1) {

		printf("creating socket.\n");
		socket = simpleSocket();
		if(socket < 0) {
			printf("Error on socket creation.\n");
		}
		else {

			printf("Trying connection to IP.\n");
			if( (ret = simpleConnect(socket, PC_IP, TCP_PORT)) < 0) {
				printf("Error on connect.\n");
			}
			else {
				sprintf(data, "msg %3d\n", count);
				printf("Client sending to IP: %s", data);
				if( (ret = simpleSendStr(socket, data)) < 0) {
					printf("Error on send.\n");
				}
			}

			simple_shutdown(socket, 2);

			vTaskDelay(100);

			if( (ret=simpleClose(socket)) < 0)
				printf("ERROR while closing socket: %d.\n", ret);

			printf("Socket Closed.\n");
			printf("(free mem = %d)\n", xPortGetFreeHeapSize());

		}

		if(++count > 999)
			count = 0;
		vTaskDelay(200);

		printf("\n");

	}	// While(1)
}


#endif /* DEMO_CLIENT */
