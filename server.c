#include "./include/tcp_sockets.h"

int main(int argc, char * argv[]) {
	return TCP_server_start(DEF_SERVER_IP, DEF_PORT);
}
