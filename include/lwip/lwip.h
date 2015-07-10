/*
 * lwip.h
 *
 *	Offers simple methods to use TCP, UDP or TLS to connect to a server, or
 *	to accept clients connections.
 *
 *  Created on: 24 mars 2015
 *      Author: yannick.dayer
 */

#ifndef LWIP_H_
#define LWIP_H_

#include "heivs/config.h"

#if USE_LWIP

// Load user configuration
#include "config/lwip_opt.h"

#if LWIP_DHCP
/**
 *	@brief initializes lwip with DHCP address assignment.
 *
 *	@param waitAddress: 0 when the function must not block
 *	@param hostname: the desired hostname for the board. NULL for no hostname
 *
 *	@return 0 if initialized, something else otherwise.
 */
int lwip_init_DHCP(const int waitAddress, const char* hostname);

#endif	/* LWIP_DHCP */

/**
 *	@brief initializes lwip with a user defined address
 *
 *	@param ip: Our IP address							(e.g. "192.168.1.2")
 *	@param mask: The subnet mask						(e.g. "255.255.255.0")
 *	@param gateway: The default gateway of the network	(e.g. "192.168.1.1")
 *	@param hostname: the desired hostname for the board. NULL for no hostname
 *
 *	@return 0 if initialized, something else otherwise.
 */
int lwip_init_static(const char* ip, const char* mask, const char* gateway, const char* hostname);

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
extern const int EV_LWIP_DHCP_STARTED;			/// The DHCP address assignment failed
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
 *	@return the actual value of the EventGroup bits	// maybe: return something more intuitive
 */
int lwip_wait_events(const int event, int timeout);

#endif	/* USE_FREERTOS */

/**
 *	@brief Returns the board's current IP address
 *
 *	@return the IP address in "153.109.5.1" format
 */
char* getMyIP(void);


#if USE_SIMPLE_SOCKET

#include "lwip/lwip/sockets.h"

typedef enum {TCP = SOCK_STREAM, UDP = SOCK_DGRAM} protocole_t;


/**
 *	@brief Creates a TCP or UDP socket
 *
 *	@param proto: the protocole to use, UDP or TCP
 *
 *	@return the socket identifier
 */
	int simpleSocket(protocole_t proto);

/**
 *	@brief Binds a port to the given socket
 *
 *	@param socket: the socket identifier
 *	@param localIP: the address to bind to
 *	@param port: the port to bind to
 *
 *	@return -1 if error
 */
	int simpleBind(int socket, char* localIP, int port);

/**
 *	@brief Opens the port binded to the socket
 *
 *	@param socket: the socket identifier
 *
 *	@return -1 if error
 */
	int simpleListen(int socket);

/**
 *	@brief Waits for an external connection
 *
 *	@param socket: the socket identifier
 *
 *	@return -1 if error
 */
	int simpleAccept(int socket);

/**
 *	@brief Connects the socket to an IP
 *
 *	@param socket: the socket identifier
 *	@param distantIP: a string containing the IP in format "1.0.255.123"
 *	@param port: the TCP port to connect to
 *
 *	@return -1 if error
 */
	int simpleConnect(int socket, char* distantIP, int port);

/**
 *	@brief Retrieves the ip address via DNS and connect to it
 *
 *	@param socket: the socket identifier
 *	@param name: an URL
 *	@param port: the TCP port to connect to
 *
 *	@return -1 if error
 */
	int simpleConnectDNS(int socket, char* name, int port);

/**
 *	@brief Sends length byte pointed by data
 *
 *	@param socket: the socket identifier
 *	@param data: the values to send
 *	@param length: the number of bytes to send
 *
 *	@return -1 if error
 */
	int simpleSend(int socket, const unsigned char* data, size_t length);

/**
 *	@brief Sends a string of char
 *
 *	@param socket: the socket identifier
 *	@param data: a null terminated string to send
 *
 *	@return -1 if error
 */
	int simpleSendStr(int socket, const char* data);

/**
 *	@brief Returns when some data were received
 *
 *	@param socket: the socket identifier
 *	@param data: a pointer to store the data received
 *	@param maxLength: the maximal amount of bytes that can be stored in data
 *
 *	@return the length of data
 */
	int simpleRecv(int socket, unsigned char* data, size_t maxLength);

/**
 *	@brief Ends the connection and destroy the socket
 *
 *	@param socket: the socket identifier
 *
 *	@return -1 if error
 */
	int simpleClose(int socket);



/**
 *	@brief Returns when one of the events given in parameter happen
 *
 *	@param socket: the socket identifier
 *	@param events: the event(s) to wait for
 *	@param timeout: the maximal amount of ms to wait before returning
 *
 *	@return the value of the EventGroup
 */
	int socket_wait_events(int socket, const uint32_t events, int timeout);

/**
 *	@brief Returns the current events state
 *
 *	@param socket: the socket identifier
 *	@param events: a mask for the wished events
 *
 *	@return the value of the event. -1 if error
 */
	int socket_get_events(int socket, uint32_t events);

/**
 *	@brief Returns the number of created sockets
 *
 *	@return the number of sockets
 */
	int getSocketNb(void);

/**
 *	@brief Returns the maximum number of simultaneously active socket
 *
 *	@return the maximum number of sockets
 */
	int getSocketNbMax(void);

/**
 *	@brief Returns the state of the socket
 *
 *	@param: socket: the socket identifier
 *
 *	@return 1 if the socket is valid (created), 0 if the socket is closed.
 */
	uint8_t socketValid(int socket);


#endif	// USE_SIMPLE_SOCKET


#if USE_MBEDTLS


/**
 *	@brief Creates a TLS socket
 *
 *	@return the new socket. -1 if error
 */
	int secureSocket();

/**
 *	@brief Binds a TCP port to a socket
 *
 *	@param socket: the socket identifier
 *	@param localIP: the ip address to bind to
 *	@param port: the TCP port to bind to
 *
 *	@return -1 if error
 */
	int secureBind(int socket, char* localIP, int port);

/**
 *	@brief Sets a TCP port to listen mode
 *
 *	@param socket: the socket identifier
 *
 *	@return -1 if error
 */
	int secureListen(int socket);

/**
 *	@brief Waits for a connection request and creates a new secureSocket
 *
 *	@param socket: the socket identifier
 *
 *	@return the newly created socket. -1 if error
 */
	int secureAccept(int socket);

/**
 *	@brief Connects to a TLS server. Verifies the certificate validity
 *
 *	@param socket: the socket identifier
 *	@param distantIP: the ip of the server
 *	@param port: the destination TCP port
 *
 *	@return -1 if error
 */
	int secureConnect(int socket, char* distantIP, int port);

/**
 *	@brief Connects to a TLS server by its dns address
 *
 *	@param socket: the socket identifier
 *	@param name: the dns address
 *	@param port: the TCP port to use
 *
 *	@return -1 if error
 */
	int secureConnectDNS(int socket, char* name, int port);

/**
 *	@brief Sends data over TLS to the host
 *
 *	@param socket: the socket identifier
 *	@param data: length bytes of data
 *	@param length: the size of the data
 *
 *	@return -1 if error
 */
	int secureSend(int socket, const unsigned char* data, size_t length);

/**
 *	@brief Sends a string of data over TLS to the host
 *
 *	@param socket: the socket identifier
 *	@param data: a null-terminated string to send
 *
 *	@return -1 if error
 */
	int secureSendStr(int socket, const char* data);

/**
 *	@brief Waits some data to come from the host
 *
 *	@param socket: the socket identifier
 *	@param data: memory place to store received data
 *	@param maxLength: the size of the free space at data
 *
 *	@return the number of bytes received. -1 if error
 */
	int secureRecv(int socket, unsigned char* data, size_t maxLength);

/**
 *	@brief Ends the connection and destroys the socket
 *
 *	@param socket: the socket identifier
 *
 *	@return -1 if error
 */
	int secureClose(int socket);

#endif	// USE_MBEDTLS



#endif	/* USE_LWIP */

#endif	/* LWIP_H_ */
