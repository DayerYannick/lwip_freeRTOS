/**
 * @file main.c
 *
 * @brief test programs for lwip TCP stack
 * @author Dayer Yannick
 */

#include "heivs/config.h"

#include <stdint.h>
#include "heivs/bsp.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"


#include "lwip/lwip.h"



#include "polarssl/md5.h"


//-- Defines --//

#define MY_IP_BY_DHCP 1	/* 0: use MY_IP / 1: use DHCP */


#if !MY_IP_BY_DHCP
#define MY_IP "192.168.1.5"
#define MY_MASK "255.255.255.0"
#define MY_GW "192.168.1.1"
#define PC_IP "192.168.1.2"
#else
#define PC_IP "153.109.5.181"
#endif

#define TCP_PORT 4433



#define SERVER_TEST 0	/* Accept incoming connections */
#define CLIENT_TEST 0	/* Connect to PC_IP and send messages */
#define HTTP_TEST 0		/* Send a request to http://www.hevs.ch */
#define HTTP_SERVER_TEST 0	/* A (very) simple HTTP server... */

#define LED_COMMAND_TEST 0	/* Remote control of the LEDs on the board */

#define SECURE_CLIENT_TEST 1	/* Connection to a secured server via TLS */

#define USE_DISPLAY 0
#if USE_DISPLAY
#include "ugfx/gfx.h"
#endif

#if SERVER_TEST && USE_DISPLAY
#define DISPLAY_MSG_ON_LCD 1	/* 1: Displays the last received message on LCD screen (slow!) */
#endif

#define USE_AUDIO 0
#if USE_AUDIO
#include "heivs/audio.h"
#endif


#define MSG_LEN_MAX 1000	/* The maximum length (in bytes) of a message received via TCP */


void main_task(void* param);
void led_task(void* param);
#if USE_DISPLAY
void lcd_task(void* param);
#endif
#if USE_AUDIO
void audio_task(void* param);
#endif

#if HTTP_SERVER_TEST || SERVER_TEST || LED_COMMAND_TEST
void clientHandle_task(void* param);
#endif

#if CLIENT_TEST
void clientToIP_task(void* param);
#endif

#if SECURE_CLIENT_TEST
void clientToIPSecured_task(void* param);
#endif


#define ClNbMax 10

volatile int s[ClNbMax];


#if DISPLAY_MSG_ON_LCD
QueueHandle_t LCD_msgQueue;
typedef struct {
	char* ptr;
	unsigned int tick;
} queueLCDMsg_t;
#endif

#if USE_AUDIO
QueueHandle_t AUDIO_msgQueue;
typedef struct {
	unsigned int pitch;
	unsigned int duration;
} queueAUDIOMsg_t;
#endif

int main(int argc, char** argv) {
	int i;
	unsigned char digest[16];
	char hello[] = "Hello, world!";
 	printf("\n");

#if DISPLAY_MSG_ON_LCD
 	LCD_msgQueue = xQueueCreateNamed(1, sizeof(queueLCDMsg_t), "LCD_msg queue");
#endif

#if USE_AUDIO
 	AUDIO_msgQueue = xQueueCreateNamed(1, sizeof(queueAUDIOMsg_t), "AUDIO_msg queue");
#endif

	// DO NOT CALL ANY LWIP FUNCTIONS HERE


 	printf("MD5('%s') = ", hello);

 	md5( (unsigned char*)hello, 13, digest);

 	for(i=0; i<16; ++i) {
 		printf("%02x", digest[i]);
 	}

 	printf("\n\n");


	//xTaskCreate(led_task, "Led Task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);	// Indicates that the system is running
	xTaskCreate(main_task, "Main Task", configMINIMAL_STACK_SIZE*8, NULL, 3, NULL);	// Runs the tests
#if USE_DISPLAY
	xTaskCreate(lcd_task, "LCD Task", configMINIMAL_STACK_SIZE, NULL, 3, NULL);	// Displays informations on the screen
#endif
#if USE_AUDIO
	xTaskCreate(audio_task, "Audio Task", configMINIMAL_STACK_SIZE, NULL, 2, NULL);	// Plays sounds when something happens
#endif
	vTaskStartScheduler();	// Start FreeRTOS scheduler

	return 0;
}


/*
 * Socket Task
 */
void main_task(void* param) {

	printf("Starting main task.\n");

#if HTTP_TEST || SERVER_TEST || LED_COMMAND_TEST  || HTTP_SERVER_TEST
	int socket;
#endif
#if SERVER_TEST || LED_COMMAND_TEST  || HTTP_SERVER_TEST
	int i;

#endif

#if HTTP_TEST
	int ret;
	char data[MSG_LEN_MAX];
#endif

	vTracePrintF(xTraceOpenLabel("main"), "Init start");

	//-- INIT --//
#if MY_IP_BY_DHCP
	while(lwip_init_DHCP(0) != 0);
	lwip_wait_events(EV_LWIP_IP_ASSIGNED, portMAX_DELAY);
#if !USE_DISPLAY
	printf("My ip: %s\n", getMyIP());
#endif
#else
	lwip_init_static(MY_IP, MY_MASK, MY_GW);
#endif

	vTracePrintF(xTraceOpenLabel("main"), "Init end");

#if CLIENT_TEST
	xTaskCreate(clientToIP_task, "client task", configMINIMAL_STACK_SIZE*16, (void*)NULL, uxTaskPriorityGet(NULL), NULL);
#endif

#if SECURE_CLIENT_TEST
	xTaskCreate(clientToIPSecured_task, "SecuClient task", configMINIMAL_STACK_SIZE*34, (void*)NULL, uxTaskPriorityGet(NULL), NULL);
#endif


#if SERVER_TEST || LED_COMMAND_TEST  || HTTP_SERVER_TEST

	int clientHandleTaskStackSize = ((MSG_LEN_MAX/configMINIMAL_STACK_SIZE)+2) * configMINIMAL_STACK_SIZE;
	socket = simpleSocket();
	if(socket == -1)
		printf("ERROR while creating socket\n");

	//printf("socket = %d.\n", socket);
	if(simpleBind(socket, getMyIP(), TCP_PORT) == -1)
		printf("ERROR in Bind\n");
	if(simpleListen(socket) == -1)
		printf("ERROR in Listen\n");

	printf("accepting...\n");
	vTracePrintF(xTraceOpenLabel("main"), "Accepting");

	for(i=0;i<ClNbMax;++i) {
		s[i] = -1;
	}
	while(1) {
		int tempS = simpleAccept(socket);

		if(tempS == -1)
			printf("ERROR in Accept.\n");
		else {
			for(i=0;i<ClNbMax;++i)
				if(s[i] == -1)
					break;

			if(i==ClNbMax) {
				printf("ERROR: Cannot create socket: too many clients.");
				simpleClose(tempS);
			}
			else {
				s[i] = tempS;
				xTaskCreate(clientHandle_task, "Client Handle", clientHandleTaskStackSize, (void*)i, uxTaskPriorityGet(NULL), NULL);
			}
		}
	}

#endif



#if HTTP_TEST

	while(1) {
		printf("--Socket creation.\n");
		socket = simpleSocket();

		vTaskDelay(1000);

		printf("--Connection.\n");
		if(socket != -1) {
			if(simpleConnectDNS(socket, "www.hevs.ch", 80) == 0) {
				printf("--Connected.\n");

				vTaskDelay(1000);

				printf("--Send.\n");
				if(simpleSendStr(socket, "POST /img/logo-hes-so-valais.png HTTP/1.1\r\n"
						"Host: www.hevs.ch\r\n"
						"Accept: */*\r\n"
						"Content-Type: text/html\r\n"
						"Content-Length: 0\r\n\r\n") != -1) {
					printf("--Message sent.\n");

					vTaskDelay(2000);

					do {
						ret = simpleRecv(socket, data, MSG_LEN_MAX);
						if(ret != -1 && ret != 0)
							printf("Received %d char: %.*s\n", ret, ret, data);
						else if(ret == 0)
							printf("--End of connection requested.\n");
						else
							printf("ERROR while receiving.\n");
					} while(ret != 0 && ret != -1);

				}
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

#endif


	vTaskDelete(NULL);

}

#if HTTP_TEST
#define msgLength 200
void clientHandle_task(void* param) {
	char msg[msgLength];
	int i = (int)param;
	int ret;
	//char* msgBack;
	printf("New client: %d\n", i);

	while(1) {
		ret = simpleRecv(s[i], msg, msgLength);
		//printf("msg received... ret=%d\n", ret);

		if(ret == -1) {
			printf("error with client %d\n", i);
			break;
		}
		else if(ret == 0)
			break;
		else {
			//printf("%.*s", ret, msg);

			//printf("send Ok\n");

			//lwip_send(s[i], "<echo>\n", 8, 0);
			//msgBack = pvPortMalloc(ret);
			//sprintf(msgBack, "%s", msg);
			printf("Message received on server: %s", msg);
			//lwip_send(s[i], msgBack, ret, 0);
			//lwip_send(s[i], "</echo>\n", 9, 0);
			//vPortFree(msg);
			//vPortFree(msgBack);
		}
	}
	printf("Connection %d closed.\n", i);
	simpleClose(s[i]);
	s[i] = -1;

	vTaskDelete(NULL);
}
#endif

#if HTTP_SERVER_TEST || SERVER_TEST || LED_COMMAND_TEST
void clientHandle_task(void* param) {
	char msg[MSG_LEN_MAX];
	int i = (int)param;
	int ret;


	//char* msgBack;
#if DISPLAY_MSG_ON_LCD
	queueLCDMsg_t toSendLCD;
#endif


#if USE_AUDIO
	queueAUDIOMsg_t toSendAudio;

	toSendAudio.pitch = 2000;
	toSendAudio.duration = 100;
	xQueueSend(AUDIO_msgQueue, &toSendAudio, 0);
#endif

	printf("New client: %d\n", i);
	vTracePrintF(xTraceOpenLabel("client"), "connect");
	msg[MSG_LEN_MAX-1] = '\0';

	while(1) {
		ret = simpleRecv(s[i], (unsigned char*)msg, MSG_LEN_MAX-1);

		if(ret == -1) {
			printf("error with client %d\n", i);
			break;
		}
		else if(ret == 0)
			break;
		else {
			//printf("Message received on server: %.*s", ret, msg);

			//printf("send Ok\n");

			//lwip_send(s[i], "<echo>\n", 8, 0);
			//sprintf(msgBack, "%s", msg);
			//simpleSendStr(msgBack);
#if DISPLAY_MSG_ON_LCD
			toSendLCD.tick = xTaskGetTickCount();
			if(uxQueueSpacesAvailable(LCD_msgQueue) != 0) {
				toSendLCD.ptr = pvPortMalloc(ret+1);
				memcpy(toSendLCD.ptr, msg, ret);
				toSendLCD.ptr[ret] = '\0';
				if(xQueueSend(LCD_msgQueue, &toSendLCD, 0) != pdTRUE)
					vPortFree(toSendLCD.ptr);
			}
#endif

#if USE_AUDIO
			xQueueSend(AUDIO_msgQueue, &toSendAudio, 0);
#endif

			//vTracePrintF(xTraceOpenLabel("client"), "msg rcv");
#if HTTP_SERVER_TEST
			simpleSendStr(s[i], "HTTP/1.1 200 OK \n\r"
								"Date: Wen, 06 May 2015 10:40:00 GMT\n\r"
								"Server: Apache/0.8.4\n\r"
								"Content-Length: 140\n\r"
								"Content-Type: text/html\n\r"
								"\n\r"
								"<html><head><title>ARMEBS 4</title></head><body><h1>This is a test</h1>This page was generated on the ARMEBS4 and sent via tcp</body></html>\n\r");
#elif LED_COMMAND_TEST
			if(msg[0] == '1')
				bsp_led_toggle(1);
			else if(msg[0] == '2')
				bsp_led_toggle(2);
			else if(msg[0] == '3')
				bsp_led_toggle(3);
			//else
				//simpleSendStr(s[i], "not a LED.\n");
#endif
			//vPortFree(msg);
			//vPortFree(msgBack);
		}
	}
	vTracePrintF(xTraceOpenLabel("client"), "disconnect");
	printf("Connection %d closed.\n", i);

	simpleClose(s[i]);


#if USE_AUDIO
	toSendAudio.pitch = 500;
	toSendAudio.duration = 500;
	xQueueSend(AUDIO_msgQueue, &toSendAudio, portMAX_DELAY);
#endif
	s[i] = -1;

	vTaskDelete(NULL);
}
#endif

#if CLIENT_TEST
void clientToIP_task(void* param) {
	int socket;
	int count=0;
	int ret;
	char data[9];

	vTaskDelay(1000);

	while(1) {

		printf("creating socket.\n");
		socket = simpleSocket();
		if(socket < 0)
			printf("Error on socket creation.\n");
		else {
			printf("Trying connection to IP.\n");
			ret = simpleConnect(socket, PC_IP, TCP_PORT);
			if(ret < 0) {
				printf("Error on connect.\n");
			}
			else {
				sprintf(data, "msg %3d\n", count);
				printf("Client sending to IP: %s\n", data);
				ret = simpleSendStr(socket, data);
				if(ret < 0) {
					printf("Error on send.\n");
				}
			}

			simpleClose(socket);
		}

		if(++count >= 1000)
			count = 0;
		vTaskDelay(200);
	}	// While(1)
}

#endif


#if SECURE_CLIENT_TEST
void clientToIPSecured_task(void* param) {
	int socket;
	int count=0;
	int ret;
	char data[9];

	vTaskDelay(1000);

	while(1) {

		printf("creating socket.\n");
		vTracePrintF(xTraceOpenLabel("SecuClient"), "Create socket");
		socket = securedSocket();
		if(socket < 0)
			printf("Error on socket creation.\n");
		else {
			printf("Trying connection to IP with socket %d.\n", socket);
			vTracePrintF(xTraceOpenLabel("SecuClient"), "Connect socket %d", socket);
			ret = securedConnect(socket, PC_IP, TCP_PORT);
			if(ret < 0) {
				printf("Error on connect.\n");
			}
			else {
				sprintf(data, "msg %3d\n", count);
				printf("Client sending to IP: %s\n", data);
				vTracePrintF(xTraceOpenLabel("SecuClient"), "send");
				ret = securedSendStr(socket, (unsigned char*)data);
				if(ret < 0) {
					printf("Error on send.\n");
				}
			}

			vTracePrintF(xTraceOpenLabel("SecuClient"), "Close socket");
			securedClose(socket);
		}

		if(++count >= 1000)
			count = 0;
		vTaskDelay(1000);
		breakpoint();
	}	// While(1)
}

#endif

void led_task(void* param) {
	uint8_t led = 0;
	int8_t pwm = 0;
	uint8_t pwmInc = 1;
	while (1) {
		bsp_led_set_pwm(led, pwm);
		if(pwmInc != 0) {
			if (++pwm >= 100)
				pwmInc = 0;
		}
		else {
			if(--pwm <= 10)
				pwmInc = 1;
		}
		vTaskDelay(0.011 * configTICK_RATE_HZ);
	}
}

#if USE_DISPLAY

void lcd_task(void* param) {

	GWidgetInit wi;
	//GHandle ghButton;
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

/*		// Apply the button parameters
		wi.g.width = 100;
		wi.g.height = 30;
		wi.g.y = 10;
		wi.g.x = 10;
		wi.text = "Push Button";

		// Create the actual button
		ghButton = gwinButtonCreate(0, &wi);
*/

		//-- TITLE --//
#if SERVER_TEST
		wi.text = "TCP/IP STACK: Server test";
#elif CLIENT_TEST
		wi.text = "TCP/IP STACK: Client test";
#elif HTTP_TEST
		wi.text = "TCP/IP STACK: HTTP client test";
#elif HTTP_SERVER_TEST
		wi.text = "TCP/IP STACK: HTTP server test";
#elif LED_COMMAND_TEST
		wi.text = "TCP/IP STACK: LED command test";
#elif SECURE_CLIENT_TEST
		wi.text = "TCP/IP STACK: SSL connnection test";
#endif

		wi.g.y = 10;
		wi.g.x = 50;
		wi.g.width = 150;
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
		vTracePrintF(xTraceOpenLabel("LCD"), "message received");
		gwinSetText(ghLastMessage, received.ptr, 0);
		sprintf(msg, "%d.%03d:",  (int)(received.tick/1000), (int)(received.tick%1000));
		gwinSetText(ghLastMessageTime, msg, 0);

		vPortFree(received.ptr);
#else
		vTaskDelay(10*configTICK_RATE_HZ);
#endif

	}

}

#endif


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
	}
}
#endif

