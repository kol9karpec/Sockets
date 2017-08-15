CC = gcc
CFLAGS = -Wall -Werror 

SERVER_OBJS = server.o
CLIENT_OBJS = client.o

TEST_OBJS = ./src/sockets.o
TEST_HEADERS = ./headers/sockets.h

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

.PHONY: clean test_compile

test_compile: $(TEST_OBJS) $(TEST_HEADERS)

$(TEST_HEADERS):
