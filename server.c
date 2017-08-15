#include "./include/sockets.h"

int main(int argc, char * argv[]) {
	TCP_server_start("192.168.0.25",4500);

	return 0;
}
