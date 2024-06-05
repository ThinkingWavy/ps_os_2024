CFLAGS = -Wall -Wextra -std=c11 -O3 -fPIC -g

.PHONY: all clean plugins

PLUGINS = caesar_cipher xor_string reverse_string library_one library_two

# Target to build all plugins (uses pattern rule)
all: $(addsuffix .so, $(PLUGINS)) map_string

# Target to build only plugins (uses pattern rule)
plugins: $(addsuffix .so, $(PLUGINS))

# Pattern rule to build .o files from .c files
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^  # Compile source to object file

# Pattern rule to build shared libraries from .o files
%.so: %.o
	$(CC) $(CFLAGS) -shared -o $@ $^  # Link object files to shared library

# Build map_string program
map_string: map_string.c 
	$(CC) $(CFLAGS) -o $@ $^ 

# Clean target to remove object and shared library files
clean:
	$(RM) *.o *.so map_string 

# Include missing main function for linking
map_string.c:  # (or add your main function file here)
