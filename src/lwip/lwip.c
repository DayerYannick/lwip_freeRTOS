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

//-- FreeRTOS includes --//
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

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

const int MAX_DHCP_TRIES = 4;	// The max number of time the DHCP can fail


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


EventGroupHandle_t evGrSock[MEMP_NUM_TCP_PCB];	// Stores one EventGroup per socket
int idOfevGrSock[MEMP_NUM_TCP_PCB];				// Links the socket number to the ID of his EventGroup in evGrSock
static int getID(int socket);					// Gives the ID of a socket

const int EV_LWIP_SOCKET_CONNECTED=		1<<1;	// The socket is connected
const int EV_LWIP_SOCKET_DISCONNECTED=	1<<2;	// The socket is disconnected
const int EV_LWIP_SOCKET_RECEIVED=		1<<3;	// The socket received data
const int EV_LWIP_SOCKET_CONNECT_TIMEOUT= 1<<4;	// The socket could not connect before the timeout value
const int EV_LWIP_SOCKET_RECV_TIMEOUT=	1<<5;	// The socket did not receive any data before the timeout value
const int EV_LWIP_SOCKET_ACCEPT_TIMEOUT= 1<<6;	// The socket did not accept any connection before the timeout value
const int EV_LWIP_SOCKET_CONNECTED_INTERN= 1<<10; // Internal event indicating the end of lwip_connect


int lwip_wait_events(const int event, int timeout) {
	while(evGrLwipCreated == 0) vTaskDelay(1);	// If evGrLwip is not a created EventGroup

	return xEventGroupWaitBits(evGrLwip, event, pdFALSE, pdFALSE, timeout / portTICK_PERIOD_MS);	// Actually wait for the event
}

#endif	/* USE_FREERTOS */


//-- DHCP --//

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
	xTaskCreate(dhcp_task, "DHCP task", configMINIMAL_STACK_SIZE*4, NULL, uxTaskPriorityGet(NULL), NULL);	// Run the DHCP task	// TODO remove *4

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
	int i=0;

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

	for(i=0; i<MEMP_NUM_TCP_PCB; ++i) {	// default value for sockets IDs
		idOfevGrSock[i] = -1;
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

/**
 * getID
 *	returns the ID (in the evGrSock array) of the socket
 *
 *	@param socket: a valid socket identifier
 *
 *	@returns the ID in evGrSock array of socket
 */
int getID(int socket) {
	int i;
	for(i=0; i< MEMP_NUM_TCP_PCB; ++i) {	// Scan through the table
		if(idOfevGrSock[i] == socket)
			break;
	}

	if(i == MEMP_NUM_TCP_PCB) {	// Not found
		return -1;
	}
	return i;
}

int simpleSocket() {
	int socket = 0;
	int i=0;
	int keepAliveIdleTime = KEEPALIVE_IDLE, interval = KEEPALIVE_INTVL, count = KEEPALIVE_COUNT;
#if LWIP_SO_RCVTIMEO
	int recvTimeout = RECV_TIMEOUT_VAL;
#endif
#if LWIP_SO_SNDTIMEO
	int sendTimeout = SEND_TIMEOUT_VAL;
#endif

	xSemaphoreTake(createSema, portMAX_DELAY);
		for(i=0; i< MEMP_NUM_TCP_PCB; ++i) {	// Reserve a spot in the array
			if(idOfevGrSock[i] == -1)
				break;
		}

		if(i == MEMP_NUM_TCP_PCB) {
			printf("ERROR: Too many simultaneous sockets!\n");
			return -1;
		}

		socket = lwip_socket(AF_INET, SOCK_STREAM, 0);	// Create the socket
		idOfevGrSock[i] = socket;	// Stores it in the array to link it to his EventGroup
	xSemaphoreGive(createSema);

	if(socket != -1) {
		evGrSock[i] = xEventGroupCreate();
		xEventGroupClearBits(evGrSock[i], 0xFFFFFF);
		xEventGroupSetBits(evGrSock[i], EV_LWIP_SOCKET_DISCONNECTED);

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
	int keepAliveIdleTime = KEEPALIVE_IDLE, interval = KEEPALIVE_INTVL, count = KEEPALIVE_COUNT;
#if LWIP_SO_RCVTIMEO
	int recvTimeout = RECV_TIMEOUT_VAL;
#endif
#if LWIP_SO_SNDTIMEO
	int sendTimeout = SEND_TIMEOUT_VAL;
#endif
	int ret = 0;
	int i;
	int error;
	socklen_t optLen = sizeof(error);

	while((ret = lwip_accept(socket, NULL, NULL)) == -1) {
		lwip_getsockopt(socket, SOL_SOCKET, SO_ERROR, &error, &optLen);	// Retrieve the error

		if(error!= EWOULDBLOCK)	// Error!
			break;
		else {	// timeout: set the event
			if((i=getID(socket)) == -1)
				break;
			else
				xEventGroupSetBits(evGrSock[i], EV_LWIP_SOCKET_ACCEPT_TIMEOUT);
		}
	}

	// Initialize the socket's parameters
	if(ret != -1) {
		xSemaphoreTake(createSema, portMAX_DELAY);
			for(i=0; i< MEMP_NUM_TCP_PCB; ++i) {
				if(idOfevGrSock[i] == -1)
					break;
			}

			if(i == MEMP_NUM_TCP_PCB) {
				printf("ERROR: Too many simultaneous sockets!\n");
				lwip_close(ret);
				return -1;
			}

			idOfevGrSock[i] = ret;
		xSemaphoreGive(createSema);

		evGrSock[i] = xEventGroupCreate();
		xEventGroupClearBits(evGrSock[i], 0xFFFFFF);
		xEventGroupSetBits(evGrSock[i], EV_LWIP_SOCKET_CONNECTED);

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
	int i;
	ip.sin_addr.s_addr = *(params[1]);
	ip.sin_port = *(params[2]);
	ip.sin_family = AF_INET;
	ip.sin_len = sizeof(ip);

	*ret = lwip_connect(socket, (struct sockaddr*)&ip, ip.sin_len);
	if((i=getID(socket)) != -1)
		xEventGroupSetBits(evGrSock[i], EV_LWIP_SOCKET_CONNECTED_INTERN);	// The function returned.

	// The task sould be deleted now.
	while(1)
		vTaskDelay(100);
}

int simpleConnect(int socket, char* distantIP, int port) {
	int ret = 0, ret2 = 0;
	int id;
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

	id = getID(socket);

	if(id != -1) {

	xSemaphoreTake(connectSema, portMAX_DELAY);
	xTaskCreate(connectTask, "Connect Task", configMINIMAL_STACK_SIZE, (void*)params, uxTaskPriorityGet(NULL), &connectHandle);

		uxBits = xEventGroupWaitBits(evGrSock[id], EV_LWIP_SOCKET_CONNECTED_INTERN, pdTRUE, pdFALSE, CONNECT_TIMEOUT_VAL/portTICK_PERIOD_MS);
		xSemaphoreGive(connectSema);

		if((uxBits & EV_LWIP_SOCKET_CONNECTED_INTERN) != EV_LWIP_SOCKET_CONNECTED_INTERN) {	// Timeout
			xEventGroupSetBits(evGrSock[id], EV_LWIP_SOCKET_CONNECT_TIMEOUT | EV_LWIP_SOCKET_DISCONNECTED);
			xEventGroupClearBits(evGrSock[id], EV_LWIP_SOCKET_CONNECTED);
			ret2 = -1;
			//printf("Timeout of connect.\n");
		}
		else {
			xEventGroupClearBits(evGrSock[id], EV_LWIP_SOCKET_DISCONNECTED);
			xEventGroupSetBits(evGrSock[id], EV_LWIP_SOCKET_CONNECTED);
			ret2 = ret;
		}
		vTaskDelete(connectHandle);
	}
	else {
		//printf("Error: socket not found.\n");
		return -1;
	}

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

int simpleSend(int socket, void* data, int length) {
	int ret = 0;

	if(length <= 0)
		return -1;

	ret = lwip_send(socket, data, length, 0);

	return ret;
}

int simpleSendStr(int socket, char* data) {
	int length;

	for(length = 0; data[length] != '\0'; ++length);
	++length;

	//printf("str len = %d", length);
	return simpleSend(socket, (void*)data, length);
}

int simpleRecv(int socket, void* data, int maxLength) {
	int ret = 0;
	int error;
	int id;
	socklen_t optLen = sizeof(error);

	while((ret = lwip_recv(socket, data, maxLength, 0)) == -1) {
		vTracePrintF(xTraceOpenLabel("LwIP"), "lwip_recv timeout");
		lwip_getsockopt(socket, SOL_SOCKET, SO_ERROR, &error, &optLen);
		if(error!= EWOULDBLOCK) {
			break;
		}
		else {
			if((id = getID(socket)) != -1)
				xEventGroupSetBits(evGrSock[id], EV_LWIP_SOCKET_RECV_TIMEOUT);
		}
	}

	vTracePrintF(xTraceOpenLabel("SimpleSock"), "lwip_recv exit. ret = %d", ret);
	return ret;
}

int simpleClose(int socket) {
	int ret = 0;
	int i = 0;

	if((i=getID(socket))!= -1) {
		vEventGroupDelete(evGrSock[i]);
		idOfevGrSock[i] = -1;
	}
	else
		return -1;

	ret = lwip_close(socket);


	return ret;
}

int socket_wait_events(int socket, const int events, int timeout) {
	int i;
	i = getID(socket);
	if(i == -1)
		return -1;

	return xEventGroupWaitBits(evGrSock[i], events^EV_LWIP_SOCKET_CONNECTED_INTERN, pdFALSE, pdFALSE, timeout / portTICK_PERIOD_MS);
}

#endif	/* USE_SIMPLESOCKET && USE_FREERTOS */

char* getMyIP(void) {
	return myIP;
}

#endif	/* USE_LWIP */
