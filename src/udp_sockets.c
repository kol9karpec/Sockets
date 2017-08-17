#include "../include/udp_sockets.h"

/*pthread_t client_threads[MAX_CLIENTS] = {0};

pthread_mutex_t output_lock = PTHREAD_MUTEX_INITIALIZER;

volatile unsigned char clients_count = 0;

static int client_fd[MAX_CLIENTS];

static void set_def_client_fd();

static int get_free_client_slot();

static pthread_t clients_checker_thread = 0;
static void* clients_checker(void * arg);


static char * get_client_ip(int socket_fd);
*/

static int receive_data(int server_socket_fd);

int
UDP_server_start(const char * inaddr,
					unsigned short int port) {

	int socket_fd = socket(AF_INET,SOCK_DGRAM,0);

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

	while(1) {
		receive_data(socket_fd);
	}

	return 0;
}

int
UDP_client_start(const char * server_ip_addr,
				unsigned short int server_port) {

	int socket_fd = socket(AF_INET,SOCK_DGRAM,0);

	if(socket_fd == -1) {
		perror("socket() failed!");
		return -1;
	}

	struct sockaddr_in server_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(server_port),
		.sin_addr.s_addr = inet_addr(server_ip_addr)
	}; //server_addr

	char buffer[DEF_BUFSIZE] = {0};
	int n_bytes = 0;

	while(1) {
		scanf("%s",buffer);

		n_bytes = sendto(socket_fd,
						buffer,
						sizeof(buffer),
						0,
						&server_addr,
						sizeof(server_addr));

		if(n_bytes <= 0) {
			perror("sendto() failed");
			close(socket_fd);
			return -1;
		}

		if(!strcmp(buffer,"close")) {
			break;
		}
	}

	return 0;
}


static int receive_data(int server_socket_fd) {
	struct sockaddr_storage client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	memset(&client_addr,'\0',sizeof(client_addr));

	char buffer[DEF_BUFSIZE] = {0};
	char remote_ip[DEF_BUFSIZE] = {0};

	int n_bytes = recvfrom(server_socket_fd,
							buffer,
							sizeof(buffer),
							0,
							(struct sockaddr *)&client_addr,
							&client_addr_len);

	if(n_bytes <= 0) {
		perror("recvfrom() failed");
		return -1;
	}

	printf("From: %s\n",inet_ntop(client_addr.ss_family,
								&(((struct sockaddr_in *)&client_addr)->sin_addr),
								remote_ip,
								sizeof(remote_ip)));
	printf("Message: %s\n",buffer);
	
	return 0;
}
