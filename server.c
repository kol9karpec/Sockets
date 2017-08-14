#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define DEF_PORT 4500
#define DEF_BACKLOG 10
#define DEF_BUFSIZE 256

int main(int argc, char * argv[]) {
	int socket_fd = socket(AF_INET,SOCK_STREAM,0); //syscall to create socket

	if(socket_fd == -1) {
		printf("Socket creating failed!\n");
		return -1;
	}

	//This is server, so we need to bind port to our program
	//For binding we are need to create struct sockaddr_in

	struct sockaddr_in server_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(DEF_PORT),
		.sin_addr.s_addr = INADDR_ANY,
	}; //server_addr

	if(bind(socket_fd,
			(struct sockaddr *)(&server_addr),
			sizeof(server_addr))) {
		perror("Binding error!\n");
		return -1;
	}

	if(listen(socket_fd,DEF_BACKLOG)) {
		perror("Listening error!\n");
		return -1;
	}

	struct sockaddr_in client;
	socklen_t addrlen = sizeof(client);
	int client_fd = 0;

	client_fd = accept(socket_fd,(struct sockaddr *)&client,&addrlen);

	if(client_fd < 0) {
		perror("Accept error!");
		return -1;
	}

	printf("Client connected!\n");

	unsigned char buffer[DEF_BUFSIZE];
	memset(&buffer,'\0',DEF_BUFSIZE);

	int n = read(client_fd,buffer,DEF_BUFSIZE-1);

	printf("Number of bytes read: %d\n",n);

	printf("Server has got a message: %s\n",buffer);

	close(socket_fd);

	return 0;
}
