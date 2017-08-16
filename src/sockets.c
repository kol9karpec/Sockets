#include "../include/sockets.h"

pthread_t client_threads[MAX_CLIENTS] = {0};

pthread_mutex_t output_lock = PTHREAD_MUTEX_INITIALIZER;

volatile unsigned char clients_count = 0;

static int client_fd[MAX_CLIENTS];

static void set_def_client_fd();

static int get_free_client_slot();

static pthread_t clients_checker_thread = 0;
static void* clients_checker(void * arg);

static int join_client(int slot_number, int client_socket_fd);

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

	unsigned int i = 0;

	for(i=0;i<MAX_CLIENTS;i++) {
		client_fd[i] = -1;
	}

	signal(SIGINT,sigint_handler);
	pthread_mutex_init(&output_lock,NULL);

	set_def_client_fd();
	pthread_create(&clients_checker_thread,
					NULL,
					clients_checker,
					NULL);

	while(1) {
		int slot_number = get_free_client_slot();

		if(slot_number >= 0)
			join_client(slot_number,socket_fd);
	}

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
	//memset(buffer,'\0',sizeof(buffer));
	int n_bytes = 0;

	while(1) {
		scanf("%s",buffer);
		//fgets(buffer,sizeof(buffer),stdin);
		n_bytes = write(socket_fd,buffer,sizeof(buffer));
		if(n_bytes <= 0) {
			perror("write() failed");
			close(socket_fd);
			return -1;
		}
		if(!strcmp(buffer,"close")) {
			break;
		}
	}

	return 0;
}


void *
client_handler(void * arg) {
	int client_fd = *((int*)(arg));
	char buffer[DEF_BUFSIZE] = {0};
	//memset(buffer,'\0',sizeof(buffer));
	int n_of_bytes = 0;

	while((n_of_bytes = read(client_fd,buffer,sizeof(buffer))) > 0) {
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,NULL);
		//Handling buffer

		pthread_mutex_lock(&output_lock);
		printf("Submitted data: %s\n",buffer);
		pthread_mutex_unlock(&output_lock);

		if(!strcmp(buffer,"close")) {
			break;
		}

		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
		memset(buffer,'\0',sizeof(buffer));
	}

	close(client_fd);
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

static void set_def_client_fd() {
	unsigned int i = 0;
	for(;i<MAX_CLIENTS;i++)
		client_fd[i] = -1;
}


int get_free_client_slot() {
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

static int join_client(int slot_number, int client_socket_fd) {
	struct sockaddr_in  client_addr;
	socklen_t client_addr_len = 0;

	client_fd[slot_number] = accept(client_socket_fd,
			(struct sockaddr *)(&client_addr),
			&client_addr_len);

	if(client_fd[slot_number] < 0) {
		perror("Accept error!");
		return -1;
	}

	printf("New client: %s:%u\n",
			inet_ntoa(client_addr.sin_addr),
			ntohs(client_addr.sin_port));

	pthread_create(&client_threads[slot_number], //pthread * thread
			NULL, //const pthread_attr_t *attr
			client_handler, //void *(*start_routine) (void * arg)
			&client_fd[slot_number]); //void *arg

	clients_count++;

	return 0;
}

static void* clients_checker(void * arg) {
	unsigned int i = 0;
	while(1) {
		for(i=0;i<MAX_CLIENTS;i++) {
			if((client_fd[i] != -1) && \
					!pthread_tryjoin_np(client_threads[i],NULL)) {
				printf("Client %d unjoined!\n",i);
				client_fd[i] = -1;
			}
		}

		//printf("checker works!\n");
		sleep(1);
	}

	return NULL;
}
