CC = gcc
CFLAGS = -Wextra -Wall -std=c11

.PHONY: clean all create_main

all: main

main: main.o hello.o goodbye.o
	@echo "Linking object files:"
	$(CC) -o $@ $^

main.o: create_main
	@echo "Compiling $<:"
	$(CC) $(CFLAGS) -c main.c -o $@

hello.o: hello.c
	@echo "Compiling hello:"
	$(CC) $(CFLAGS) -c $^ -o $@

goodbye.o: goodbye.c
	@echo "Compiling goodbye:"
	$(CC) $(CFLAGS) -c $^ -o $@	

create_main:
	@echo '#include "hello.h"' > main.c
	@echo '#include "goodbye.h"' >> main.c
	@echo '#include <stdlib.h>' >> main.c
	@echo '' >> main.c
	@echo 'int main(void) {' >> main.c
	@echo '    hello();' >> main.c
	@echo '    goodbye();' >> main.c
	@echo '    return EXIT_SUCCESS;' >> main.c
	@echo '}' >> main.c


clean:
	$(RM) *.o main.c main