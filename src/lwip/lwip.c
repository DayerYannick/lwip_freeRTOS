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
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/platform.h"
#include "mbedtls/debug.h"
#include "mbedtls/ctr_drbg.h"
#include "config/mbedTLSCerts.h"



//-- Private global variables --//
struct netif* stmNetif = 0;		// Object representing the network interface for lwip
volatile int dhcp_state;	// Current state of the DHCP state-machine
int tcpip_init_once = 0;	// Flag indicating that lwip was already initialized

#if USE_FREERTOS
TaskHandle_t connectHandle;	// For deleting the connect task after a return or a timeout
SemaphoreHandle_t createSema;	// Prevents errors while creating sockets ids
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

static int lwip_init_common(const int ip, const int mask, const int gateway, const char* hostname);	// A private function that initializes lwip and the hardware



//-- EVENTS --//
#if USE_FREERTOS

EventGroupHandle_t evGrLwip;		// LWIP Events
volatile int evGrLwipCreated = 0;	// Indicates that the eventGroup was created

const int EV_LWIP_INITIALIZED=			1<<0;	// Lwip is initialized
const int EV_LWIP_ETH_UP=				1<<1;	// Ethernet connection could be established
const int EV_LWIP_ETH_DOWN=				1<<2;	// Ethernet connection was lost (cable disconnected)
const int EV_LWIP_IP_ASSIGNED=			1<<3;	// The interface has an IP
const int EV_LWIP_DHCP_STARTED=			1<<4;	// The DHCP assignation protocol has started
const int EV_LWIP_DHCP_FAILED=			1<<5;	// The DHCP tries got higher than MAX_DHCP_TRIES

const int EV_LWIP_ERROR=				1<<23;	// There was an error with lwip



const int EV_LWIP_SOCKET_CONNECTED=		1<<0;	// The socket is connected
const int EV_LWIP_SOCKET_DISCONNECTED=	1<<1;	// The socket is disconnected
const int EV_LWIP_SOCKET_RECEIVED=		1<<2;	// The socket received data
const int EV_LWIP_SOCKET_CONNECT_TIMEOUT= 1<<3;	// The socket could not connect before the timeout value
const int EV_LWIP_SOCKET_RECV_TIMEOUT=	1<<4;	// The socket did not receive any data before the timeout value
const int EV_LWIP_SOCKET_SEND_TIMEOUT=	1<<5;	// The socket did not send the data before the timeout value
const int EV_LWIP_SOCKET_ACCEPT_TIMEOUT= 1<<6;	// The socket did not accept any connection before the timeout value
const int EV_LWIP_SOCKET_CONNECTED_INTERN= 1<<10; // Internal event indicating the end of lwip_connect

// sockets variables
#define MAX_SOCKET_NB MEMP_NUM_TCP_PCB
typedef struct {
	EventGroupHandle_t events;	// Socket events handle

	uint8_t isSocket;	// 'Socket initialized' flag

	ssl_context* ssl;	// The ssl context for this socket

	x509_crt* caChain;	// CA certificate chain (all the top level CA)
	x509_crt* ownCert;	// Our certificate chain (the path to the top CA)
	pk_context pkey;	// Our private key

} socket_t;
socket_t Sock[MAX_SOCKET_NB] = {{0}};


#if USE_MBEDTLS
#ifdef POLARSSL_MEMORY_BUFFER_ALLOC_C

#include "mbedtls/memory_buffer_alloc.h"
	unsigned char polarsslBuffer[200000] __attribute__ ((section (".ext_ram.bss")));

#endif
#endif


//-- Functions implementation --//


int lwip_wait_events(const int event, int timeout) {
	while(evGrLwipCreated == 0) vTaskDelay(1);	// If evGrLwip is not a created EventGroup

	return xEventGroupWaitBits(evGrLwip, event, pdFALSE, pdFALSE, timeout / portTICK_PERIOD_MS);	// Actually wait for the event
}

#endif	/* USE_FREERTOS */



#if LWIP_DHCP

int lwip_init_DHCP(const int waitAddress, const char* hostname) {
	int ret;
	char ip[16];	// ip address
	char nm[16];	// network mask
	char gw[16];	// gateway ip address
	EventBits_t uxBits;	// The EventGroupWaitBits answer
	//printf("Lwip init: DHCP\n");

	if( (ret = lwip_init_common(0, 0, 0, hostname)) != 0 )	// Call the main init function with ip parameters at 0
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

int lwip_init_static(const char* ip, const char* mask, const char* gateway, const char* hostname) {

	//printf("Lwip init with static IP: %s, %s, %s\n", ip, mask, gateway);

	if(lwip_init_common(inet_addr(ip), inet_addr(mask), inet_addr(gateway), hostname) != 0)	// Initialize lwip with ip parameters
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
static int lwip_init_common(const int ip, const int mask, const int gateway, const char* hostname) {
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
		//connectSema = xSemaphoreCreateMutexNamed("connect sema");
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

	}
	else {
		netif_set_down(stmNetif);
		xEventGroupSetBits(evGrLwip, EV_LWIP_ETH_DOWN | EV_LWIP_ERROR);
		xEventGroupClearBits(evGrLwip, EV_LWIP_ETH_UP);
		return -1;
	}

#if LWIP_NETIF_HOSTNAME
	if(hostname != NULL)
		netif_set_hostname(stmNetif, (char*)hostname);
#endif

#if USE_MBEDTLS
	debug_set_threshold(0);	// 0: nothing, 4: everything	// XXX here...
	threading_set_alt(polarssl_mutex_init_func, polarssl_mutex_free_func, polarssl_mutex_lock_func, polarssl_mutex_unlock_func);
	random_init();

#ifdef POLARSSL_MEMORY_BUFFER_ALLOC_C
	memory_buffer_alloc_init(polarsslBuffer, sizeof(polarsslBuffer));
#endif
#endif	/* USE_MBEDTLS */


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

			xEventGroupSetBits(evGrLwip, EV_LWIP_DHCP_STARTED);

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






#if USE_SIMPLE_SOCKET


int simpleSocket(protocole_t proto) {
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


	socket = lwip_socket(PF_INET, proto, 0);	// Create the socket

	if(socket > MAX_SOCKET_NB) {
		printf("Lwip: too many sockets!\n");
		return -1;
	}

	lwip_setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (void*)1, sizeof(int));

	if(socket >= 0) {
		Sock[socket].events = xEventGroupCreate();
		xEventGroupClearBits(Sock[socket].events, 0xFFFFFF);
		xEventGroupSetBits(Sock[socket].events, EV_LWIP_SOCKET_DISCONNECTED);

	//-- Set parameters --//
#if LWIP_TCP_KEEPALIVE
		if( lwip_setsockopt(socket, IPPROTO_TCP, LWIP_TCP_KEEPALIVE, (void*)&keepAliveIdleTime, sizeof(keepAliveIdleTime)) == -1)
			printf("ERROR: cannot set LWIP_TCP_KEEPALIVE.\n");

		if( lwip_setsockopt(socket, IPPROTO_TCP, TCP_KEEPINTVL, (void*)&interval, sizeof(interval)) == -1)
			printf("ERROR: cannot set TCP_KEEPINTVL.\n");

		if( lwip_setsockopt(socket, IPPROTO_TCP, TCP_KEEPCNT, (void*)&count, sizeof(count)) == -1)
			printf("ERROR: cannot set TCP_KEEPCNT.\n");
#endif

#if LWIP_SO_RCVTIMEO
		if( lwip_setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (void*)&recvTimeout, sizeof(recvTimeout)) == -1)
			printf("ERROR: cannot set SO_RCVTIMEO.\n");
#endif

#if LWIP_SO_SNDTIMEO
		if( lwip_setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (void*)&sendTimeout, sizeof(sendTimeout)) == -1)
			printf("ERROR: cannot set SO_SNDTIMEO.\n");
#endif

		Sock[socket].isSocket = 1;
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
	int keepAliveIdleTime = KEEPALIVE_IDLE;
	int interval = KEEPALIVE_INTVL;
	int count = KEEPALIVE_COUNT;
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

	while((ret = lwip_accept(socket, NULL, NULL)) < 0) {
		// Retrieve the error
		lwip_getsockopt(socket, SOL_SOCKET, SO_ERROR, &error, &optLen);

		if(error!= EWOULDBLOCK) {	// Error!
			//printf("Error in lwip_accept.\n");
			break;
		}
		else {	// Timeout! Set the event.
			xEventGroupSetBits(Sock[socket].events, EV_LWIP_SOCKET_ACCEPT_TIMEOUT);
		}
	}

	// Initialize the socket's parameters
	if(ret != -1) {

	//lwip_setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (int)1, sizeof(int));	// Turns off Nagle algorithm

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
		if( lwip_setsockopt(ret, IPPROTO_TCP, LWIP_TCP_KEEPALIVE, (void*)&keepAliveIdleTime, sizeof(keepAliveIdleTime)) == -1)
			printf("ERROR: cannot set LWIP_TCP_KEEPALIVE.\n");

		if( lwip_setsockopt(ret, IPPROTO_TCP, TCP_KEEPINTVL, (void*)&interval, sizeof(interval)) == -1)
			printf("ERROR: cannot set TCP_KEEPINTVL.\n");

		if( lwip_setsockopt(ret, IPPROTO_TCP, TCP_KEEPCNT, (void*)&count, sizeof(count)) == -1)
			printf("ERROR: cannot set TCP_KEEPCNT.\n");
#endif

#if LWIP_SO_RCVTIMEO
		if( lwip_setsockopt(ret, SOL_SOCKET, SO_RCVTIMEO, (void*)&recvTimeout, sizeof(recvTimeout)) == -1)
			printf("ERROR: cannot set SO_RCVTIMEO.\n");
#endif

#if LWIP_SO_SNDTIMEO
		if( lwip_setsockopt(ret, SOL_SOCKET, SO_SNDTIMEO, (void*)&sendTimeout, sizeof(sendTimeout)) == -1)
			printf("ERROR: cannot set SO_SNDTIMEO.\n");
#endif

	}
	Sock[ret].isSocket = 1;

	return ret;
}

/**
 * connectTask
 *	A FreeRTOS task to measure the time that it takes for lwip_connect to return
 *	and generate a timeout if it takes too long.
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

	// The function returned. Notify the main task with an event.
	xEventGroupSetBits(Sock[socket].events, EV_LWIP_SOCKET_CONNECTED_INTERN);

	// Wait for deletion
	while(1)
		vTaskDelay(portMAX_DELAY);
}

int simpleConnect(int socket, char* distantIP, int port) {
	volatile int ret = 0;
	int ipParam, portParam;
	 volatile int* params[4];
	EventBits_t uxBits;
	TaskHandle_t connectTaskHandle;

	// Transform ip from string to int
	if( (ipParam = inet_addr(distantIP)) == INADDR_NONE)
		return -1;

	// Transform port to network format (endianness)
	portParam = htons(port);

	// Prepare parameters for the task
	params[0] = &socket;
	params[1] = &ipParam;
	params[2] = &portParam;
	params[3] = &ret;


	xTaskCreate(connectTask, "Connect Task", configMINIMAL_STACK_SIZE, (void*)params, uxTaskPriorityGet(NULL), &connectTaskHandle);

	uxBits = xEventGroupWaitBits(Sock[socket].events, EV_LWIP_SOCKET_CONNECTED_INTERN, pdTRUE, pdFALSE, CONNECT_TIMEOUT_VAL/portTICK_PERIOD_MS);


	if((uxBits & EV_LWIP_SOCKET_CONNECTED_INTERN) != EV_LWIP_SOCKET_CONNECTED_INTERN) {	// Timeout
		xEventGroupSetBits(Sock[socket].events, EV_LWIP_SOCKET_CONNECT_TIMEOUT | EV_LWIP_SOCKET_DISCONNECTED);
		xEventGroupClearBits(Sock[socket].events, EV_LWIP_SOCKET_CONNECTED);
		ret = -1;
	}
	else {
		xEventGroupClearBits(Sock[socket].events, EV_LWIP_SOCKET_DISCONNECTED);
		xEventGroupSetBits(Sock[socket].events, EV_LWIP_SOCKET_CONNECTED);
	}

	vTaskDelete(connectTaskHandle);

	return ret;
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
		printf("Error while resolving DNS name.\n");
		return -1;
	}

	return ret;
}

int simpleSend(int socket, const unsigned char* data, size_t length) {
	int ret = 0;
	int error;
	int count = 0;
	socklen_t optLen = sizeof(error);

	if(length <= 0)
		return -1;

	do {
		ret = lwip_send(socket, data, length, 0);
		if(ret < 0) {
			lwip_getsockopt(socket, SOL_SOCKET, SO_ERROR, &error, &optLen);
			if(error != EWOULDBLOCK) { // Error
				break;
			}
			else {	// Timeout
				xEventGroupSetBits(Sock[socket].events, EV_LWIP_SOCKET_SEND_TIMEOUT);
			}
		}	// if(ret < 0)
		else {
			count += ret;
		}
	} while(count < length);

	return ret;
}

int simpleSendStr(int socket, const char* data) {
	int length;

	for(length = 0; data[length] != '\0'; ++length);

	++length;

	return simpleSend(socket, (unsigned char*)data, length);
}


int simpleRecv(int socket, unsigned char* data, size_t maxLength) {
	int ret = 0;
	int error;
	socklen_t optLen = sizeof(error);


	do {
		ret = lwip_recv(socket, data, maxLength, 0);

		if(ret < 0) {
			lwip_getsockopt(socket, SOL_SOCKET, SO_ERROR, &error, &optLen);
			if(error!= EWOULDBLOCK) {	// error!
				break;
			}
			else {	// Timeout.
				xEventGroupSetBits(Sock[socket].events, EV_LWIP_SOCKET_RECV_TIMEOUT);
			}
		}
	} while(ret < 0);	// while((ret = lwip_recv...

	return ret;
}

int simpleClose(int socket) {
	int ret = 0;

	vEventGroupDelete(Sock[socket].events);

	ret = lwip_close(socket);

	Sock[socket].isSocket = 0;


	return ret;
}




int socket_wait_events(int socket, const uint32_t events, int timeout) {
	if(Sock[socket].isSocket != 0)
		return xEventGroupWaitBits(Sock[socket].events, events&(~EV_LWIP_SOCKET_CONNECTED_INTERN), pdFALSE, pdFALSE, timeout / portTICK_PERIOD_MS);
	else
		return -1;
}


int socket_get_events(int socket, uint32_t events) {
	if(Sock[socket].isSocket != 0)
		return (xEventGroupGetBits(Sock[socket].events) & events);
	else
		return -1;
}

char* getMyIP(void) {
	return myIP;
}

int getSocketNb(void) {
	int sock, nb=0;
	for(sock=0; sock<MAX_SOCKET_NB; ++sock) {
		if(Sock[sock].isSocket)
			++nb;
	}
	return nb;
}

int getSocketNbMax(void) {
	return MAX_SOCKET_NB;
}

uint8_t socketValid(int socket) {
	return Sock[socket].isSocket;
}

#endif	/* USE_SIMPLESOCKET */


/*
 * MBED TLS
 */

#if USE_MBEDTLS

int sendHelper(void* fd, const unsigned char* buf, size_t len) {
	//printf("**ssl sending %d char: \"%s\" on socket %d.\n", len, buf, (int) fd);
#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("TLS send/recv"), "ssl send %d", len);
#endif
	return simpleSend((int)fd, buf, len);
}

int recvHelper(void* fd, unsigned char* buf, size_t len) {
	int ret;
#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("TLS send/recv"), "ssl recv WAIT");
#endif
	ret = simpleRecv((int)fd, buf, len);
	//printf("**ssl receiving %d char: \"%s\" on socket %d.\n", ret, buf, (int) fd);
#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("TLS send/recv"), "ssl recv END (%d)", ret);
#endif
	return ret;
}

int random_Hardware(void* fd, unsigned char* buf, size_t len) {
	const int timeout = 10;
	int ret;
	int i, j;
	uint32_t tmp;

#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("TLS rand"), "Start len = %d", len);
#endif
	//printf("**Rand%d. t=%d\n", (int)len, (int)xTaskGetTickCount());

	j = (len%4==0)?len/4:len/4-1;

	for(i=0; i<j; ++i) {
		if((ret=random_get(((uint32_t*)buf+i), timeout)) == NO_ERROR) {
			ret = 0;
		}
		else {
			printf("Error in random_get: %d", ret);
			return -1;
		}
	}

	// fill the last bytes (if not multiple of 4)
	if(len%4 != 0) {
		if((ret=random_get(&tmp, timeout)) != NO_ERROR) {
			printf("Error in random_get");
			return -1;
		}
		for(j=0; j<len%4; ++j)
			buf[(i*4)+j] = tmp & (0xFF<<(j*4)) >> (j*4);
	}

#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("TLS rand"), "END");
#endif
	//printf("**Rand end. t=%d\n", (int)xTaskGetTickCount());

	return ret;
}

void sslDebugHelper(void* fd, int level, const char* data) {
	printf("lvl%d: %s", level, data);
}

int secureSocket() {
	int socket;

	socket = simpleSocket(TCP);

	if(socket < 0)
		return socket;

	Sock[socket].ssl = NULL;

	return socket;
}

int secureBind(int socket, char* localIP, int port) {
	int ret;

	ret = simpleBind(socket, localIP, port);

	return ret;
}

int secureListen(int socket) {
	int ret;

	ret = simpleListen(socket);

	return ret;
}

int secureAccept(int socket) {
	int ret, clientSocket;

	clientSocket = simpleAccept(socket);

	if(clientSocket < 0)
		return clientSocket;

	printf("Connecting socket %d.\n", clientSocket);

	Sock[clientSocket].ssl = pvPortMalloc(sizeof(ssl_context));
	memset(Sock[clientSocket].ssl, 0, sizeof(ssl_context));

#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("mbedtls"), "ssl init");
#endif
	if( (ret = ssl_init(Sock[clientSocket].ssl)) != 0) {	// TODO move in secureScoket()?
		printf("Error in ssl_init.\n");
		vPortFree(Sock[clientSocket].ssl);	// Cancel the malloc
		Sock[clientSocket].ssl = NULL;
		secureClose(clientSocket);
		return ret;
	}

	ssl_set_endpoint(Sock[clientSocket].ssl, SSL_IS_SERVER);
	ssl_set_authmode(Sock[clientSocket].ssl, SSL_VERIFY_NONE);

	//ssl_set_rng(Sock[clientSocket].ssl, ctr_drbg_random, &ctr_drbg);	// default polarssl version of random
	ssl_set_rng(Sock[clientSocket].ssl, random_Hardware, 0);				// Using only the hardware RNG
	ssl_set_dbg(Sock[clientSocket].ssl, sslDebugHelper, NULL);
	ssl_set_bio(Sock[clientSocket].ssl, recvHelper, (void*)clientSocket, sendHelper, (void*)clientSocket);

	ssl_set_ciphersuites(Sock[clientSocket].ssl, ssl_list_ciphersuites());

	ssl_set_min_version(Sock[clientSocket].ssl, 3, 1);


	pk_init(&(Sock[clientSocket].pkey));

	if( (ret=pk_parse_key(&(Sock[clientSocket].pkey), (const unsigned char*)srv_key, sizeof(srv_key), NULL, 0) ) != 0)
		printf("pk_parse_key returned %d\n", ret);

	Sock[clientSocket].caChain = polarssl_malloc( sizeof(x509_crt) );
	Sock[clientSocket].ownCert = polarssl_malloc( sizeof(x509_crt) );

	x509_crt_init(Sock[clientSocket].caChain);
	x509_crt_init(Sock[clientSocket].ownCert);

	if( (ret=x509_crt_parse(Sock[clientSocket].caChain, (const unsigned char*) ca_crt, strlen(ca_crt)) ) != 0)
		printf("x509_crt_parse of the CA certificate returned %d\n", ret);
	if( (ret=x509_crt_parse(Sock[clientSocket].ownCert, (const unsigned char*) srv_crt, strlen(srv_crt)) ) != 0)
		printf("x509_crt_parse of our certificate returned %d\n", ret);

	// TODO: Add support for Certificate Revocation List (CRL) and common name check
	ssl_set_ca_chain(Sock[clientSocket].ssl, Sock[clientSocket].caChain, NULL, NULL);

	if( (ssl_set_own_cert(Sock[clientSocket].ssl, Sock[clientSocket].ownCert, &(Sock[clientSocket].pkey)) ) != 0)
		printf("ssl_set_own_cert returned %d\n", ret);



#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("mbedtls"), "handshake start");
#endif
	while((ret=ssl_handshake(Sock[clientSocket].ssl)) != 0) {
		if(ret != POLARSSL_ERR_NET_WANT_READ && ret != POLARSSL_ERR_NET_WANT_WRITE) {
			printf("Error: SSL_handshake: -0x%x.\n", -ret);
			secureClose(clientSocket);
			return ret;
		}

	}
#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("mbedtls"), "handshake END");
#endif

	return clientSocket;
}

int secureConnect(int socket, char* distantIP, int port) {
	int ret;

	if( (ret = simpleConnect(socket, distantIP, port)) != 0) {
		//printf("Error in simplesocket\n");
		return ret;
	}

	Sock[socket].ssl = pvPortMalloc(sizeof(ssl_context));
	memset(Sock[socket].ssl, 0, sizeof(ssl_context));

#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("mbedtls"), "ssl init");
#endif
	if( (ret = ssl_init(Sock[socket].ssl)) != 0) {
		printf("Error in ssl_init.\n");
		return ret;
	}

	ssl_set_endpoint(Sock[socket].ssl, SSL_IS_CLIENT);
	ssl_set_authmode(Sock[socket].ssl, SSL_VERIFY_REQUIRED);

	//ssl_set_rng(Sock[socket].ssl, ctr_drbg_random, &ctr_drbg);	// default polarssl version of random
	// Function that give random numbers (True RNG via hardware)
	ssl_set_rng(Sock[socket].ssl, random_Hardware, 0);
	// Debug handler to display messages
	ssl_set_dbg(Sock[socket].ssl, sslDebugHelper, NULL);
	// Input/ouput functions (TCP)
	ssl_set_bio(Sock[socket].ssl,	recvHelper, (void*)socket,
									sendHelper, (void*)socket);

	// Cipher suites to use (define SSL_CIPHERSUITES in mbedTLSConfig to change)
	ssl_set_ciphersuites(Sock[socket].ssl, ssl_list_ciphersuites());

	// Version minimum to use: SSL v3.1 (= TLS v1.0)
	ssl_set_min_version(Sock[socket].ssl, 3, 1);

	// ARC4 disabled (deprecated)
    ssl_set_arc4_support(Sock[socket].ssl, SSL_ARC4_DISABLED);

    // Certificates
    pk_init(&(Sock[socket].pkey));

	if( (ret=pk_parse_key(&(Sock[socket].pkey), (const unsigned char*)cli_key, sizeof(cli_key), NULL, 0) ) != 0)
		printf("pk_parse_key returned %d\n", ret);


	Sock[socket].caChain = polarssl_malloc( sizeof(x509_crt) );
	Sock[socket].ownCert = polarssl_malloc( sizeof(x509_crt) );

	x509_crt_init(Sock[socket].caChain);
	x509_crt_init(Sock[socket].ownCert);


	if( (ret=x509_crt_parse(Sock[socket].caChain, (const unsigned char*) ca_crt, strlen(ca_crt)) ) != 0)
		printf("x509_crt_parse CA returned %d\n", ret);

	debug_print_crt(Sock[socket].ssl, 1, "lwip.c", 1046, "parsed CA cert", Sock[socket].caChain);

	if( (ret=x509_crt_parse(Sock[socket].ownCert, (const unsigned char*) cli_crt, strlen(cli_crt)) ) != 0)
		printf("x509_crt_parse own certificate returned %d\n", ret);

	debug_print_crt(Sock[socket].ssl, 1, "lwip.c", 1051, "parsed own cert", Sock[socket].ownCert);

	// TODO: add support for CRL and expected commonName
	ssl_set_ca_chain(Sock[socket].ssl, Sock[socket].caChain, NULL, NULL);

	if( (ssl_set_own_cert(Sock[socket].ssl, Sock[socket].ownCert, &(Sock[socket].pkey)) ) != 0)
		printf("ssl_set_own_cert returned %d\n", ret);


#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("mbedtls"), "ssl handshake");
#endif
	while((ret=ssl_handshake(Sock[socket].ssl)) != 0) {
		if(ret != POLARSSL_ERR_NET_WANT_READ && ret != POLARSSL_ERR_NET_WANT_WRITE) {
			printf("Error: SSL_handshake: -0x%x.\n", -ret);
			return ret;
		}

	}
#if configUSE_TRACE_FACILITY
	vTracePrintF(xTraceOpenLabel("mbedtls"), "ssl handshake END");
#endif


	return 0;
}

int secureConnectDNS(int socket, char* name, int port) {
	int ret = 0;
	char ip[16];
	struct hostent* ent;

	ent = gethostbyname(name);	// Search for the ip


	if(ent != 0) {
		sprintf(ip, "%d.%d.%d.%d", ent->h_addr_list[0][0]&0xFF, ent->h_addr_list[0][1]&0xFF, ent->h_addr_list[0][2]&0xFF, ent->h_addr_list[0][3]&0xFF);
		ret = secureConnect(socket, ip, port);
		//printf("dns of %s => %s\n", name, ip);
	}
	else {
		printf("Error while resolving DNS name.\n");
		return -1;
	}

	return ret;
}

int secureSend(int socket, const unsigned char* data, size_t length) {
	int ret;

	if(length <= 0)
		return -1;


	ret = ssl_write(Sock[socket].ssl, data, length);

	return ret;
}

int secureSendStr(int socket, const char* data) {
	int length;

	for(length = 0; data[length] != '\0'; ++length);
		++length;

	return secureSend(socket, (unsigned char*)data, length);
}

int secureRecv(int socket, unsigned char* data, size_t maxLength) {
	int ret = 0;

	ret = ssl_read(Sock[socket].ssl, data, maxLength);

	return ret;
}

int secureClose(int socket) {

	int ret;

	if(socket >= 0) {
		if(Sock[socket].isSocket != 0 && Sock[socket].ssl != NULL) {
			pk_free(&Sock[socket].pkey);
			x509_crt_free(Sock[socket].caChain);
			x509_crt_free(Sock[socket].ownCert);
			ssl_free(Sock[socket].ssl);
			vPortFree(Sock[socket].ssl);
			Sock[socket].ssl = NULL;
		}
	}

	ret = simpleClose(socket);
	return ret;
}

#endif	/* USE_MBEDTLS */

#endif	/* USE_LWIP */
