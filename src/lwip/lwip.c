/*
 * lwip.c
 *
 *  Created on: 24 mars 2015
 *      Author: yannick.dayer
 */

// Public declarations
#include "lwip/lwip.h"

// Do not compile if we don't need LWIP
#if USE_LWIP

//#include <string.h>

//-- Lwip includes --//
#include "lwip/lwip/netif.h"
#include "lwip/lwip/tcpip.h"
#include "lwip/lwip/tcp.h"
#include "lwip/lwip/inet.h"
#include "lwip/arch/ethernetif.h"
#include "lwip/lwip/dhcp.h"
#include "lwip/lwip/sockets.h"
#include "lwip/lwip/netdb.h"

//-- Stm32 includes --//
#include "stm32/stm32f4x7_eth_bsp.h"
#include "stm32/stm32f4xx_eth.h"
#include "heivs/random.h"

//-- FreeRTOS includes --//
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"


//-- mbedTLS includes --//
#include "polarssl/ssl.h"
#include "polarssl/entropy.h"
#include "polarssl/platform.h"
#include "polarssl/debug.h"
//#include "polarssl/ctr_drbg.h"



//-- Private global variables --//
struct netif* stmNetif = 0;		// Object representing the network interface for lwip
volatile int dhcp_state;	// Current state of the DHCP state-machine
int tcpip_init_once = 0;	// Flag indicating that lwip was already initialized

#if USE_FREERTOS
TaskHandle_t connectHandle;	// For deleting the connect task after a return or a timeout
SemaphoreHandle_t createSema;	// Prevents errors while creating sockets ids
SemaphoreHandle_t connectSema;	// Ensures that the connect function is called once at a time
int semaCreated = 0;	// Indicates that the semaphores were created.
#endif

char myIP[16];

//-- Constants --//
// DHCP states
#define DHCP_START 1
#define DHCP_WAIT_ADDRESS 2
#define DHCP_ADDRESS_ASSIGNED 3
#define DHCP_TIMEOUT 4
#define DHCP_LINK_DOWN 5

#define MAX_DHCP_TRIES 4	/* The max number of time the DHCP can fail */


//-- Functions prototypes --//

#if USE_FREERTOS
static void dhcp_task(void* params);	// DHCP state-machine
static void connectTask(void* param);	// A task allowing a timeout detection on lwip_connect
#endif

static int lwip_init_common(const int ip, const int mask, const int gateway);	// A private function that initializes lwip and the hardware
void waitForLinkDown(void);

//-- EVENTS --//
#if USE_FREERTOS

EventGroupHandle_t evGrLwip;		// LWIP Events
volatile int evGrLwipCreated = 0;	// Indicates that the eventGroup was created

const int EV_LWIP_INITIALIZED=			1<<1;	// Lwip is initialized
const int EV_LWIP_ETH_UP=				1<<2;	// Ethernet connection could be established
const int EV_LWIP_ETH_DOWN=				1<<3;	// Ethernet connection was lost (cable disconnected)
const int EV_LWIP_IP_ASSIGNED=			1<<4;	// The interface has an IP
const int EV_LWIP_DHCP_FAILED=			1<<5;	// The DHCP tries got higher than MAX_DHCP_TRIES

const int EV_LWIP_ERROR=				1<<23;	// There was an error with lwip



const int EV_LWIP_SOCKET_CONNECTED=		1<<1;	// The socket is connected
const int EV_LWIP_SOCKET_DISCONNECTED=	1<<2;	// The socket is disconnected
const int EV_LWIP_SOCKET_RECEIVED=		1<<3;	// The socket received data
const int EV_LWIP_SOCKET_CONNECT_TIMEOUT= 1<<4;	// The socket could not connect before the timeout value
const int EV_LWIP_SOCKET_RECV_TIMEOUT=	1<<5;	// The socket did not receive any data before the timeout value
const int EV_LWIP_SOCKET_ACCEPT_TIMEOUT= 1<<6;	// The socket did not accept any connection before the timeout value
const int EV_LWIP_SOCKET_CONNECTED_INTERN= 1<<10; // Internal event indicating the end of lwip_connect

// sockets variables
#define MAX_SOCKET_NB MEMP_NUM_TCP_PCB
typedef struct {
	EventGroupHandle_t events;

	ssl_context* ssl;
	//x509_crt cacert;

} socket_t;
socket_t Sock[MAX_SOCKET_NB];

entropy_context entropy;



//-- Functions implementation --//


int lwip_wait_events(const int event, int timeout) {
	while(evGrLwipCreated == 0) vTaskDelay(1);	// If evGrLwip is not a created EventGroup

	return xEventGroupWaitBits(evGrLwip, event, pdFALSE, pdFALSE, timeout / portTICK_PERIOD_MS);	// Actually wait for the event
}

#endif	/* USE_FREERTOS */



#if LWIP_DHCP

int lwip_init_DHCP(const int waitAddress) {
	int ret;
	char ip[16];	// ip address
	char nm[16];	// network mask
	char gw[16];	// gateway ip address
	EventBits_t uxBits;	// The EventGroupWaitBits answer
	//printf("Lwip init: DHCP\n");

	if( (ret = lwip_init_common(0, 0, 0)) != 0 )	// Call the main init function with ip parameters at 0
		return ret;

	// DHCP part
	dhcp_state = DHCP_START;
	xTaskCreate(dhcp_task, "DHCP task", configMINIMAL_STACK_SIZE, NULL, uxTaskPriorityGet(NULL), NULL);	// Run the DHCP task

	if(waitAddress != 0) {	// Wait for the task to find an IP
		//printf("waiting dhcp...\n");
		uxBits = xEventGroupWaitBits(evGrLwip, EV_LWIP_IP_ASSIGNED | EV_LWIP_DHCP_FAILED, pdFALSE, pdFALSE, portMAX_DELAY);	// Wait for IP or Fail

		if((uxBits&EV_LWIP_DHCP_FAILED) != 0) {	// Returned because of DHCP_FAILED event
			return -1;
		}
		else if((uxBits&EV_LWIP_IP_ASSIGNED) != 0) {	// Returned because of IP_ASSIGNED event

		}

		strcpy(ip, inet_ntoa(stmNetif->ip_addr.addr));
		strcpy(nm, inet_ntoa(stmNetif->netmask.addr));
		strcpy(gw, inet_ntoa(stmNetif->gw.addr));
		//printf("assigned IP: %s, %s, %s\n", ip, nm, gw);
	}

	xEventGroupSetBits(evGrLwip, EV_LWIP_INITIALIZED);
	//printf("LWIP initialized.\n");
	return 0;
}
#endif	/* LWIP_DHCP */

int lwip_init_static(const char* ip, const char* mask, const char* gateway) {

	//printf("Lwip init with static IP: %s, %s, %s\n", ip, mask, gateway);

	if(lwip_init_common(inet_addr(ip), inet_addr(mask), inet_addr(gateway)) != 0)	// Initialize lwip with ip parameters
		return -1;

	memcpy(myIP, ip, 16);

	xEventGroupSetBits(evGrLwip, EV_LWIP_INITIALIZED | EV_LWIP_IP_ASSIGNED);
	//printf("LWIP initialized.\n");


	return 0;
}

/**
 * lwip_init_common
 *
 *
 *	@param ip: the address of the interface
 *	@param mask: the subnet mask of the interface
 *	@param gateway: the default gateway in the subnet
 *
 *	@returns 0 in case of success
 */
static int lwip_init_common(const int ip, const int mask, const int gateway) {
	struct ip_addr myIp;
	struct ip_addr myMask;
	struct ip_addr myGw;
	struct netif* ret;

	//-- EVENTS INIT --//

	if(evGrLwipCreated == 0) {	// Do not recreate the EventGroup if we already did
		evGrLwip = xEventGroupCreate();
		evGrLwipCreated = 1;
	}

	xEventGroupClearBits(evGrLwip, 0xFFFFFF);
	xEventGroupSetBits(evGrLwip, EV_LWIP_ETH_DOWN);


	//-- SEMAPHORES INIT --//

	if(semaCreated == 0) {	// Do not recreate the semaphores if we already did
		connectSema = xSemaphoreCreateMutexNamed("connect sema");
		createSema = xSemaphoreCreateMutexNamed("create sema");
		semaCreated = 1;
	}

	//-- BSP INIT --//

	bsp_eth_config();	// Low-level hardware configuration


	//-- LWIP NETIF INIT --//

	myIp.addr = ip;
	myMask.addr = mask;
	myGw.addr = gateway;

	if(tcpip_init_once == 0) {	// Only init the TCP IP once. ERROR otherwise
		tcpip_init(NULL, NULL);
		tcpip_init_once = 1;
	}

	if(stmNetif != 0) {	// Clear a past netif if we already created one once
		netif_remove(stmNetif);
		vPortFree(stmNetif);
	}

	// Create a new network interface (netif)
	stmNetif = pvPortMalloc(sizeof(struct netif));
	ret = netif_add(stmNetif, &myIp, &myMask, &myGw, NULL, ethernetif_init, tcpip_input);
	if(ret == NULL) {
		//printf("Error in lwip_init_common: netif_add failed\n");
		vPortFree(stmNetif);
		xEventGroupSetBits(evGrLwip, EV_LWIP_ERROR);
		return -1;
	}

	netif_set_default(stmNetif);


	//-- BSP STATUS CHECK --//

	if(bsp_eth_status() == (ETH_INIT_FLAG | ETH_LINK_FLAG)) {
		stmNetif->flags |= NETIF_FLAG_LINK_UP;
		netif_set_up(stmNetif);
		xEventGroupSetBits(evGrLwip, EV_LWIP_ETH_UP);
		xEventGroupClearBits(evGrLwip, EV_LWIP_ETH_DOWN);

		//**** TO REMOVE: test for link down ****//
		printf("Link up.\n");
		//ETH_DMAITConfig(ETH_DMA_IT_FBE | ETH_DMA_IT_AIS, ENABLE);
		//waitForLinkDown();

	}
	else {
		netif_set_down(stmNetif);
		xEventGroupSetBits(evGrLwip, EV_LWIP_ETH_DOWN | EV_LWIP_ERROR);
		xEventGroupClearBits(evGrLwip, EV_LWIP_ETH_UP);
		return -1;
	}


#if USE_MBEDTLS
	debug_set_threshold(2);	// 0: nothing, 4(+?): everything
	platform_set_malloc_free(pvPortMalloc, vPortFree);
	random_init();
	entropy_init(&entropy);
#endif

	return 0;
}




static void dhcp_task(void* params) {
	uint32_t ip_addr;
	uint32_t msCount = 0;
	char ip[16];

	while(1) {
		switch (dhcp_state) {
		case DHCP_START:
			dhcp_state = DHCP_WAIT_ADDRESS;

			dhcp_start(stmNetif);	// Start the DHCP procedure

			ip_addr = 0;

			break;

		case DHCP_WAIT_ADDRESS: {

			/* Read the new IP address */
			ip_addr = stmNetif->ip_addr.addr;

			if (ip_addr != 0) {	// The address was assigned
				dhcp_state = DHCP_ADDRESS_ASSIGNED;

				xEventGroupSetBits(evGrLwip, EV_LWIP_IP_ASSIGNED);

				strcpy(ip, inet_ntoa(ip_addr));
				memcpy(myIP, ip, 16);

			} else {	// ip_addr still not set (ip_addr == 0)
				/* DHCP timeout ? */
				if (stmNetif->dhcp->tries > MAX_DHCP_TRIES) {
					dhcp_state = DHCP_TIMEOUT;

					xEventGroupSetBits(evGrLwip, EV_LWIP_DHCP_FAILED | EV_LWIP_ERROR);

					/* Stop DHCP */
					dhcp_stop(stmNetif);

					vTaskDelete(xTaskGetCurrentTaskHandle()); // This task is useless now.

				}
			}
		}
			break;
		default:
			break;
		}	// switch(dhcp_state)

		vTaskDelay(DHCP_FINE_TIMER_MSECS / portTICK_PERIOD_MS);
		dhcp_fine_tmr();	// For DHCP retries
		msCount += DHCP_FINE_TIMER_MSECS;
		if(msCount >= DHCP_COARSE_TIMER_MSECS) {
			dhcp_coarse_tmr();	// Check for IP invalidity (default: after 60s)
			msCount = 0;
		}

	}	// while(1)
}




void waitForLinkDown(void) {
	while(bsp_eth_status() == (ETH_INIT_FLAG | ETH_LINK_FLAG));
	printf("Link down!\n");
}


#if USE_SIMPLE_SOCKET && USE_FREERTOS


int simpleSocket() {
	int socket = 0;
	//int i=0;

#if LWIP_TCP_KEEPALIVE
	int keepAliveIdleTime = KEEPALIVE_IDLE, interval = KEEPALIVE_INTVL, count = KEEPALIVE_COUNT;
#endif
#if LWIP_SO_RCVTIMEO
	int recvTimeout = RECV_TIMEOUT_VAL;
#endif
#if LWIP_SO_SNDTIMEO
	int sendTimeout = SEND_TIMEOUT_VAL;
#endif


	socket = lwip_socket(AF_INET, SOCK_STREAM, 0);	// Create the socket


	if(socket >= 0) {
		Sock[socket].events = xEventGroupCreate();
		xEventGroupClearBits(Sock[socket].events, 0xFFFFFF);
		xEventGroupSetBits(Sock[socket].events, EV_LWIP_SOCKET_DISCONNECTED);

	//-- Set parameters --//
#if LWIP_TCP_KEEPALIVE
		if( lwip_setsockopt(socket, IPPROTO_TCP, LWIP_TCP_KEEPALIVE, (void*)&keepAliveIdleTime, sizeof(keepAliveIdleTime)) == -1)
			i=0;//printf("ERROR: cannot set LWIP_TCP_KEEPALIVE.\n");

		if( lwip_setsockopt(socket, IPPROTO_TCP, TCP_KEEPINTVL, (void*)&interval, sizeof(interval)) == -1)
			i=0;//printf("ERROR: cannot set TCP_KEEPINTVL.\n");

		if( lwip_setsockopt(socket, IPPROTO_TCP, TCP_KEEPCNT, (void*)&count, sizeof(count)) == -1)
			i=0;//printf("ERROR: cannot set TCP_KEEPCNT.\n");
#endif

#if LWIP_SO_RCVTIMEO
		if( lwip_setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (void*)&recvTimeout, sizeof(recvTimeout)) == -1)
			i=0;//printf("ERROR: cannot set SO_RCVTIMEO.\n");
#endif

#if LWIP_SO_SNDTIMEO
		if( lwip_setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (void*)&sendTimeout, sizeof(sendTimeout)) == -1)
			i=0;//printf("ERROR: cannot set SO_SNDTIMEO.\n");
#endif

	}

	return socket;
}

int simpleBind(int socket, char* localIP, int port) {
	struct sockaddr_in ip;
	int ret = 0;

	ip.sin_family = AF_INET;
	ip.sin_addr.s_addr = inet_addr(localIP);
	ip.sin_port = htons(port);
	ip.sin_len = sizeof(ip);

	ret = lwip_bind(socket, (struct sockaddr*)&ip, sizeof(ip));

	return ret;
}

int simpleListen(int socket) {
	int ret = 0;

	ret = lwip_listen(socket, 0);

	return ret;
}

int simpleAccept(int socket) {

#if LWIP_TCP_KEEPALIVE
	int keepAliveIdleTime = KEEPALIVE_IDLE, interval = KEEPALIVE_INTVL, count = KEEPALIVE_COUNT;
#endif
#if LWIP_SO_RCVTIMEO
	int recvTimeout = RECV_TIMEOUT_VAL;
#endif
#if LWIP_SO_SNDTIMEO
	int sendTimeout = SEND_TIMEOUT_VAL;
#endif
	int ret = 0;
	int error;
	socklen_t optLen = sizeof(error);

	while((ret = lwip_accept(socket, NULL, NULL)) == -1) {
		lwip_getsockopt(socket, SOL_SOCKET, SO_ERROR, &error, &optLen);	// Retrieve the error

		if(error!= EWOULDBLOCK)	// Error!
			break;
		else {	// timeout: set the event
			xEventGroupSetBits(Sock[socket].events, EV_LWIP_SOCKET_ACCEPT_TIMEOUT);
			break;
		}
	}

	// Initialize the socket's parameters
	if(ret != -1) {

		if(ret >= MAX_SOCKET_NB) {
			printf("ERROR: Too many simultaneous sockets!\n");
			lwip_close(ret);
			return -1;
		}

		Sock[ret].events = xEventGroupCreate();
		xEventGroupClearBits(Sock[ret].events, 0xFFFFFF);
		xEventGroupSetBits(Sock[ret].events, EV_LWIP_SOCKET_CONNECTED);

//-- set parameters --//
#if LWIP_TCP_KEEPALIVE
		if( lwip_setsockopt(socket, IPPROTO_TCP, LWIP_TCP_KEEPALIVE, (void*)&keepAliveIdleTime, sizeof(keepAliveIdleTime)) == -1)
			i=0;//printf("ERROR: cannot set LWIP_TCP_KEEPALIVE.\n");

		if( lwip_setsockopt(socket, IPPROTO_TCP, TCP_KEEPINTVL, (void*)&interval, sizeof(interval)) == -1)
			i=0;//printf("ERROR: cannot set TCP_KEEPINTVL.\n");

		if( lwip_setsockopt(socket, IPPROTO_TCP, TCP_KEEPCNT, (void*)&count, sizeof(count)) == -1)
			i=0;//printf("ERROR: cannot set TCP_KEEPCNT.\n");
#endif

#if LWIP_SO_RCVTIMEO
		if( lwip_setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (void*)&recvTimeout, sizeof(recvTimeout)) == -1)
			i=0;//printf("ERROR: cannot set SO_RCVTIMEO.\n");
#endif

#if LWIP_SO_SNDTIMEO
		if( lwip_setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (void*)&sendTimeout, sizeof(sendTimeout)) == -1)
			i=0;//printf("ERROR: cannot set SO_SNDTIMEO.\n");
#endif

	}

	return ret;
}

/**
 * connectTask
 *	A FreeRTOS task to measure the time that it takes and generate a timeout
 *
 */
void connectTask(void* param) {
	int** params = param;
	int* ret = params[3];
	int socket = *(params[0]);
	struct sockaddr_in ip;
	ip.sin_addr.s_addr = *(params[1]);
	ip.sin_port = *(params[2]);
	ip.sin_family = AF_INET;
	ip.sin_len = sizeof(ip);

	*ret = lwip_connect(socket, (struct sockaddr*)&ip, ip.sin_len);

	xEventGroupSetBits(Sock[socket].events, EV_LWIP_SOCKET_CONNECTED_INTERN);	// The function returned.

	// The task sould be deleted now.
	while(1)
		vTaskDelay(100);
}

int simpleConnect(int socket, char* distantIP, int port) {
	int ret = 0, ret2 = 0;
	int ipParam, portParam;
	int* params[4];
	EventBits_t uxBits;

	ipParam = inet_addr(distantIP);	// Transform from string to int

	if(ipParam == INADDR_NONE)
		return -1;

	portParam = htons(port);	// Transform to network format (endianness)

	// Prepare parameters for the task
	params[0] = &socket;
	params[1] = &(ipParam);
	params[2] = &(portParam);
	params[3] = &ret;


	//if(socket != -1) {

	xSemaphoreTake(connectSema, portMAX_DELAY);
	xTaskCreate(connectTask, "Connect Task", configMINIMAL_STACK_SIZE, (void*)params, uxTaskPriorityGet(NULL), &connectHandle);

		uxBits = xEventGroupWaitBits(Sock[socket].events, EV_LWIP_SOCKET_CONNECTED_INTERN, pdTRUE, pdFALSE, CONNECT_TIMEOUT_VAL/portTICK_PERIOD_MS);
		xSemaphoreGive(connectSema);

		if((uxBits & EV_LWIP_SOCKET_CONNECTED_INTERN) != EV_LWIP_SOCKET_CONNECTED_INTERN) {	// Timeout
			xEventGroupSetBits(Sock[socket].events, EV_LWIP_SOCKET_CONNECT_TIMEOUT | EV_LWIP_SOCKET_DISCONNECTED);
			xEventGroupClearBits(Sock[socket].events, EV_LWIP_SOCKET_CONNECTED);
			ret2 = -1;
			//printf("Timeout of connect.\n");
		}
		else {
			xEventGroupClearBits(Sock[socket].events, EV_LWIP_SOCKET_DISCONNECTED);
			xEventGroupSetBits(Sock[socket].events, EV_LWIP_SOCKET_CONNECTED);
			ret2 = ret;
		}
		vTaskDelete(connectHandle);
	//}
	//else {
		//printf("Error: socket not found.\n");
		//return -1;
	//}

	ret = ret2;

	return ret2;
}

int simpleConnectDNS(int socket, char* name, int port) {
	int ret = 0;
	char ip[16];
	struct hostent* ent;

	ent = lwip_gethostbyname(name);	// Search for the ip


	if(ent != 0) {
		sprintf(ip, "%d.%d.%d.%d", ent->h_addr_list[0][0]&0xFF, ent->h_addr_list[0][1]&0xFF, ent->h_addr_list[0][2]&0xFF, ent->h_addr_list[0][3]&0xFF);
		ret = simpleConnect(socket, ip, port);
		//printf("dns of %s => %s\n", name, ip);
	}
	else {
		//printf("Error while resolving DNS name.\n");
		return -1;
	}

	return ret;
}

int simpleSend(int socket, const unsigned char* data, size_t length) {
	int ret = 0;

	if(length <= 0)
		return -1;

	ret = lwip_send(socket, data, length, 0);	// FIXME Check if wrong: put Sock[id].socket instead of socket

	return ret;
}

int simpleSendStr(int socket, const unsigned char* data) {
	int length;

	for(length = 0; data[length] != '\0'; ++length);
	++length;

	return simpleSend(socket, (void*)data, length);
}

int simpleRecv(int socket, unsigned char* data, size_t maxLength) {
	int ret = 0;
	int error;
	socklen_t optLen = sizeof(error);

	while((ret = lwip_recv(socket, data, maxLength, 0)) == -1) {
		//vTracePrintF(xTraceOpenLabel("LwIP"), "lwip_recv timeout");
		lwip_getsockopt(socket, SOL_SOCKET, SO_ERROR, &error, &optLen);
		if(error!= EWOULDBLOCK) {
			break;
		}
		else {
			xEventGroupSetBits(Sock[socket].events, EV_LWIP_SOCKET_RECV_TIMEOUT);
		}
	}

	//vTracePrintF(xTraceOpenLabel("SimpleSock"), "lwip_recv exit. ret = %d", ret);
	return ret;
}

int simpleClose(int socket) {
	int ret = 0;

	vEventGroupDelete(Sock[socket].events);

	ret = lwip_close(socket);


	return ret;
}

#endif	/* USE_SIMPLESOCKET && USE_FREERTOS */


int socket_wait_events(int socket, const int events, int timeout) {

	return xEventGroupWaitBits(Sock[socket].events, events&(~EV_LWIP_SOCKET_CONNECTED_INTERN), pdFALSE, pdFALSE, timeout / portTICK_PERIOD_MS);
}

char* getMyIP(void) {
	return myIP;
}


#if USE_MBEDTLS && USE_FREERTOS

int sendHelper(void* fd, const unsigned char* buf, size_t len) {
	//printf("**ssl sending %d char: \"%s\" on socket %d.\n", len, buf, (int) fd);
	vTracePrintF(xTraceOpenLabel("mbedtls"), "ssl send");
	return simpleSend((int)fd, buf, len);
}

int recvHelper(void* fd, unsigned char* buf, size_t len) {
	int ret;
	ret = simpleRecv((int)fd, buf, len);
	//printf("**ssl receiving %d char: \"%s\" on socket %d.\n", ret, buf, (int) fd);
	return ret;
}

int randomHelper(void* fd, unsigned char* buf, size_t len) {
	int ret;
	int i, j;
	uint32_t tmp;

	for(i=0; i<(len/4)-1; ++i) {
		if(random_get(((uint32_t*)buf+i), 50) == NO_ERROR) {
			ret = 0;
			//printf("rn%d: %d\n", i, (int)((uint32_t*)buf)[i]);
		}
		else {
			printf("Error in random_get: %d", ret);
			return -1;
		}
	}

	random_get(&tmp, 50);
	for(j=0; j<len%4; ++j)
		buf[(i*4)+j] = tmp & (0xFF<<(j*4)) >> (j*4);

	//printf("**Random: %d.\n", (int)buf[0]);

	return ret;
}

void sslDebugHelper(void* fd, int level, const char* data) {
	printf("mbedTLS lvl%d: %s", level, data);
}

int securedSocket() {
	int socket;
	//int ret;
	//char pers[] = "random string";	// TODO: pass the personalization string in parameter or use a unique global one (i.e derived from the MAC address)

	socket = simpleSocket();

	if(socket < 0)
		return socket;

	Sock[socket].ssl = pvPortMalloc(sizeof(ssl_context));

	//x509_crt_init(&(Sock[id].cacert));




	//x509_crt_parse(&(Sock[id].cacert), (const unsigned char*) test_ca_list, strlen(test_ca_list));



	return socket;
}

int securedBind(int socket, char* localIP, int port) {
	return -1;
}

int securedListen(int socket) {
	return -1;
}

int securedAccept(int socket) {
	return -1;
}

int securedConnect(int socket, char* distantIP, int port) {
	int ret;

	if( (ret = simpleConnect(socket, distantIP, port)) != 0)
		return ret;

	vTracePrintF(xTraceOpenLabel("mbedtls"), "ssl init");
	if( (ret = ssl_init(Sock[socket].ssl)) != 0) {
		printf("Error in ssl_init.\n");
		securedClose(socket);
		return ret;
	}

	vTracePrintF(xTraceOpenLabel("mbedtls"), "ssl set");

	ssl_set_endpoint(Sock[socket].ssl, SSL_IS_CLIENT);
	ssl_set_authmode(Sock[socket].ssl, SSL_VERIFY_NONE);

	//ssl_set_rng(Sock[id].ssl, ctr_drbg_random, &ctr_drbg);	// default polarssl version of random
	ssl_set_rng(Sock[socket].ssl, randomHelper, 0);
	ssl_set_dbg(Sock[socket].ssl, sslDebugHelper, NULL);
	ssl_set_bio(Sock[socket].ssl, recvHelper, (void*)socket, sendHelper, (void*)socket);

	ssl_set_ciphersuites(Sock[socket].ssl, ssl_list_ciphersuites());

	ssl_set_min_version(Sock[socket].ssl, 3, 1);


	vTracePrintF(xTraceOpenLabel("mbedtls"), "ssl handshake");
	while((ret=ssl_handshake(Sock[socket].ssl)) != 0) {
		if(ret != POLARSSL_ERR_NET_WANT_READ && ret != POLARSSL_ERR_NET_WANT_WRITE) {
			printf("Error: SSL_handshake: -0x%x.\n", -ret);
			return -1;
		}

	}


	return 0;
}

int securedConnectDNS(int socket, char* name, int port) {
	int ret = 0;
	char ip[16];
	struct hostent* ent;

	ent = gethostbyname(name);	// Search for the ip


	if(ent != 0) {
		sprintf(ip, "%d.%d.%d.%d", ent->h_addr_list[0][0]&0xFF, ent->h_addr_list[0][1]&0xFF, ent->h_addr_list[0][2]&0xFF, ent->h_addr_list[0][3]&0xFF);
		ret = securedConnect(socket, ip, port);
		//printf("dns of %s => %s\n", name, ip);
	}
	else {
		//printf("Error while resolving DNS name.\n");
		return -1;
	}

	return ret;
}

int securedSend(int socket, const unsigned char* data, size_t length) {
	int ret;

	if(length <= 0)
		return -1;


	ret = ssl_write(Sock[socket].ssl, data, length);

	return ret;
}

int securedSendStr(int socket, const unsigned char* data) {
	int length;

	for(length = 0; data[length] != '\0'; ++length);
	++length;

	return securedSend(socket, (void*)data, length);
}

int securedRecv(int socket, unsigned char* data, size_t maxLength) {
	int ret = 0;

	ret = ssl_write(Sock[socket].ssl, data, maxLength);

	return ret;
}

int securedClose(int socket) {


	if(socket >= 0) {
		ssl_free(Sock[socket].ssl);
	//	memset(Sock[socket].ssl, 0, sizeof(ssl_context));
		vPortFree(Sock[socket].ssl);
	}
	return simpleClose(socket);
}

#endif	// USE_MBEDTLS && USE_FREERTOS

#endif	/* USE_LWIP */
