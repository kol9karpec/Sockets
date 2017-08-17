#ifndef __UDP_SOCKETS_H__
#define __UDP_SOCKETS_H__

#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h> //stuff functions to work with sockets
#include <sys/types.h>

#include <netinet/in.h> //sockaddr(_in), in_addr
#include <string.h> //strlen, bzero, memset
#include <unistd.h> //system calls
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h> //inet_addr()

#define DEF_PORT 4500
#define DEF_BACKLOG 10
#define DEF_BUFSIZE 256
#define MAX_CLIENTS 5

int UDP_server_start(const char * inaddr,
					unsigned short int port);

int UDP_client_start(const char * server_ip_addr,
				unsigned short int server_port);

#endif //__UDP_SOCKETS_H__
