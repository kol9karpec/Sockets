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

	struct sockaddr_in server_addr = {
		.sin_family = AF_INET,
		.sin_port = DEF_PORT,
		.sin_addr.s_addr = INADDR_ANY
	}; //server_addr

	if(connect(socket_fd,
				(struct sockaddr *)(&server_addr),
				sizeof(struct sockaddr))) {
		perror("Connecting error!");
		return -1;
	}

	const char message[] = "Message from client!";
	write(socket_fd,message,strlen(message));

	close(socket_fd);


	return 0;
}
