#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_BUFFER 128

void replace_newline_with_null(char* str) {
	for (int i = 0; str[i] != '\0'; i++) {
		if (str[i] == '\n') {
			str[i] = '\0';
		}
	}
}

void print_characters(const char* str) {
	for (int i = 0; str[i] != '\0'; i++) {
		printf("%c = %d\n", str[i], str[i]);
	}
}

int main(int argc, char const* argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <port> <username>\n", argv[0]);
		return EXIT_FAILURE;
	}

	int port = strtol(argv[1], NULL, 10);
	const char* username = argv[2];

	// Initializing socket
	int sock;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Socket creation error");
		return EXIT_FAILURE;
	}
	int opt = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // REUSE Socket
	const struct sockaddr_in serv_addr = {
		.sin_family = AF_INET,
		.sin_addr = { .s_addr = htonl(INADDR_ANY) },
		.sin_port = htons(port),
	};

	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("Connection failed");
		return EXIT_FAILURE;
	}

	// Send username
	send(sock, username, strlen(username), 0);

	char msg[MAX_BUFFER];
	while (1) {
		printf("Enter message: ");
		fgets(msg, MAX_BUFFER, stdin);
		print_characters(msg);

		// Send message
		send(sock, msg, strlen(msg), 0);

		replace_newline_with_null(msg);
		if (strncmp(msg, "/shutdown", 9) == 0) {
			printf("Client is shutting down.\n");
			break;
		}
		memset(msg, 0, sizeof(msg));
	}

	close(sock);
	return EXIT_SUCCESS;
}
