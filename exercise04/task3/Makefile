CFLAGS = -std=c11 -Wall -Wextra -g

.PHONY: all clean
all: server client

clean:
	$(RM) server client

server: server.c common.h

client: client.c common.h

