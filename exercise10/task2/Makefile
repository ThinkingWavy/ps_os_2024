CFLAGS = -Wall -Wextra -std=c11 -O3
LDLIBS = -static

.PHONY:  all clean 

all: libsom_library.so my_program

libsome_library.o: some_library.c
	$(CC) $(CFLAGS) -fPIC -c -o $@ $^

libsome_library.so: libsome_library.o
	$(CC) -shared  -o $@ $^

my_program: my_program.c
	$(CC) $(CFLAGS) $(LDLIBS) -o $@ $^

clean:
	$(RM) *.o my_program *.so 

#Extend your Makefile to compile this program as a static executable
#(again, have a look at the linking options and find out how this can be done with GCC).
#Compare the sizes of some_program and my_program — what do you find?
#Verify that your executable is actually static. Use ldd and file for t
#NOTE: NOT POSSIBLE on ZID