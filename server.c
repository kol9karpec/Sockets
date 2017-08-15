#include "./include/sockets.h"

int main(int argc, char * argv[]) {
	TCP_server_start("0.0.0.0",4500);

	return 0;
}
