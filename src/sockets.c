#include "../include/sockets.h"

//return value 0/-1
int TCP_server_start(struct in_addr inaddr,
					unsigned short int port) {
	//TODO: Implement function
	//TODO: Implement threads
}

//return value - client's file descriptor
int TCP_client_start(const char * server_ip_addr,
				unsigned short int server_port) {
	//TODO: Implement function
}


void client_handler(void * argument,...) {
	//TODO: Implement function
	//This function is for handling 
	//	clients in different threads.
}
