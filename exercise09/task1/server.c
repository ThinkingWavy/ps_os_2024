#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_BUFFER 1024
#define MAX_CONNECTIONS 16
#define MAX_ADMINS 5

volatile sig_atomic_t running = 1;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
int client_count = 0;
pthread_t listener_tid;
pthread_t client_threads[MAX_CONNECTIONS];
int client_sockets[MAX_CONNECTIONS];

typedef struct {
	int socket;
	char username[50];
	int is_admin;
} client_info_t;

char admin_usernames[MAX_ADMINS][50];

void handler() {
	running = 0;
}

void* client_thread(void* arg) {
	client_info_t* client_info = (client_info_t*)arg;
	char msg[MAX_BUFFER];
	long val_read;

	while ((val_read = read(client_info->socket, msg, MAX_BUFFER)) > 0) {
		msg[val_read] = '\0';
		printf("%s: %s", client_info->username, msg);

		if (strncmp(msg, "/shutdown", 9) == 0) {
			if (client_info->is_admin) {
				printf("Server is shutting down.\n");
				pthread_mutex_lock(&clients_mutex);
				printf("Waiting for %d clients to disconnect.\n", client_count - 1);
				pthread_mutex_unlock(&clients_mutex);
				running = 0;
				break;
			}
		}
	}

	close(client_info->socket);
	free(client_info);

	pthread_mutex_lock(&clients_mutex);
	client_count--;
	pthread_mutex_unlock(&clients_mutex);

	return NULL;
}

void* listener_thread(void* arg) {
	int server_fd = *(int*)arg;
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(struct sockaddr_in);

	while (running) {
		int client_socket;
		if ((client_socket = accept(server_fd, (struct sockaddr*)&addr, &addr_len)) < 0) {
			if (errno == EINTR) {
				continue; // if interrupted by signal, continue the loop
			}
			perror("Error while accepting");
			break;
		}

		pthread_mutex_lock(&clients_mutex);
		if (client_count >= MAX_CONNECTIONS) {
			pthread_mutex_unlock(&clients_mutex);
			close(client_socket);
			continue;
		}
		pthread_mutex_unlock(&clients_mutex);

		client_info_t* client_info = malloc(sizeof(client_info_t));
		client_info->socket = client_socket;

		// Receive the username
		read(client_socket, client_info->username, sizeof(client_info->username));
		client_info->username[strcspn(client_info->username, "\n")] = '\0';

		// Check if the user is an admin
		client_info->is_admin = 0;
		for (int i = 0; i < MAX_ADMINS; i++) {
			if (strcmp(client_info->username, admin_usernames[i]) == 0) {
				client_info->is_admin = 1;
				break;
			}
		}

		pthread_mutex_lock(&clients_mutex);
		client_sockets[client_count] = client_socket;
		pthread_create(&client_threads[client_count], NULL, client_thread, client_info);
		client_count++;
		fprintf(stdout, "client count = %d\n", client_count);
		pthread_mutex_unlock(&clients_mutex);
	}

	return NULL;
}

int main(int argc, char const* argv[]) {
	if (argc < 3 || argc > 7) {
		fprintf(stderr, "Usage: %s <port> <admin1> [<admin2> ... <admin5>]\n", argv[0]);
		return EXIT_FAILURE;
	}

	int port = strtol(argv[1], NULL, 10);

	// Save admin usernames
	int num_admins = argc - 2;
	for (int i = 0; i < num_admins; i++) {
		strncpy(admin_usernames[i], argv[i + 2], 50);
	}

	// Create Signal Handler
	struct sigaction sig;
	memset(&sig, 0, sizeof(sig));
	sig.sa_handler = handler;
	if (sigaction(SIGINT, &sig, NULL) == -1) {
		perror("Setting Signal handler for SIGINT failed");
	}

	// Initializing socket
	int server_fd;
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Socket failed");
		return EXIT_FAILURE;
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// Binding the socket
	if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("Binding failed");
		close(server_fd);
		return EXIT_FAILURE;
	}

	// Listening on the socket
	if (listen(server_fd, MAX_CONNECTIONS) < 0) {
		perror("Listening failed");
		close(server_fd);
		return EXIT_FAILURE;
	}

	printf("Listening on port %d\n", port);

	// Create listener thread
	if (pthread_create(&listener_tid, NULL, listener_thread, &server_fd) != 0) {
		perror("Failed to create listener thread");
		close(server_fd);
		return EXIT_FAILURE;
	}

	while (running) {
	}
	pthread_cancel(listener_tid);

	// Wait for the listener thread to finish
	pthread_join(listener_tid, NULL);

	// Close all client sockets and wait for client threads to finish
	for (int i = 0; i < client_count; i++) {
		close(client_sockets[i]);
		pthread_cancel(client_threads[i]);
		pthread_join(client_threads[i], NULL);
	}

	// Close the server socket
	close(server_fd);
	return EXIT_SUCCESS;
}
