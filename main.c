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



#include "mbedtls/md5.h"
#include "mbedtls/sha1.h"
#include "mbedtls/des.h"
#include "mbedtls/aes.h"

//-- Defines --//

#define MY_IP_BY_DHCP 1	/* 0: use MY_IP / 1: use DHCP */


#if !MY_IP_BY_DHCP
#define MY_IP "192.168.1.5"
#define MY_MASK "255.255.255.0"
#define MY_GW "192.168.1.1"
#define PC_IP "192.168.1.2"
#else
#define PC_IP "153.109.5.178"
#endif

#define TCP_PORT 4433



#define CLIENT_TEST 0	/* Connect to PC_IP and send messages */
#define SERVER_TEST 0	/* Accept incoming connections */
#define HTTP_TEST 0		/* Send a request to http://www.hevs.ch */
#define HTTP_SERVER_TEST 0	/* A (very) simple HTTP server... */

#define LED_COMMAND_TEST 0	/* Remote control of the LEDs on the board */

#define SECURE_CLIENT_TEST 0	/* Connection to a secured server via TLS */
#define SECURE_SERVER_TEST 1	/* Allows a client to connect via TLS */
#define HTTPS_TEST 0			/* Send a request to https://www.google.ch */



#define USE_DISPLAY 1	/* Set to 1 to display info about the system on the LCD screen */
#if USE_DISPLAY
#include "ugfx/gfx.h"
#endif

#if (SERVER_TEST || SECURE_SERVER_TEST) && USE_DISPLAY
#define DISPLAY_MSG_ON_LCD 1	/* 1: Displays the last received message on LCD screen */
#endif

#define USE_AUDIO 0	/* Set to 1 to be notified with a sound when an event occurs */
#if USE_AUDIO
#include "heivs/audio.h"
#endif


#define MSG_LEN_MAX 2000	/* The maximum length (in bytes) of a message received via TCP */


void main_task(void* param);
void led_task(void* param);
#if USE_DISPLAY
void lcd_task(void* param);
#endif
#if USE_AUDIO
void audio_task(void* param);
#endif

#if HTTP_SERVER_TEST || SERVER_TEST || LED_COMMAND_TEST || SECURE_SERVER_TEST
void clientHandle_task(void* param);
#endif

#if CLIENT_TEST
void clientToIP_task(void* param);
#endif

#if SECURE_CLIENT_TEST
void clientToIPSecured_task(void* param);
#endif


#if HTTP_TEST
void clientTohttp_task(void* param);
#endif

#if HTTPS_TEST
void clientTohttps_task(void* param);
#endif



#if SERVER_TEST || LED_COMMAND_TEST || HTTP_SERVER_TEST || SECURE_SERVER_TEST
#define ClNbMax 3
volatile int s[ClNbMax];
#endif


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
	unsigned char digest[20];
	char hello[] = "Hello, world!";
 	printf("\n");

/*
 * TEMPORARY TESTS
 * MD5, SHA1, DES, 3DES, AES
 */


#if DISPLAY_MSG_ON_LCD
 	LCD_msgQueue = xQueueCreateNamed(1, sizeof(queueLCDMsg_t), "LCD_msg queue");
#endif

#if USE_AUDIO
 	AUDIO_msgQueue = xQueueCreateNamed(1, sizeof(queueAUDIOMsg_t), "AUDIO_msg queue");
#endif


 	printf("MD5('%s') = ", hello);


#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("speed"), "MD5 start");
#endif
 	md5( (unsigned char*)hello, 13, digest);
#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("speed"), "MD5 end");
#endif

 	for(i=0; i<16; ++i) {
 		printf("%02X", digest[i]);
 	}

 	printf("\n");

 	printf("SHA1('%s') = ", hello);

#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("speed"), "SHA1 start");
#endif
 	sha1( (unsigned char*)hello, 13, digest);
#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("speed"), "SHA1 end");
#endif

 	for(i=0; i<20; ++i) {
 		printf("%02X", digest[i]);
 	}

 	printf("\n");

 	{
 		unsigned char output[8];
 		unsigned char key[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x03, 0x02, 0x01};
 		des_context ctx;
 		des_init(&ctx);
#if configUSE_TRACE_FACILITY
 		vTracePrintF(xTraceOpenLabel("speed"), "des_ecb start");
#endif
		des_setkey_enc(&ctx, key);
		des_crypt_ecb(&ctx, (unsigned char*)"datatocr", output);

#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("speed"), "des_ecb end");
#endif

		printf("DES(\"datatocr\", \"0001020304030201\") = ");

	 	for(i=0; i<8; ++i) {
	 		printf("%02X", output[i]);
	 	}

	 	printf("\n");
 	}


 	{
 		unsigned char output[8];
 	 	unsigned char key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};
 		des3_context ctx;
 		des3_init(&ctx);
#if configUSE_TRACE_FACILITY
 		vTracePrintF(xTraceOpenLabel("speed"), "3des_ecb start");
#endif
		des3_set3key_enc(&ctx, key);
		des3_crypt_ecb(&ctx, (unsigned char*)"datatocr", output);

#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("speed"), "3des_ecb end");
#endif

		printf("DES3(\"datatocr\", \"00010203040506070807060504030201\") = ");

	 	for(i=0; i<8; ++i) {
	 		printf("%02X", output[i]);
	 	}

	 	printf("\n");
 	}
 	{
 		aes_context ctx;
 	 	unsigned char output[16];
 	 	unsigned char key[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01,
 	 							 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A};

 	 	aes_init(&ctx);

#if configUSE_TRACE_FACILITY
 	 	vTracePrintF(xTraceOpenLabel("speed"), "aes_ecb start");
#endif
 	 	aes_setkey_enc(&ctx, key, 256);
 	 	aes_crypt_ecb(&ctx, AES_ENCRYPT, (unsigned char*)"datatocrypt12345", output);

#if configUSE_TRACE_FACILITY
 		vTracePrintF(xTraceOpenLabel("speed"), "aes_ecb end");
#endif

 		printf("AES(\"datatocrypt12345\", \"000102030405060708070605040302010908070605040302030405060708090A\") = ");

 		for(i=0; i<16; ++i) {
 			printf("%02X ", output[i]);
 		}

 		printf("\n");
 	 }
	 printf("\n");

	xTaskCreate(led_task, "Led Task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);	// Indicates that the system is running
	xTaskCreate(main_task, "Main Task", configMINIMAL_STACK_SIZE*6, NULL, 3, NULL);	// Runs the tests
#if USE_DISPLAY
	xTaskCreate(lcd_task, "LCD Task", configMINIMAL_STACK_SIZE, NULL, 3, NULL);	// Displays informations on the screen
#endif
#if USE_AUDIO
	xTaskCreate(audio_task, "Audio Task", configMINIMAL_STACK_SIZE, NULL, 2, NULL);	// Plays sounds when something happens
#endif
	vTaskStartScheduler();	// Start FreeRTOS scheduler

	return 0;
}	/* main */


/*
 * Socket Task
 */
void main_task(void* param) {

	#if SERVER_TEST || LED_COMMAND_TEST || HTTP_SERVER_TEST || SECURE_SERVER_TEST
	int socket;
	int i;
#endif

	printf("Starting main task.\n");


#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("main"), "Init start");
#endif

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


#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("main"), "Init end");
#endif


#if CLIENT_TEST
	xTaskCreate(clientToIP_task, "client task", configMINIMAL_STACK_SIZE*16, NULL, uxTaskPriorityGet(NULL), NULL);
#endif

#if SECURE_CLIENT_TEST
	xTaskCreate(clientToIPSecured_task, "SecuClient task", configMINIMAL_STACK_SIZE*6, NULL, uxTaskPriorityGet(NULL), NULL);
	//xTaskCreate(clientToIPSecured_task, "SecuClient2 task", configMINIMAL_STACK_SIZE*6, NULL, uxTaskPriorityGet(NULL), NULL);
#endif


#if HTTP_TEST
	xTaskCreate(clientTohttp_task, "http client task", configMINIMAL_STACK_SIZE * 8, NULL, uxTaskPriorityGet(NULL), NULL);
#endif

#if HTTPS_TEST
	xTaskCreate(clientTohttps_task, "https client task", configMINIMAL_STACK_SIZE*8, NULL, uxTaskPriorityGet(NULL), NULL);
#endif


#if SERVER_TEST || LED_COMMAND_TEST  || HTTP_SERVER_TEST || SECURE_SERVER_TEST

	const int clientHandleTaskStackSize = ((MSG_LEN_MAX/configMINIMAL_STACK_SIZE)+4) * configMINIMAL_STACK_SIZE;
	socket = simpleSocket();
	if(socket == -1)
		printf("ERROR while creating socket\n");

	//printf("socket = %d.\n", socket);
	if(simpleBind(socket, getMyIP(), TCP_PORT) == -1)
		printf("ERROR in Bind\n");
	if(simpleListen(socket) == -1)
		printf("ERROR in Listen\n");

	printf("accepting...\n");

#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("main"), "Accepting");
#endif

	for(i=0;i<ClNbMax;++i) {
		s[i] = -1;
	}
	while(1) {
#if SECURE_SERVER_TEST
		int tempS = secureAccept(socket);
#else
		int tempS = simpleAccept(socket);
#endif

		if(tempS < 0)
			printf("ERROR in Accept.\n");
		else {
			for(i=0;i<ClNbMax;++i)
				if(s[i] == -1)
					break;

			if(i==ClNbMax) {
				printf("ERROR: Cannot create socket: too many clients.");
				secureClose(tempS);
			}
			else {
				s[i] = tempS;
				xTaskCreate(clientHandle_task, "Client Handle", clientHandleTaskStackSize, (void*)i, uxTaskPriorityGet(NULL), NULL);
			}
		}
	}

#endif


	vTaskDelete(NULL);

}	/* main_task */


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

#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("client"), "connect");
#endif
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

#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("client"), "disconnect");
#endif
	printf("Connection %d closed.\n", i);

	simpleClose(s[i]);


#if USE_AUDIO
	toSendAudio.pitch = 500;
	toSendAudio.duration = 500;
	xQueueSend(AUDIO_msgQueue, &toSendAudio, portMAX_DELAY);
#endif
	s[i] = -1;

	vTaskDelete(NULL);
} /* clientHandle_task */
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
}	/* clientToIP_task */

#endif


#if SECURE_CLIENT_TEST
void clientToIPSecured_task(void* param) {
	int socket;
	int count=0;
	int ret;
	char data[30];
	char RData[200];
	int error = 0;

	vTaskDelay(1000);

	while(1) {
		error = 0;

		printf("creating socket.\n");

#if configUSE_TRACE_FACILITY
		vTracePrintF(xTraceOpenLabel("SecuClient"), "Create socket");
#endif
		socket = secureSocket();
		if(socket < 0)
			printf("Error on socket creation.\n");
		else {
			printf("Trying connection to IP with socket %d.\n", socket);

#if configUSE_TRACE_FACILITY
			vTracePrintF(xTraceOpenLabel("SecuClient"), "Connect socket %d", socket);
#endif
			ret = secureConnect(socket, PC_IP, TCP_PORT);
			if(ret < 0) {
				printf("Error on connect.\n");
			}
			else {
				while(error == 0) {
					snprintf(data, 30, "GET localhost http/1.0 \r\n\r\n");
					//snprintf(data, 27, "msg %3d from socket %1d \n\r\n\r", count, socket);
					if(++count >= 1000)
						count = 0;

					printf("Client sending to IP: %s\n", data);

	#if configUSE_TRACE_FACILITY
					vTracePrintF(xTraceOpenLabel("SecuClient"), "send");
	#endif
					//ret = secureSendStr(socket, (unsigned char*)data);
					ret = secureSend(socket, (unsigned char*)data, 27);

					if(ret < 0) {
						printf("Error on send on socket %d.\n", socket);
						error = 1;
					}
					else {
						//do {
							ret = secureRecv(socket, (unsigned char*)RData, 200);
							if(ret < 0) {
								switch(-ret) {
								case 0x7880:
									printf("Server notified that the connection is going to be closed.\n");
									break;
								default:
									printf("Error on recv on socket %d: returned -%04X.\n", socket, -ret);
								}
								error = 1;
							}
							else if(ret != 0) {
								printf("received %d char on socket %d: %.*s\n", ret, socket, ret, RData);
							}
							else {
								printf("Recv returned 0 char on socket %d.\n", socket);
								error = 1;
							}
						//} while(ret>0);
					}
				}	// while(error == 0)
			}

#if configUSE_TRACE_FACILITY
			vTracePrintF(xTraceOpenLabel("SecuClient"), "Close socket");
#endif
			secureClose(socket);
			printf("Socket closed.\n");
		}

		//breakpoint();


	}	// While(1)
}	/* clientToIPSecured_task */

#endif


#if SECURE_SERVER_TEST
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

#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("client"), "connect");
#endif
	msg[MSG_LEN_MAX-1] = '\0';

	while(1) {
		printf("mem free: %d\n", xPortGetFreeHeapSize());
		ret = secureRecv(s[i], (unsigned char*)msg, MSG_LEN_MAX-1);

		if(ret < 0) {
			switch(-ret) {
			case 0x7880:
				printf("Client %d notified us that the connection is going to be closed.\n", i);
				break;
			default:
				printf("error with client %d: returned -0x%X\n", i, -ret);
			}
			break;
		}
		else if(ret == 0)
			break;
		else {
#if DISPLAY_MSG_ON_LCD
			toSendLCD.tick = xTaskGetTickCount();
			if(uxQueueSpacesAvailable(LCD_msgQueue) != 0) {
				toSendLCD.ptr = pvPortMalloc(ret+1);
				memcpy(toSendLCD.ptr, msg, ret);
				toSendLCD.ptr[ret] = '\0';
				if(xQueueSend(LCD_msgQueue, &toSendLCD, 0) != pdTRUE)
					vPortFree(toSendLCD.ptr);
			}
#else
			printf("Message received on server: %.*s", ret, msg);
#endif


#if USE_AUDIO
			xQueueSend(AUDIO_msgQueue, &toSendAudio, 0);
#endif

			secureSend(s[i], (unsigned char*)"OK!\n", 4);

		}
	}

#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("client"), "disconnect");
#endif
	printf("Connection %d closed.\n", i);

	secureClose(s[i]);


#if USE_AUDIO
	toSendAudio.pitch = 500;
	toSendAudio.duration = 500;
	xQueueSend(AUDIO_msgQueue, &toSendAudio, portMAX_DELAY);
#endif
	s[i] = -1;

	vTaskDelete(NULL);
}	/* clientHandle_task */
#endif

#if HTTP_TEST
void clientTohttp_task(void* param) {
	int ret;
	char data[MSG_LEN_MAX];
	int socket;

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
				if(simpleSendStr(socket, (unsigned char*)"POST /img/logo-hes-so-valais.png HTTP/1.1\r\n"
						"Host: www.hevs.ch\r\n"
						"Accept: */*\r\n"
						"Content-Type: text/html\r\n"
						"Content-Length: 0\r\n\r\n") != -1) {
					printf("--Message sent.\n");

					vTaskDelay(2000);

					do {
						ret = simpleRecv(socket, (unsigned char*)data, MSG_LEN_MAX);
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
}	/* clientTohttp_task */
#endif


#if HTTPS_TEST
void clientTohttps_task(void* param) {
	int socket;
	int ret;
	char data[MSG_LEN_MAX];

	vTaskDelay(1000);

	while(1) {

		printf("creating socket.\n");

#if configUSE_TRACE_FACILITY
		vTracePrintF(xTraceOpenLabel("SecuClient"), "Create socket");
#endif
		socket = secureSocket();
		if(socket < 0)
			printf("Error on socket creation.\n");
		else {
			printf("Trying connection to address with socket %d.\n", socket);

#if configUSE_TRACE_FACILITY
			vTracePrintF(xTraceOpenLabel("SecuClient"), "Connect socket %d", socket);
#endif
			ret = secureConnectDNS(socket, "www.google.ch", 443);
			if(ret < 0) {
				printf("Error on connect.\n");
			}
			else {

#if configUSE_TRACE_FACILITY
				vTracePrintF(xTraceOpenLabel("SecuClient"), "send");
#endif
				ret = secureSendStr(socket, (unsigned char*)"POST /images/srpr/logo11w.png HTTP/1.1\r\n"
						"Host: www.google.ch\r\n"
						"Accept: */*\r\n"
						"Content-Type: text/html\r\n"
						"Content-Length: 0\r\n\r\n");
				if(ret < 0) {
					printf("Error on send.\n");
				}
				else {
					do {
						ret = secureRecv(socket, (unsigned char*)data, MSG_LEN_MAX);
						if(ret < 0) {
							printf("\nError on recv.\n");
						}
						else if(ret == 0) {
							printf("\nEnd connection received.\n");
						}
						else {
							printf("%.*s", ret, data);
						}
					} while(ret > 0);
				}
			}

#if configUSE_TRACE_FACILITY
			vTracePrintF(xTraceOpenLabel("SecuClient"), "Close socket");
#endif
			secureClose(socket);
			printf("Socket closed.\n");
		}

		breakpoint();

		vTaskDelay(2000);

	}	// While(1)
}	/* clientTohttps_task */
#endif

void led_task(void* param) {
	int8_t pwm = 0;
	uint8_t pwmInc = 1;
	const uint8_t ratems = 100;
	while (1) {
		bsp_led_set_pwm(0, pwm);
		if(pwmInc != 0) {
			if ((pwm+=ratems/10+1) >= 100) {
				pwmInc = 0;
				pwm = 100;
			}
		}
		else {
			if((pwm-=ratems/10+1) <= 10) {
				pwmInc = 1;
				pwm = 10;
			}
		}
		vTaskDelay((float)ratems/1000 * configTICK_RATE_HZ);
	}
}	/* led_task */

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
		wi.text = "TCP/IP STACK: SSL client test";
#elif SECURE_SERVER_TEST
		wi.text = "TCP/IP STACK: SSL server test";
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
		gwinSetText(ghLastMessage, received.ptr, 0);
		sprintf(msg, "%d.%03d:",  (int)(received.tick/1000), (int)(received.tick%1000));
		gwinSetText(ghLastMessageTime, msg, 0);

		vPortFree(received.ptr);
#else
		vTaskDelay(10*configTICK_RATE_HZ);
#endif

	}

}	/* lcd_task */

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
}	/* audio_task */
#endif
