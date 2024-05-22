CFLAGS = -std=c11 -Wall -Wextra -g -ldl -Wpedantic
LFLAGS = -c -fPIC

.PHONY: all clean
all: map_string double.so remove_whitespace.so uppercase.so remove_A.so lowercase.so

clean:
	$(RM) map_string *.so *.o

map_string: map_string.c

double.so: double.o
	$(CC) -shared $^ -o $@

remove_whitespace.so: remove_whitespace.o
	$(CC) -shared $^ -o $@

uppercase.so: uppercase.o
	$(CC) -shared $^ -o $@

double.o: double.c
	$(CC) $(CFLAGS) $(LFLAGS) $^ -o $@

uppercase.o: uppercase.c
	$(CC) $(CFLAGS) $(LFLAGS) $^ -o $@

remove_whitespace.o: remove_whitespace.c
	$(CC) $(CFLAGS) $(LFLAGS) $^ -o $@

lowercase.o: lowercase.c
	$(CC) $(CFLAGS) $(LFLAGS) $^ -o $@

lowercase.so: lowercase.o
	$(CC) -shared $^ -o $@

remove_A.o: remove_A.c
	$(CC) $(CFLAGS) $(LFLAGS) $^ -o $@

remove_A.so: remove_A.o
	$(CC) -shared $^ -o $@
