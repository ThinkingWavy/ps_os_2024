CC = gcc
CFLAGS = -Wextra -Wall -std=c11 -g 

.PHONY: clean all

all: calculator

calculator: calculator.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) *.o calculator