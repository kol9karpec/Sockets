#include "./include/udp_sockets.h"

int main(void) {
	UDP_server_start("0.0.0.0",4060);

	return 0;
}

