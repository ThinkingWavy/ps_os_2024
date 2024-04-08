CC = gcc
CFLAGS = -Wextra -Wall -std=c11

.PHONY: clean all

all: task3

task3: task3.c
	$(CC) $(CFLAGS) -D_POSIX_C_SOURCE=199309L  $^ -o $@


clean:
	$(RM) *.o task3