CFLAGS = -std=c11 -Wall -Wextra -g -pthread -pthread -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE -D_DARWIN_C_SOURCE

.PHONY: all clean
all: server client

clean:
	$(RM) server client

server: server.c 

client: client.c 

