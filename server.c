#include "./include/tcp_sockets.h"

int main(int argc, char * argv[]) {
	return TCP_server_start("0.0.0.0",4500);
}
