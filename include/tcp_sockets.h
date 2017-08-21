#ifndef __TCP_SOCKETS_H__
#define __TCP_SOCKETS_H__

#include <pthread.h>

#define DEF_SERVER_IP "0.0.0.0"
#define DEF_PORT 4500
#define DEF_BACKLOG 10
#define DEF_BUFSIZE 256
#define MAX_CLIENTS 5

/* Synchronous launch of TCP server.
 * Overrides SIGINT.
 *
 * @arg inaddr IPv4 address for the server to bind
 * @port TCP port to use
 * @return 0 if success, < 0 otherwise
 */
int TCP_server_start(const char * inaddr, unsigned short port);

/* Synchronous launch of TCP client.
 * Overrides SIGINT.
 *
 * @arg inaddr IPv4 address for the server to connect
 * @port TCP port to use
 * @return 0 if success, < 0 otherwise
 */
int TCP_client_start(const char * server_ip_addr, unsigned short server_port);

#endif //__TCP_SOCKETS_H__
