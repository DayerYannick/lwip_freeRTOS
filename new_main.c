/*
 * new_main.c
 *
 *  Created on: 22 juin 2015
 *      Author: yannick.dayer
 */

#include "demos/demos.h"

#include "freertos/FreeRTOS.h"

#if DEMO_TLS_CLIENT || DEMO_TLS_SERVER
#define MAIN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE*8
#else
#define MAIN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE*8
#endif

void breath_task(void* param);
void watcher_task(void* param);


/*============================================================================*/
/*----------------------------------- MAIN -----------------------------------*/
/*============================================================================*/

int main(int argc, char** argv) {
	(void) argc;
	(void) argv;

	printf("\n\n");

#if USE_DISPLAY
 	LCD_msgQueue = xQueueCreateNamed(1,
 										sizeof(queueLCDMsg_t),
 										"LCD_msg queue" );
	xTaskCreate(lcd_task,
				"Display task",
				configMINIMAL_STACK_SIZE*8,
				NULL,
				3,
				NULL );
#endif

#if USE_AUDIO
 	AUDIO_msgQueue = xQueueCreateNamed(1,
 										sizeof(queueAUDIOMsg_t),
 										"AUDIO_msg queue" );
	xTaskCreate(audio_task,
				"Audio task",
				configMINIMAL_STACK_SIZE*8,
				NULL,
				3,
				NULL );
#endif

	xTaskCreate(main_task,
				"Main task",
				MAIN_TASK_STACK_SIZE,
				NULL,
				3,
				NULL );

/*	xTaskCreate(breath_task,
				"Breath LED task",
				configMINIMAL_STACK_SIZE,
				NULL,
				2,
				NULL );

	xTaskCreate(watcher_task,
				"event watcher task",
				configMINIMAL_STACK_SIZE*4,
				NULL,
				uxTaskPriorityGet(NULL),
				NULL);

*/
	vTaskStartScheduler();

	printf("ERROR: Task scheduler returned ??\n");
	return 0;
}



/*============================================================================*/
/*---------------------------------- Tasks -----------------------------------*/
/*============================================================================*/

/**
 * @brief Changes the luminosity of a LED periodically
 *
 * Changes the pwm of the LED0 every 'ratems' ms, to inform the user that the
 * system is running.
 */
void breath_task(void* param) {
	int8_t pwm = 0;		// Actual luminosity. 0: LED off / 100: lum. max
	uint8_t pwmInc = 1;	// 1: increase lum. / 0: decrease lum.
	const uint8_t ratems = 100;	// Time in ms between two changes of lum.

	while (1) {

		bsp_led_set_pwm(0, pwm);
		if(pwmInc != 0) {	// pwmInc != 0: increase the pwm
			if ((pwm+=ratems/10+1) >= 100) {
				pwmInc = 0;
				pwm = 100;
			}
		}
		else {	// pwmInc == 0: decrease the pwm
			if((pwm-=ratems/10+1) <= 10) {
				pwmInc = 1;
				pwm = 10;
			}
		}
		vTaskDelay((float)ratems/1000 * configTICK_RATE_HZ); // wait 'ratems' ms

	}	// while(1)

}	/* breath_task */

#if USE_DISPLAY

void lcd_task(void* param) {
	GWidgetInit wi;
	GHandle ghStatus;
	GHandle ghConsole;
	GHandle ghLastMessage;
	GHandle ghLastMessageTime;
	uint32_t t;

	queueLCDMsg_t received;

	char msg[32];

	gfxInit();
	// Set the widget defaults
		gwinSetDefaultFont(gdispOpenFont("DejaVuSans10"));
		gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
		gdispClear(White);

		// Attach the mouse input
		gwinAttachMouse(0);

		// Apply some default values for GWIN
		wi.customDraw = 0;
		wi.customParam = 0;
		wi.customStyle = 0;
		wi.g.show = TRUE;


		//-- TITLE --//
#if DEMO_SERVER
		wi.text = "TCP/IP STACK: Server test";
#elif DEMO_CLIENT
		wi.text = "TCP/IP STACK: Client test";
#elif DEMO_HTTP_CLIENT
		wi.text = "TCP/IP STACK: HTTP client test";
#elif DEMO_HTTP_SERVER
		wi.text = "TCP/IP STACK: HTTP server test";
#elif LED_COMMAND_TEST
		wi.text = "TCP/IP STACK: LED command test";
#elif DEMO_SSL_CLIENT
		wi.text = "TCP/IP STACK: SSL client test";
#elif DEMO_SSL_SERVER
		wi.text = "TCP/IP STACK: SSL server test";
#elif DEMO_HTTPS_SERVER
		wi.text = "TCP/IP STACK: HTTPS server test";
#else
		wi.text = "-- no title --";
#endif

		wi.g.y = 10;
		wi.g.x = 50;
		wi.g.width = 200;
		wi.g.height = 20;

		gwinLabelCreate(NULL, &wi);

		//-- Last message label --//

		wi.g.y = 30;
		wi.g.x = 10;
		wi.g.width = 300;
		wi.g.height = 20;
		wi.text = "0.000:";

		ghLastMessageTime = gwinLabelCreate(NULL, &wi);


		wi.g.y = 45;
		wi.g.x = 10;
		wi.g.width = 300;
		wi.g.height = 20;
		wi.text = "No message received.";

		ghLastMessage = gwinLabelCreate(NULL, &wi);




		//-- Console --//

		wi.g.y = 70;
		wi.g.x = 10;
		wi.g.width = 250;
		wi.g.height = 150;
		wi.text = NULL;

		ghConsole = gwinConsoleCreate(NULL, &wi.g);
		gwinSetColor(ghConsole,  RGB2COLOR(255,255,100));
		gwinSetBgColor(ghConsole, Black);
		gwinClear(ghConsole);



		//-- status label --//

		wi.g.y = 225;
		wi.g.x = 10;
		wi.g.width = 150;
		wi.g.height = 20;
		wi.text = "lwip not initialized.";

		ghStatus = gwinLabelCreate(NULL, &wi);




		t = (uint32_t)xTaskGetTickCount()/portTICK_PERIOD_MS;
		sprintf(msg, "%d.%03d: gfx initialized.\n", (int)(t/1000), (int)(t%1000));
		gwinPutString(ghConsole, msg);

		lwip_wait_events(EV_LWIP_INITIALIZED, portMAX_DELAY);
		gwinSetText(ghStatus, "lwip initialized.", 1);

		t = (uint32_t)xTaskGetTickCount()/portTICK_PERIOD_MS;
		sprintf(msg, "%d.%03d: lwip initialized.\n",  (int)(t/1000), (int)(t%1000));
		gwinPutString(ghConsole, msg);

		lwip_wait_events(EV_LWIP_IP_ASSIGNED, portMAX_DELAY);

		t = (uint32_t)xTaskGetTickCount()/portTICK_PERIOD_MS;
		sprintf(msg, "%d.%03d: IP assigned: %s.\n",  (int)(t/1000), (int)(t%1000), getMyIP());
		gwinPutString(ghConsole, msg);


	while(1) {
		xQueueReceive(LCD_msgQueue, &received, portMAX_DELAY);

#if configUSE_TRACE_FACILITY
		vTracePrintF(xTraceOpenLabel("LCD"), "message received");
#endif
		switch(received.type) {
		case 2:
			gwinPutString(ghConsole, received.ptr);
			break;
		case 5:	// picture

			break;
		default:
			gwinSetText(ghLastMessage, received.ptr, 0);
			sprintf(msg, "%d.%03d:",  (int)(received.tick/1000), (int)(received.tick%1000));
			gwinSetText(ghLastMessageTime, msg, 0);
		}
		vPortFree(received.ptr);

	}	// while(1)

}	/* lcd_task */

#endif	/* USE_DISPLAY */


#if USE_AUDIO

void audio_task(void* param) {

	queueAUDIOMsg_t audioMsg;

 	Audio_Init(16000, 16);
 	Audio_SetVolumeSpeaker(150);

 	Audio_PlaySin(880, 100);
 	Audio_PlaySin(1320, 100);
 	Audio_PlaySin(1760, 200);

	while(1) {
		xQueueReceive(AUDIO_msgQueue, &audioMsg, portMAX_DELAY);

		Audio_PlaySin(audioMsg.pitch, audioMsg.duration);
		//vTaskDelay(10*configTICK_RATE_HZ);
	}	// while(1)

}	/* audio_task */

#endif /* USE_AUDIO */

void watcher_task(void* param) {
	uint32_t bits[10] = {0};
	uint32_t temp;
	uint8_t socket;
	uint32_t i;
#if USE_DISPLAY
	queueLCDMsg_t toSend;
#endif

	while(1) {
		for(socket=0; socket<getSocketNbMax() && socket<10; ++socket) {	// process all the sockets
			if(socketValid(socket) == 0) {
				bits[socket] = 0;
				continue;
			}
			if( (temp = socket_get_events(socket, EV_LWIP_SOCKET_RECV_TIMEOUT | EV_LWIP_SOCKET_SEND_TIMEOUT | EV_LWIP_SOCKET_ACCEPT_TIMEOUT)) > 0) {
				if(temp != bits[socket]) {	// Changes detected

					for(i=1; i<=(1<<6); i<<=1) {	// Process all bits
						if(!(i&bits[socket]) && (i&temp)) {	// Event raised


#if USE_DISPLAY
							toSend.tick = xTaskGetTickCount();
							toSend.type = 2;
							toSend.ptr = pvPortMalloc(50);

							switch(i&temp) {
							case 1<<4:
								sprintf(toSend.ptr, "socket %d: EV_LWIP_SOCKET_RECV_TIMEOUT\n", socket);
								break;
							case 1<<5:
								sprintf(toSend.ptr, "socket %d: EV_LWIP_SOCKET_SEND_TIMEOUT\n", socket);
								break;
							case 1<<6:
								sprintf(toSend.ptr, "socket %d: EV_LWIP_SOCKET_ACCEPT_TIMEOUT\n", socket);
								break;
							default:
								;
							}	// switch(i&temp)
#else
							switch(i&temp) {
							case 1<<4:
								printf("socket %d: EV_LWIP_SOCKET_RECV_TIMEOUT\n", socket);
								break;
							case 1<<5:
								printf("socket %d: EV_LWIP_SOCKET_SEND_TIMEOUT\n", socket);
								break;
							case 1<<6:
								printf("socket %d: EV_LWIP_SOCKET_ACCEPT_TIMEOUT\n", socket);
								break;
							default:
								;
							}	// switch(i&temp)
#endif
						}	// if(!(i&bits...
					}	// for(i=1;...

					bits[socket] = temp;

				}	// if(temp != bits[...
			}	// if( (temp =...
		}	// for(socket=0...

		vTaskDelay(500);
	}	// while(1) {
}
