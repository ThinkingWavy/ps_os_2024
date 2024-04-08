CC = gcc
CFLAGS = -Wextra -Wall -std=c11 -pthread 

.PHONY: clean all

task1_mutex: task1_mutex.c
	$(CC) $(CFLAGS) $^ -o $@

all: task1_mutex

task1_atmoic: task1_atomic.c
	$(CC) $(CFLAGS) $^ -o $@

all: task1_mutex task1_atomic

clean:
	$(RM) *.o task1_mutex task1_atmoic