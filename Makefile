CC = gcc
CFLAGS = --std=c99 -Wall -Werror -pedantic

OBJS = server.o

all: app.out

app.out: $(OBJS) 
	@$(CC) $? -o $@

%.o: %.c
	@$(CC) -c $(CFLAGS) -o $@ $?

clean:
	@rm -f $(OBJS) app.out
