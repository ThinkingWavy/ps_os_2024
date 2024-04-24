#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

#define FILE "file.txt"

int main(void) {
	int f_ptr[2];
	pipe(f_ptr);

	// commands must end with NULL
	char* args_parent[] = { "cat", "file.txt", NULL };
	char* args_child[] = { "cut", "-c", "22-", NULL };

	pid_t pid = fork();

	if (pid == -1) { // Fail case
		fprintf(stderr, "Fork failed\n");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		// Case child
		// make stdin references to reading end of pipe
		dup2(f_ptr[0], STDIN_FILENO);
		close(f_ptr[1]);
		close(f_ptr[0]);
		execvp(args_child[0], args_child);
	} else {
		// Case parent
		// make stdout references to writing end of pipe
		dup2(f_ptr[1], STDOUT_FILENO);

		close(f_ptr[1]);
		close(f_ptr[0]);
		execvp(args_parent[0], args_parent);
	}
	return EXIT_SUCCESS;
}