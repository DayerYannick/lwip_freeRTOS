/*
 * lwip.h
 *
 *  Created on: 24 mars 2015
 *      Author: yannick.dayer
 */

#ifndef LWIP_H_
#define LWIP_H_

#include "heivs/config.h"

#if USE_LWIP

// Load user configuration
//#include "config/lwipConfig.h"
// Initialize undefined options
#include "config/lwip_opt.h"

#if LWIP_DHCP
/**
 *	@brief initializes lwip with DHCP address assignment.
 *
 *	@param waitAddress: 0 when the function must not block
 *
 *	@return 0 if initialized, something else otherwise.
 */
int lwip_init_DHCP(const int waitAddress);

#endif	/* LWIP_DHCP */

/**
 *	@brief initializes lwip with a user defined address
 *
 *	@param ip: Our IP address							(e.g. "192.168.1.2")
 *	@param mask: The subnet mask						(e.g. "255.255.255.0")
 *	@param gateway: The default gateway of the network	(e.g. "192.168.1.1")
 *
 *	@return 0 if initialized, something else otherwise.
 */
int lwip_init_static(const char* ip, const char* mask, const char* gateway);

#if USE_FREERTOS
/*
 * FreeRTOS events definition for lwip
 *
 * Those events are called when the specified action happened.
 */
extern const int EV_LWIP_INITIALIZED;			/// Lwip is initialized
extern const int EV_LWIP_ETH_UP;				/// Ethernet connection could be established
extern const int EV_LWIP_ETH_DOWN;				/// Ethernet connection was lost (cable disconnected)
extern const int EV_LWIP_IP_ASSIGNED;			/// The device has an IP address
extern const int EV_LWIP_DHCP_FAILED;			/// The DHCP address assignment failed
extern const int EV_LWIP_ERROR;					/// Something went wrong

extern const int EV_LWIP_SOCKET_CONNECTED;		/// TCP socket is connected
extern const int EV_LWIP_SOCKET_DISCONNECTED;	/// TCP socket is disconnected
extern const int EV_LWIP_SOCKET_RECEIVED;		/// TCP socket received data
extern const int EV_LWIP_SOCKET_CONNECT_TIMEOUT;/// The socket could not connect before the timeout value
extern const int EV_LWIP_SOCKET_RECV_TIMEOUT;	/// The socket did not receive any data before the timeout value
extern const int EV_LWIP_SOCKET_SEND_TIMEOUT;	/// The socket did not send the data before the timeout value
extern const int EV_LWIP_SOCKET_ACCEPT_TIMEOUT;	/// The socket did not accept any connection before the timeout value

/**
 *	@brief Returns when an event concerning lwip comes.
 *
 *	@param event: the event(s) to wait for
 *	@param timeout: the amount of ms to wait before the function returns anyway
 *
 *	@return the actual value of the EventGroup bits	// FIXME: return something more intuitive
 */
int lwip_wait_events(const int event, int timeout);

#endif	/* USE_FREERTOS */


#if USE_SIMPLE_SOCKET && USE_FREERTOS

/**
 *	@brief Creates a socket
 *
 *	@return the socket identifier
 */
	int simpleSocket();

/**
 *	@brief bind a port to the given socket
 *
 *	@param socket: the socket identifier
 *	@param localIP: the address to bind to
 *	@param port: the port to bind to
 *
 *	@return -1 if error
 */
	int simpleBind(int socket, char* localIP, int port);

/**
 *	@brief open the port binded to the socket
 *
 *	@param socket: the socket identifier
 *
 *	@return -1 if error
 */
	int simpleListen(int socket);

/**
 *	@brief wait for an external connection
 *
 *	@param socket: the socket identifier
 *
 *	@return -1 if error
 */
	int simpleAccept(int socket);

/**
 *	@brief connect the socket to an IP
 *
 *	@param socket: the socket identifier
 *	@param distantIP: a string containing the IP in format "1.0.255.123"
 *	@param port: the TCP port to connect to
 *
 *	@return -1 if error
 */
	int simpleConnect(int socket, char* distantIP, int port);

/**
 *	@brief retrieve the ip address via DNS and connect to it
 *
 *	@param socket: the socket identifier
 *	@param name: an URL
 *	@param port: the TCP port to connect to
 *
 *	@return -1 if error
 */
	int simpleConnectDNS(int socket, char* name, int port);

/**
 *	@brief send length byte pointed by data
 *
 *	@param socket: the socket identifier
 *	@param data: the values to send
 *	@param length: the number of bytes to send
 *
 *	@return -1 if error
 */
	int simpleSend(int socket, const unsigned char* data, size_t length);

/**
 *	@brief send a string of char
 *
 *	@param socket: the socket identifier
 *	@param data: a null terminated string to send
 *
 *	@return -1 if error
 */
	int simpleSendStr(int socket, const char* data);

/**
 *	@brief returns when some data were received
 *
 *	@param socket: the socket identifier
 *	@param data: a pointer to store the data received
 *	@param maxLength: the maximal amount of bytes that can be stored in data
 *
 *	@return the length of data
 */
	int simpleRecv(int socket, unsigned char* data, size_t maxLength);

/**
 *	@brief end the connectoin and destroy the socket
 *
 *	@param socket: the socket identifier
 *
 *	@return -1 if error
 */
	int simpleClose(int socket);

#endif	// USE_SIMPLE_SOCKET && USE_FREERTOS

/**
 *	@brief return when the event given in parameter happen
 *
 *	@param socket: the socket identifier
 *	@param events: the event(s) to wait for
 *	@param timeout: the maximal amount of ms to wait before returning
 *
 *	@return the value of the EventGroup
 */
	int socket_wait_events(int socket, const int events, int timeout);

char* getMyIP(void);



#if USE_MBEDTLS && USE_FREERTOS

	int secureSocket();

	int secureBind(int socket, char* localIP, int port);

	int secureListen(int socket);

	int secureAccept(int socket);

	int secureConnect(int socket, char* distantIP, int port);

	int secureConnectDNS(int socket, char* name, int port);

	int secureSend(int socket, const unsigned char* data, size_t length);

	int secureSendStr(int socket, const char* data);

	int secureRecv(int socket, unsigned char* data, size_t maxLength);

	int secureClose(int socket);

#endif	// USE_MBEDTLS && USE_FREERTOS



#endif	/* USE_LWIP */

#endif	/* LWIP_H_ */
