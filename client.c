#include "./include/tcp_sockets.h"

int main(int argc, char * argv[]) {
	return TCP_client_start("127.0.0.1",4500);
}
