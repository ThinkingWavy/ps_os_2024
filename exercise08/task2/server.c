#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_BUFFER 1024
#define MAX_CONNECTIONS 16

__sig_atomic_t running = 1;

static void handler() {
	running = 0;
}

// Inspired by: https://www.geeksforgeeks.org/socket-programming-cc/
int main(int argc, char const* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		return (EXIT_FAILURE);
	}

	struct sigaction sig;
	memset(&sig, 0, sizeof(sig));
	sig.sa_handler = handler;
	if (sigaction(SIGINT, &sig, NULL) == -1) {
		perror("Setting Signal handler for SIGINT failed");
	}
	int port = strtol(argv[1], NULL, 10);

	// Initialising Socket
	int server_fd;
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // SOCK_STREAM for TCP, AF_INET for IPv4
		fprintf(stderr, "socket failed");
		return (EXIT_FAILURE);
	}
	struct sockaddr_in addr;
	int addr_len = sizeof(struct sockaddr_in);
	memset(&addr, 0, addr_len);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// Binding the socked
	int err_bind = bind(server_fd, &addr, addr_len);
	if (err_bind < 0) {
		fprintf(stderr, "Error while binding socket\n");
		return (EXIT_FAILURE);
	} else if (err_bind == EADDRINUSE) {
		fprintf(stderr, "Address already in use: Please make sure that the port is properly closed!\n");
		return (EXIT_FAILURE);
	}

	// Listening on the socket
	if (listen(server_fd, MAX_CONNECTIONS) < 0) {
		fprintf(stderr, "Error while starting listening\n");
		return (EXIT_FAILURE);
	}
	printf("Listening on port %d\n", port);

	// Creating sockets for each client
	char msg[MAX_BUFFER] = { '\0' };
	int client_socket;
	long val_read;

	double donations = 0;

	while (running) {
		if ((client_socket = accept(server_fd, (struct sockaddr*)&addr, (socklen_t*)&addr_len)) < 0) {
			fprintf(stderr, "Error while accepting\n");
			return (EXIT_FAILURE);
		}
		printf("Connection established!\n");

		while (1) {
			val_read = read(client_socket, msg, MAX_BUFFER);
			if (val_read < 0) {
				perror("reading error\n");
				close(client_socket);
				return (EXIT_FAILURE);
			} else if (val_read == 0) {
				printf("Client disconnected.\n");
				printf("Closing connection...\n");
				break;
			}

			if (strncmp(msg, "/shutdown", 9) == 0) {
				printf("Shutting down\n");
				close(client_socket);
				close(server_fd);
				return (EXIT_SUCCESS);
			}

			// remove newline from message
			msg[strcspn(msg, "\n")] = '\0';

			char* end = NULL;
			double tmp = 0;
			tmp = strtod(msg, &end);

			if (strcmp(end, "")) {
				dprintf(client_socket, "%s is not a number!\n", msg);
			} else {
				donations += tmp;
				dprintf(client_socket, "Donations: %.2lf\n", donations);
			}
			// reset message buffer
			memset(msg, '\0', sizeof(msg));
		}
	}
	return EXIT_SUCCESS;
}
