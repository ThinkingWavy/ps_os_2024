#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*################################
    Upper case (not reverible)
#################################*/
char* transform(const char* input) {
	char* result = strdup(input);
	for (int i = 0; result[i] != '\0'; ++i) {
		result[i] = toupper(result[i]);
	}
	return result;
}