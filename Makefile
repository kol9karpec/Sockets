CC = gcc
CFLAGS = -Wall -Werror 

SERVER_OBJS = server.o
CLIENT_OBJS = client.o

all: server.out client.out

server.out: $(SERVER_OBJS) 
	@$(CC) $? -o $@

client.out: $(CLIENT_OBJS)
	@$(CC) $? -o $@

%.o: %.c
	@$(CC) -c $(CFLAGS) -o $@ $?

clean:
	@rm -f $(SERVER_OBJS) $(CLIENT_OBJS) app.out

message:
	./server.out &
	./client.out
