CFLAGS = -Wall -Wextra -std=c11 -O3 -lrt -D_DEFAULT_SOURCE -g

.PHONY:  all clean

all: task2

task2: task2.o
	$(CC) $(LDFLAGS) $(CFLAGS) $(LDLIBS) -o $@ $^ 
%.o: %.c
	$(CC) $(CFLAGS) $^ -g -c -o $@

clean:
	$(RM) *.o task2 