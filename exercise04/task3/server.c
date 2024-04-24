#include "common.h"

int main(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <client-name>...\n", argv[0]);
		return EXIT_FAILURE;
	}

	char** fifo_path = (char**)calloc(argc - 1, sizeof(char*));
	int fifo[argc - 1];
	int n_clients = argc - 1;
	char** client_names = argv;
	double* counter = (double*)calloc(n_clients, sizeof(double));

	// populate fifo_paths array with correct file names and creating them
	for (int i = 0; i < n_clients; i++) {
		fifo_path[i] = (char*)calloc(SIZE, sizeof(char));
		char template[SIZE] = ADDRESS;
		strcat(template, argv[i + 1]);
		strncpy(fifo_path[i], template, SIZE);
		if (mkfifo(fifo_path[i], 0666) != 0) {
			perror("Error while creating fifo\n");
			return EXIT_FAILURE;
		}
	}

	printf("creating poll structs\n");
	// creating poll structs
	struct pollfd pfds[n_clients];
	for (int i = 0; i < n_clients; i++) {
		fifo[i] = open(fifo_path[i], O_RDONLY);
		if (fifo[i] == -1) {
			perror("Error while opening fds");
			exit(EXIT_FAILURE);
		}
		pfds[i].fd = fifo[i];
		printf("%s connected \n", argv[i + 1]);
		pfds[i].events = POLLIN;
	}

	printf("All clients connected successfully, start reading..\n");
	// read from fifo_names and print messages
	char readBuffer[PIPE_BUF];
	int closed = 0;
	while (closed < (n_clients)) {
		poll(pfds, n_clients, 100);
		for (int i = 0; i < n_clients; i++) {
			if (pfds[i].revents & POLLIN) {
				memset(readBuffer, '\0', PIPE_BUF);
				long byte_read = read(fifo[i], readBuffer, sizeof(readBuffer));

				// check for disconnect symbol
				if (readBuffer[0] == '\n') {
					printf("%s disconnected from server\n", argv[i + 1]);
					closed++;
					break;
				}

				// if something was read
				if (byte_read > 0) {
					// usage of atoi because more modern functions like strtol did not work
					int result = atoi(readBuffer);
					// only allow expressions with numbers greater than 0
					if (result > 0) {
						counter[i] += result;
						printf("%s: %g\n", client_names[i + 1], counter[i]);
					} else {
						printf("%s: %s is was malformed\n", client_names[i + 1], readBuffer);
					}
				}
			}
			// nothing was read -> closing connection
			if (pfds[i].revents & POLLHUP) {
				close(fifo[i]);
			}
		}
	}

	printf("All clients disconnected, starting clean up.\n");
	// unlink fifo_path
	for (int i = 0; i < n_clients; i++) {
		unlink(fifo_path[i]);
	}

	printf("Free Heap\n");
	// free allocated memory
	for (int i = 0; i < n_clients; i++) {
		free(fifo_path[i]);
	}
	free(fifo_path);
	printf("Clean up successful, shutting down\n");
	return EXIT_SUCCESS;
}
