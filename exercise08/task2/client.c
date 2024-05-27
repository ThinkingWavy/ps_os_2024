// Client side C/C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
__sig_atomic_t running = 1;

static void handler() {
	running = 0;
}

void print_until_newline(const char* str) {
	while (*str != '\0' && *str != '\n') {
		putchar(*str);
		str++;
	}
	putchar('\n'); // Print a newline at the end for better formatting
}

int main(int argc, char const* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port> <username>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	struct sigaction sig;
	memset(&sig, 0, sizeof(sig));
	sig.sa_handler = handler;
	if (sigaction(SIGINT, &sig, NULL) == -1) { // id: 2
		perror("Setting Signal handler for SIGINT failed");
	}
	int port = atoi(argv[1]);

	int client_fd;
	// struct sockaddr_in serv_addr;

	if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}
	int opt = 1;
	setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	const struct sockaddr_in serv_addr = {
		.sin_family = AF_INET,
		.sin_addr = { .s_addr = htonl(INADDR_ANY) },
		.sin_port = htons(port),
	};

	int err_connect = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if (err_connect < 0) {
		printf("\nConnection Failed \n");
		return -1;
	}

	char msg[BUFFER_SIZE] = { '0' };
	char read_buf[BUFFER_SIZE] = { '0' };
	while (running) {
		fflush(stdout);
		fgets(msg, BUFFER_SIZE, stdin);

		// ensure null termination
		msg[BUFFER_SIZE - 1] = '\0';

		if (strncmp(msg, "/quit\n", 6) == 0) {
			break;
		} else if (strncmp(msg, "/shutdown\n", 11) == 0) {
			send(client_fd, "/shutdown", 10, 0);
			break;
		}
		send(client_fd, msg, strlen(msg), 0);
		read(client_fd, read_buf, sizeof(read_buf));

		print_until_newline(read_buf);
		memset(read_buf, '0', sizeof(read_buf));
	}

	// closing the connected socket
	close(client_fd);
	return 0;
}
