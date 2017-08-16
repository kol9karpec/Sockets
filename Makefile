CC = gcc
CFLAGS = -Wall -Werror -pthread -D_GNU_SOURCE

SERVER_OBJS = server.o
CLIENT_OBJS = client.o
COMMON_OBJS = ./src/tcp_sockets.o

HEADERS = ./include/tcp_sockets.h
LDFLAGS = -pthread

all: server.out client.out 

server.out: $(COMMON_OBJS) $(HEADERS) $(SERVER_OBJS) 
client.out: $(COMMON_OBJS) $(HEADERS) $(CLIENT_OBJS) 

server.out client.out:
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@ 

clean:
	@rm -f *.out $(SERVER_OBJS) $(CLIENT_OBJS) $(COMMON_OBJS)

message:
	./server.out &
	./client.out 

$(HEADERS):

.PHONY: clean all message
