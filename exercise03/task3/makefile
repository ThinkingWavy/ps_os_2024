CFLAGS = -Wall -Wextra -std=c11 -g -pthread -O3 -D_POSIX_C_SOURCE -D_BSD_SOURCE
LDFLAGS = -lpthread


.PHONY: debug all clean

all: task3a task3b
debug: CFLAGS += -DDEBUG
debug: task3a task3b

task3a: task3a.o
	$(CC) $(LDFLAGS) $(CFLAGS) $(LDLIBS) -o $@ $^ 
task3b: task3b.o 
	$(CC) $(LDFLAGS) $(CFLAGS) $(LDLIBS) -o $@ $^
%.o: %.c
	$(CC) $(CFLAGS) $^ -g -c -o $@

clean:
	$(RM) *.o task3a task3b