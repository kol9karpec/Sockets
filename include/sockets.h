#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h> //stuff functions to work with sockets
#include <sys/types.h>

#include <netinet/in.h> //sockaddr(_in), in_addr
#include <string.h> //strlen, bzero, memset
#include <unistd.h> //system calls
#include <assert.h> //maybe will be used

#define DEF_PORT 4500
#define DEF_BACKLOG 10
#define DEF_BUFSIZE 256

//return value 0/-1
int TCP_server_start(struct in_addr inaddr,
					unsigned short int port);

//return value - client's file descriptor
int TCP_client_start(const char * server_ip_addr,
				unsigned short int server_port);

void client_handler(void * argument,...);

int main(int argc, char * argv[]) {
	int socket_fd = socket(AF_INET,SOCK_STREAM,0); //syscall to create socket

	if(socket_fd == -1) {
		printf("Socket creating failed!\n");
		return -1;
	}

	struct sockaddr_in server_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(DEF_PORT),
		.sin_addr.s_addr = INADDR_ANY
	}; //server_addr

	if(connect(socket_fd,
				(struct sockaddr *)(&server_addr),
				sizeof(struct sockaddr))) {
		perror("Connecting error!");
		return -1;
	}

	const char message[] = "Message from client!";
	int n = 0;
	n = write(socket_fd,message,strlen(message)+1);

	printf("Number of bytes written: %d\n",n);

	close(socket_fd);


	return 0;
}
