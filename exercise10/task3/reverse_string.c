#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* transform(const char* input) {
	int len = strlen(input);
	char* result = (char*)malloc(len + 1);
	for (int i = 0; i < len; ++i) {
		result[i] = input[len - 1 - i];
	}
	result[len] = '\0';
	return result;
}
