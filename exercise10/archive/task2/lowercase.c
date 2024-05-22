#include <ctype.h>

void string_manipulation(char *input) {
    char *s = input;
    while (*s) {
        *s = tolower(*s);
        s++;
    }
}