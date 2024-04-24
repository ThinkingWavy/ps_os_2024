#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

#define FILE "file.txt"

void close_pipes(int* pipe1, int* pipe2);

int main(void) {
	int fptr_cat_grep[2];
	int fptr_grep_cut[2];
	pipe(fptr_cat_grep); // cat -> grep
	pipe(fptr_grep_cut); // grep -> cut

	// commands must end with NULL
	char* args_cat[] = { "cat", "file.txt", NULL };
	char* args_cut[] = { "cut", "-c", "22-", NULL };
	char* args_grep[] = { "grep", "^;", NULL };

	pid_t pid_1 = fork();

	if (pid_1 == -1) {
		// fail case
		fprintf(stderr, "fork failed\n");
		exit(EXIT_FAILURE);
	} else if (pid_1 == 0) {
		// child 1
		pid_t pid_2 = fork();

		if (pid_2 == -1) {
			// fail case
			fprintf(stderr, "fork failed\n");
			exit(EXIT_FAILURE);
		} else if (pid_2 == 0) {
			// child 1 child
			dup2(fptr_cat_grep[1], STDOUT_FILENO); // setting STDOUT to writing end

			close_pipes(fptr_cat_grep, fptr_grep_cut);

			execvp(args_cat[0], args_cat);
		} else {
			// child 1 parent
			dup2(fptr_cat_grep[0], STDIN_FILENO);  // setting STDIN to reading end
			dup2(fptr_grep_cut[1], STDOUT_FILENO); // setting STDOUT to writing end

			close_pipes(fptr_cat_grep, fptr_grep_cut);

			execvp(args_grep[0], args_grep);
		}
	} else {
		// parent

		dup2(fptr_grep_cut[0], STDIN_FILENO); // setting STDIN to reading end
		close_pipes(fptr_cat_grep, fptr_grep_cut);

		execvp(args_cut[0], args_cut);
	}

	return EXIT_SUCCESS;
}

void close_pipes(int* pipe1, int* pipe2) {
	close(pipe1[0]);
	close(pipe1[1]);
	close(pipe2[0]);
	close(pipe2[1]);
}