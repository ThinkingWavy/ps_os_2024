#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* transform(const char* input) {
	char* result = strdup(input);
	for (int i = 0; result[i] != '\0'; ++i) {
		if ((result[i] >= 'a' && result[i] <= 'z') || (result[i] >= 'A' && result[i] <= 'Z')) {
			char base = (result[i] >= 'a') ? 'a' : 'A';
			result[i] = (result[i] - base + 13) % 26 + base;
		}
	}
	return result;
}
