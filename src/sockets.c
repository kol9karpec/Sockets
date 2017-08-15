#include "../include/sockets.h"

//return value 0/-1
int
TCP_server_start(const char * inaddr,
					unsigned short int port) {

	int socket_fd = socket(AF_INET,SOCK_STREAM,0);

	if(socket_fd == -1) {
		perror("socket() failed");
		return -1;
	}

	in_addr_t ip = inet_addr(inaddr);

	if(ip == (in_addr_t)(-1)) {
		perror("inet_addr() failed");
		return -1;
	}

	struct sockaddr_in server_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(port),
		.sin_addr.s_addr = ip
	}; //server_addr

	if(bind(socket_fd,
			(struct sockaddr *)(&server_addr),
			sizeof(server_addr))) {
		perror("bind() error");
		return -1;
	}

	if(listen(socket_fd,DEF_BACKLOG)) {
		perror("listen() error");
		return -1;
	}

	int client_fd[MAX_CLIENTS] = {0};
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = 0;

	signal(SIGINT,sigint_handler);
	pthread_mutex_init(&output_lock,NULL);

	while(clients_count < MAX_CLIENTS) {
		client_fd[clients_count] = accept(socket_fd,
						(struct sockaddr *)(&client_addr),
						&client_addr_len);

		if(client_fd[clients_count] < 0) {
			perror("Accept error!");
			return -1;
		}

		printf("New client: %s:%u\n",
				inet_ntoa(client_addr.sin_addr),
				ntohs(client_addr.sin_port));

		pthread_create(&client_threads[clients_count], //pthread * thread
						NULL, //const pthread_attr_t *attr
						client_handler, //void *(*start_routine) (void * arg)
						&client_fd[clients_count]); //void *arg

		clients_count++;
	}

	unsigned int i = 0;
	for(i=0;i<MAX_CLIENTS;i++) {
		pthread_join(client_threads[i],NULL);
	}

	return 0;
}

//return value - client's file descriptor
int
TCP_client_start(const char * server_ip_addr,
				unsigned short int server_port) {

	int socket_fd = socket(AF_INET,SOCK_STREAM,0);

	if(socket_fd == -1) {
		perror("socket() failed!");
		return -1;
	}

	struct sockaddr_in server_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(server_port),
		.sin_addr.s_addr = inet_addr(server_ip_addr)
	}; //server_addr

	if(connect(socket_fd,
				(struct sockaddr *)(&server_addr),
				sizeof(server_addr))) {
		perror("connect() failed");
		return -1;
	}

	char buffer[DEF_BUFSIZE] = {0};
	int n_bytes = 0;

	while(1) {
		scanf("%s",buffer);
		n_bytes = write(socket_fd,buffer,strlen(buffer)+1);
		if(n_bytes <= 0) {
			perror("write() failed");
			close(socket_fd);
			return -1;
		}
	}

	return 0;
}


void *
client_handler(void * arg) {
	int client_fd = *((int*)(arg));
	char buffer[DEF_BUFSIZE] = {0};
	int n_of_bytes = 0;

	while((n_of_bytes = read(client_fd,buffer,sizeof(buffer))) > 0) {
				bzero(buffer,sizeof(buffer));
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,NULL);
		//Handling buffer

		pthread_mutex_lock(&output_lock);
		printf("Submitted data: %s\n",buffer);
		pthread_mutex_unlock(&output_lock);

		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	}

	pthread_exit(NULL);
}

void
sigint_handler(int server_sock) {
	close(server_sock);

	unsigned int i = 0;
	for(i=0;i<clients_count;i++) {
		pthread_cancel(client_threads[i]);
	}

	exit(0);
}
