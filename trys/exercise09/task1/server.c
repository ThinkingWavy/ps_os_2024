#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_CONNECTIONS 10
#define MAX_ADMINS 5
#define MAX BUFFER 1024
#define MAX_CHAR_USERNAME 50

sig_atomic_t running = 1;

void* thread_func(void* args);
void* listener_thread(void* args);

typedef struct {
	int socket;
	char username[MAX_CHAR_USERNAME];
	int is_admin;
} client_info_t;

typedef struct {
	int sock;
	char admin[MAX_ADMINS][MAX_CHAR_USERNAME];
	client_info_t* clients;
} lt_data;

static void handler() {
	running = 0;
}

int main(int argc, char* argv[]) {

	if (argc < 2) {
		fprintf(stderr, "USAGE: %s <port> (<admin1> ...)", argv[0]);
		perror("Please enter the port as the first parameter!");
		return EXIT_FAILURE;
	}

	int port = strtol(argv[1], NULL, 10);

	// Create Signal Handler
	struct sigaction sig;
	memset(&sig, 0, sizeof(sig));
	sig.sa_handler = handler;
	if (sigaction(SIGINT, &sig, NULL) == -1) {
		perror("Setting Signal handler for SIGINT failed");
	}

	// Initialize Socket
	int sock;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Socket creation failed\n");
		return (EXIT_FAILURE);
	}
	int opt = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // REUSE Socket
	const struct sockaddr_in serv_addr = {
		.sin_family = AF_INET,
		.sin_addr = { .s_addr = htonl(INADDR_LOOPBACK) },
		.sin_port = htons(port),
	};

	// Bind the socket
	int err_bind = bind(sock, (struct sockaddr*)&serv_addr, sizeof(struct sockaddr_in));
	if (err_bind == EADDRINUSE) {
		fprintf(stderr, "Address already in use: Please make sure that the port is properly closed!\n");
		return (EXIT_FAILURE);
	} else if (err_bind < 0) {
		fprintf(stderr, "Error while binding socket\n");
		return (EXIT_FAILURE);
	}

	// Listening on the socket
	if (listen(sock, MAX_CONNECTIONS) < 0) {
		perror("Error while starting listening");
		return (EXIT_FAILURE);
	}

	// Allocate memory for connections and listener thread
	client_info_t* clients = (client_info_t*)calloc(MAX_CONNECTIONS, sizeof(client_info_t));
	lt_data* listener_data = malloc(sizeof(lt_data));
	listener_data->sock = sock;
	listener_data->clients = clients;

	// Spawn listening thread
	pthread_t listener_tid;
	if (pthread_create(&listener_tid, NULL, listener_thread, &listener_data) != 0) {
		close(sock);
		free(listener_data);
		free(clients);
		return EXIT_FAILURE;
	}

	// Keep running until flag is set
	while (running) {
	}
	pthread_cancel(listener_tid);

	// Cleanup
	close(sock);
	free(clients);
	return EXIT_SUCCESS;
}

void* listener_thread(void* args) {
	lt_data* data = (lt_data*)args;
	struct sockaddr_in addr;

	while (running) {
		int client_socket;
		if ((client_socket = accept(data->sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in))) <
		    0) {
			if (errno == EINTR) {
				continue; // if interrupted by signal, continue the loop
			}
			perror("Error while accepting");
			break;
		}
		printf("Connection established!\n");
	}
}
