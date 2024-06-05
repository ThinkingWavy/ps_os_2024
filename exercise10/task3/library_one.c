#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
################################
Substitution (like in enigma)
################################
*/

// If loaded 26 times it will display the original message
static const char substitution_table[26] = { 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O',
	                                           'P', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K',
	                                           'L', 'Z', 'X', 'C', 'V', 'B', 'N', 'M' };

static char substitute_char(char c) {
	if (c >= 'a' && c <= 'z') {
		return substitution_table[c - 'a'] + ('a' - 'A'); // Ensure lowercase output
	} else if (c >= 'A' && c <= 'Z') {
		return substitution_table[c - 'A'];
	} else {
		return c; // Non-alphabet characters remain unchanged
	}
}

char* transform(const char* input) {
	char* result = strdup(input);
	for (int i = 0; result[i] != '\0'; ++i) {
		result[i] = substitute_char(result[i]);
	}
	return result;
}
