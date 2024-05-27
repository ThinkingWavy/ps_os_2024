CFLAGS = -Wall -Wextra -std=c11 -O3 -lrt -D_BSD_SOURCE -g -pthread -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE -D_DARWIN_C_SOURCE

.PHONY: all clean
all: client server

clean:
	$(RM) client server

client: client.c

server: server.c

