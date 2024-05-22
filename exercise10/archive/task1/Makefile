CFLAGS = -Wall -Wextra -std=c11 -O3
LDLIBS = -static

.PHONY:  all clean 

all: some_library.so my_program

some_library.o: some_library.c
	$(CC) $(CFLAGS) -fPIC -c -o $@ $^

some_library.so: some_library.o
	$(CC) -shared  -o $@ $^

my_program: my_program.c
	$(CC) $(CFLAGS) $(LDLIBS) -o $@ $^
clean:
	$(RM) *.o my_program 
