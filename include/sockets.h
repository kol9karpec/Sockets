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

pthread_t client_threads[MAX_CLIENTS] = {0};

volatile unsigned char clients_count = 0;

//return value 0/-1
int TCP_server_start(const char * inaddr,
					unsigned short int port);

//return value - client's file descriptor
int TCP_client_start(const char * server_ip_addr,
				unsigned short int server_port);

void* client_handler(void * arg);

void sigint_handler(int server_sock);
