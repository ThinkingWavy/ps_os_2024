#include "common.h"

int main(int argc, char const* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <client-name1> <client-name2> ...\n", argv[0]);
	}

	// create correct file name
	char name[SIZE];
	char path[SIZE] = ADDRESS;
	strcat(path, argv[1]);
	strncpy(name, path, SIZE);

	// prompts continuously for expression
	char msg[PIPE_BUF];
	int fd = open(name, O_WRONLY);

	// opening fifo
	if (fd == -1) {
		perror("Error while opening fifo\n");
		exit(EXIT_FAILURE);
	}

	// ask for  expressions
	do {
		printf("Expression: ");
		fgets(msg, PIPE_BUF, stdin);
		write(fd, msg, strlen(msg));
	} while (strcmp(msg, "\n\0") != 0);

	close(fd);
	// cleanup
	unlink(name);

	return EXIT_SUCCESS;
}
