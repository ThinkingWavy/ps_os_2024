#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* transform(const char* input) {
	char* result = strdup(input);
	for (int i = 0; result[i] != '\0'; ++i) {
		result[i] ^= 0x20;
	}
	return result;
}
