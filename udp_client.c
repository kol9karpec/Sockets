#include "./include/udp_sockets.h"

int main(void) {
	UDP_client_start("127.0.0.1",4060);

	return 0;
}
