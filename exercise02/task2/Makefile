CC = gcc
CFLAGS = -Wextra -Wall -std=c11

.PHONY: clean all

signal_handler: signal_handler.c
	$(CC) $(CFLAGS)  $^ -o $@

all: signal_handler

clean:
	$(RM) *.o signal_handler