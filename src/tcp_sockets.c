#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> //sockaddr(_in), in_addr
#include <arpa/inet.h> //inet_addr()

#include "../include/tcp_sockets.h"

static pthread_t client_threads[MAX_CLIENTS] = { 0 };
static pthread_mutex_t output_lock = PTHREAD_MUTEX_INITIALIZER;
static unsigned char clients_count = 0;
static int client_fd[MAX_CLIENTS];

static int get_free_client_slot();

static pthread_t clients_checker_thread = 0;
static void* clients_checker(void * arg);
static int join_client(int server_socket_fd);
static char * get_client_ip(int socket_fd);

static void* client_thread_handler(void * arg);
static void sigint_handler(int server_sock);

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

	signal(SIGINT,sigint_handler);
	pthread_mutex_init(&output_lock,NULL);

	memset(client_fd, -1, sizeof(client_fd));
	/* TODO: no result checking */
	pthread_create(&clients_checker_thread,
					NULL,
					clients_checker,
					NULL);

	while(1) {
		join_client(socket_fd);
	}

	/* unreachable */
	return 0;
}

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
	int n_bytes = read(socket_fd,buffer,sizeof(buffer));
	if(!strcmp(buffer,"busy")) { //TODO: strncmp
		printf("Server is busy!\n");
		return -1;
	} else {
		while(1) {
			/* TODO: get n_bytes*/ scanf("%s",buffer); //TODO: buffer overflow
			n_bytes = write(socket_fd,buffer,sizeof(buffer));
			if(n_bytes <= 0) {
				perror("write() failed");
				close(socket_fd);
				return -1;
			}
			if(!strcmp(buffer,"close")) { //TODO: strncmp
				break;
			}
		}
	}

	return 0;
}


void *
client_thread_handler(void * arg) {
	int client_fd = *((int*)(arg));
	char buffer[DEF_BUFSIZE] = {0};
	int n_of_bytes = 0;

	while((n_of_bytes = read(client_fd,buffer,sizeof(buffer))) > 0) {
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,NULL);

		pthread_mutex_lock(&output_lock);
		printf("Submitted data: %s\n",buffer); //TODO: non-safe for NUL termination
		pthread_mutex_unlock(&output_lock);

		if(!strcmp(buffer,"close")) { //TODO: non-safe
			break;
		}

		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
		memset(buffer,0,sizeof(buffer)); //TODO: not necessary
	}

	close(client_fd);
	pthread_exit(NULL);
}

void
sigint_handler(int server_sock) {
	close(server_sock);

	unsigned int i = 0;
	for(i=0;i<MAX_CLIENTS;i++) {
		if(client_fd[i] != -1)
			pthread_cancel(client_threads[i]);
	}

	exit(0);
}

//TODO: thread safe
int
get_free_client_slot() {
	int result = -1;
	int i = 0;

	for(;i<MAX_CLIENTS;i++) {
		if(client_fd[i] == -1) {
			result = i;
			break;
		}
	}

	return result;
}

static int
join_client(int server_socket_fd) {
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);

	int cur_client_fd = 0;

	cur_client_fd = accept(server_socket_fd,
			(struct sockaddr *)(&client_addr),
			&client_addr_len);

	if(cur_client_fd < 0) {
		perror("Accept error!");
		return -1;
	}

	if(clients_count == MAX_CLIENTS) {
		write(cur_client_fd,"busy",DEF_BUFSIZE);
		close(cur_client_fd);
		//printf("Server is busy!\n");
	} else {
		int slot_number = get_free_client_slot();

		write(cur_client_fd,"ok",DEF_BUFSIZE);

		client_fd[slot_number] = cur_client_fd; //TODO: buffer underflow
		/*printf("New client: %s:%u\n",
				inet_ntoa(client_addr.sin_addr),
				ntohs(client_addr.sin_port));
				*/
		printf("New client: %s\n",
				get_client_ip(cur_client_fd));
		/* TODO: unclosed socket, if pthread haven't started before */
		pthread_create(&client_threads[slot_number],
				NULL, /**< pthread attr */
				client_thread_handler,
				&client_fd[slot_number]); //TODO: buffer underflow

		clients_count++; //TODO: sync
	}

	return 0;
}

static void*
clients_checker(void * arg) {
	unsigned int i = 0;
	while(1) {
		for(i=0;i<MAX_CLIENTS;i++) {
			if((client_fd[i] != -1) && \
					!pthread_tryjoin_np(client_threads[i],NULL)) {
				printf("Client %d unjoined!\n",i);
				client_fd[i] = -1; //TODO: sync
				clients_count--; //TODO: sync
			}
		}

		//printf("checker works!\n");
		sleep(1);
	}

	return NULL;
}

/* Non-thread safe */
static char * get_client_ip(int socket_fd) {
	char * result = (char*)(malloc(sizeof(char)*DEF_BUFSIZE)); /* TODO: memory leak */
	/* TODO: check result for not NULL */
	struct sockaddr_in addr;
	socklen_t addr_size = sizeof(struct sockaddr_in);
	getpeername(socket_fd, (struct sockaddr *)&addr, &addr_size);
	/* TODO: result handling, non-empty addr, so garbage below */
	snprintf(result,DEF_BUFSIZE,"%s:%d",
			inet_ntoa(addr.sin_addr),
			ntohs(addr.sin_port));

	return result;
}
