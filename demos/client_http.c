/*
 * client_http.c
 *
 *	Connects to http://www.hevs.ch and download the HES-SO logo
 *
 *  Created on: 22 juin 2015
 *      Author: yannick.dayer
 */


#include "demos/demos.h"

#if DEMO_HTTP_CLIENT

#define IMG_SIZE 3000
#define BUF_SIZE 300

/*============================================================================*/
/*-------------------------------- MAIN TASK ---------------------------------*/
/*============================================================================*/


void main_task(void* param) {
	int socket;
	int ret;
	char data[BUF_SIZE];
	char img[IMG_SIZE];
	char* imgPtr = img;
	char* i = img;
	uint8_t eofFound;

	imgStruct_t picture;

	picture.img = img;

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
	printf("--Socket creation.\n");
	socket = simpleSocket(TCP);

	vTaskDelay(1000);

	printf("--Connection.\n");
	if(socket != -1) {
		// Connecte le socket au port 80 (HTTP) de www.hevs.ch
		if(simpleConnectDNS(socket, "www.hevs.ch", 80) == 0) {
			printf("--Connected.\n");


			printf("--Send.\n");
			// Send a simple HTTP request for the HES-SO logo
			if(simpleSendStr(socket, "POST /img/logo-hes-so-valais.png HTTP/1.1\r\n"
										"Host: www.hevs.ch\r\n\r\n") != -1) {

				printf("--Message sent.\n");

				imgPtr = img;
				i=img;
				eofFound = 0;
				do {
					// Wait for the answer
					ret = simpleRecv(socket, (unsigned char*)data, sizeof(data));


					if(ret != -1 && ret != 0) {	// Message partly received
						printf("Received %d char: %.*s\n", ret, ret, data);

				/* TODO Image parsing
						if(imgPtr+ret > img+IMG_SIZE) {
							printf("\n\nError, too much data\n\n");
							break;
						}

						// Copy the received data to the img array
						memcpy(imgPtr, data, ret);
						imgPtr += ret;	// Move the pointer to the end of the received data

						// Search for the "\r\n\r\n" string if not already found
						while(!eofFound && i < imgPtr) {
							if(memcmp(i++, "\r\n\r\n", 4) == 0) {
								eofFound = 1;
								memcpy(img, i, imgPtr-i);	// Copy the start of the picture
								imgPtr = img;	// Put the pointer at start for actual picture data to go to the array
							}
						}
						if(!eofFound) {
							if(ret > 4)
								i-=4;
							else
								i-=ret;
						}
				*/
					}
					else if(ret == 0)
						printf("--End of connection requested from the host.\n");
					else
						printf("ERROR while receiving.\n");
				} while(ret != 0 && ret != -1);

				// Connection ended successfully

/* TODO send image to screen app
				picture.size = imgPtr-img;
				toSendLCD.type = 5;
				toSendLCD.ptr = picture; // NOT finished
				toSendLCD.tick = xTaskGetTickCount();

				xQueueSend(LCD_msgQueue, &picture, portMAX_DELAY);
*/

			}	// if(simpleSendStr(...
			else
				printf("ERROR while sending.\n");
		}
		else
			printf("ERROR while connecting.\n");

		printf("--Close.\n");
		simpleClose(socket);

		printf("\n\n");
	}
	else
		printf("ERROR while creating socket: %s.\n", strerror(errno));

	// Wait 2s and restart
	vTaskDelay(2000);
	}
}


#endif /* DEMO_HTTP_CLIENT */
