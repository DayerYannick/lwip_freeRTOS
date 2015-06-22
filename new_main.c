/*
 * new_main.c
 *
 *  Created on: 22 juin 2015
 *      Author: yannick.dayer
 */

#include "demos/demos.h"

#include "freertos/FreeRTOS.h"

void breath_task(void* param);


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
				configMINIMAL_STACK_SIZE*8,
				NULL,
				3,
				NULL );

	xTaskCreate(breath_task,
				"Breath LED task",
				configMINIMAL_STACK_SIZE,
				NULL,
				2,
				NULL );

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
 * Changes the pwm of the LED 0 every 'ratems' ms, to inform the user that the
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
#if DISPLAY_MSG_ON_LCD
	GHandle ghLastMessage;
	GHandle ghLastMessageTime;
#endif
	uint32_t t;

#if DISPLAY_MSG_ON_LCD
	queueLCDMsg_t received;
#endif

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

#if DISPLAY_MSG_ON_LCD
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
#endif




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
#if DISPLAY_MSG_ON_LCD
		xQueueReceive(LCD_msgQueue, &received, portMAX_DELAY);

#if configUSE_TRACE_FACILITY
		vTracePrintF(xTraceOpenLabel("LCD"), "message received");
#endif
		switch(received.type) {
		case 2:
			gwinPutString(ghConsole, received.ptr);
			break;
		default:
			gwinSetText(ghLastMessage, received.ptr, 0);
			sprintf(msg, "%d.%03d:",  (int)(received.tick/1000), (int)(received.tick%1000));
			gwinSetText(ghLastMessageTime, msg, 0);
		}
		vPortFree(received.ptr);
#else
		vTaskDelay(10*configTICK_RATE_HZ);
#endif

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
