CC = gcc
CFLAGS = -Wall -Werror -pthread -D_GNU_SOURCE

SERVER_OBJS = server.o
CLIENT_OBJS = client.o
COMMON_OBJS = ./src/tcp_sockets.o

UDP_SERVER_OBJS = udp_server.o
UDP_CLIENT_OBJS = udp_client.o

UDP_OBJS = ./src/udp_sockets.o
UDP_HEADERS = ./include/udp_sockets.h

HEADERS = ./include/tcp_sockets.h
LDFLAGS = -pthread

udp: udp_server.out udp_client.out 
tcp: server.out client.out

server.out: $(COMMON_OBJS) $(HEADERS) $(SERVER_OBJS) 
client.out: $(COMMON_OBJS) $(HEADERS) $(CLIENT_OBJS) 

server.out client.out:
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@ 


udp_server.out: $(UDP_OBJS) $(UDP_HEADERS) $(UDP_SERVER_OBJS)
udp_client.out: $(UDP_OBJS) $(UDP_HEADERS) $(UDP_CLIENT_OBJS)

udp_server.out udp_client.out:
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@ 

clean:
	@rm -f *.out $(SERVER_OBJS) \
				$(CLIENT_OBJS) \
				$(COMMON_OBJS) \
				$(UDP_SERVER_OBJS) \
				$(UDP_CLIENT_OBJS) \
				$(UDP_OBJS)

message:
	./server.out &
	./client.out 

$(HEADERS):

udp_test_compile: $(UDP_OBJS) $(UDP_HEADERS)

.PHONY: clean message udp_test_compile udp tcp
