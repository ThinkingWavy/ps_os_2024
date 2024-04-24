#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

void* string_shift_right(void* arg) {
	char* input = (char*)arg;
	size_t input_length = strlen(input);
	// BUG: try to return function local stack variable
	// char shifted_input[input_length];
	char* shifted_input = malloc((input_length + 1) * sizeof(char));
	for (size_t char_index = 0; char_index < input_length; ++char_index) {
		size_t new_position = (char_index + 2) % input_length;
		shifted_input[new_position] = input[char_index];
	}
	for (size_t char_index = 0; char_index < input_length; ++char_index) {
		// BUG: wrong string gets manipulated
		// shifted_input[char_index] = toupper(shifted_input[char_index]);
		input[char_index] = toupper(input[char_index]);
	}
	// BUG: Missing null terminator
	shifted_input[input_length] = '\0';
	// BUG: not correct to use return here
	return shifted_input;
}
int main(int argc, const char** argv) {
	// BUG: array must be determined on runtime
	// char* shared_strings[argc];
	char** shared_strings = malloc(sizeof(char*) * argc);

	for (int arg_index = 0; arg_index < argc; ++arg_index) {
		// BUG: strlen excludes null terminator therefore must be +1
		// size_t arg_length = strlen(argv[arg_index]);
		size_t arg_length = strlen(argv[arg_index]) + 1;

		shared_strings[arg_index] = malloc(arg_length * sizeof(char));
		if (shared_strings[arg_index] == NULL) {
			fprintf(stderr, "Could not allocate memory.\n");
			exit(EXIT_FAILURE);
		}
		strcpy(shared_strings[arg_index], argv[arg_index]);
	}

	pthread_t t_ids[argc];
	// BUG: array index starts at 0 therefore must be < not <=
	//  for (int arg_index = 0; arg_index <= argc; arg_index++) {
	for (int arg_index = 0; arg_index < argc; arg_index++) {
		pthread_create(&t_ids[arg_index], NULL, string_shift_right, shared_strings[arg_index]);

		// BUG: free happens to early
		// free(shared_strings[arg_index]);
	}

	// BUG: array index starts at 0 therefore must be < not <=
	// for (int arg_index = 0; arg_index <= argc; arg_index++) {
	for (int arg_index = 0; arg_index < argc; arg_index++) {
		char* shifted_string = NULL;
		// BUG: need to use pthread_join
		// waitpid(*t_ids + arg_index, (void*)&shifted_string, 0);
		pthread_join(t_ids[arg_index], (void**)&shifted_string);
		printf("original argv[%d]: %s\nuppercased: %s\nshifted: %s\n", arg_index, argv[arg_index],
		       shared_strings[arg_index], shifted_string);
		// BUG: missing cleanup
		free(shifted_string);
		free(shared_strings[arg_index]);
	}
	// BUG: missing cleanup
	free(shared_strings);
	return EXIT_SUCCESS;
}
