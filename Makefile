CC = gcc
CFLAGS = -Wall -Werror 

SERVER_OBJS = server.o
CLIENT_OBJS = client.o

OTHER_OBJS = ./src/sockets.o

OBJS = ./src/sockets.o \
	   server.o \
	   client.o

HEADERS = ./include/sockets.h

all: server.out client.out 

server.out: $(SERVER_OBJS) $(OTHER_OBJS) $(HEADERS) 
	@$(CC) $? -o $@

client.out: $(CLIENT_OBJS) $(OTHER_OBJS) $(HEADERS)
	@$(CC) $? -o $@

%.o: %.c
	@$(CC) -c $(CFLAGS) -o $@ $?

clean:
	@rm -f *.out

message:
	./server.out &
	./client.out

$(HEADERS):

.PHONY: clean 
