#include "myqueue.h"
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_BUFFER 1024
#define MAX_CONNECTIONS 16
#define POISON_PILL -1

void* thread_func(void* args);
void handle_client(int client_socket, double* donations, atomic_int* running);

pthread_mutex_t mutex_queue = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_donation = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

typedef struct data {
	myqueue* queue;
	pthread_t thread;
	double* donations;
	atomic_int* running;
} data_t;

int main(int argc, char const* argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <port> <#request_handlers>\n", argv[0]);
		return (EXIT_FAILURE);
	}

	long port = strtol(argv[1], NULL, 10);
	long n = strtol(argv[2], NULL, 10);

	// Create threads and data structures
	pthread_t* tid = calloc(n, sizeof(pthread_t));
	data_t* data = calloc(n, sizeof(data_t));
	myqueue queue;
	myqueue_init(&queue);
	atomic_int running = 1;
	double donations = 0.0;

	// Spawn threads
	for (int i = 0; i < n; i++) {
		data[i].queue = &queue;
		data[i].donations = &donations;
		data[i].running = &running;
		if (pthread_create(&tid[i], NULL, thread_func, &data[i]) != 0) {
			fprintf(stderr, "Error creating threads!");
			free(data);
			free(tid);
			return EXIT_FAILURE;
		}
	}

	// Initialize Socket
	int server_fd;
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Socket creation failed\n");
		free(data);
		free(tid);
		return (EXIT_FAILURE);
	}

	struct sockaddr_in addr;
	int addr_len = sizeof(struct sockaddr_in);
	memset(&addr, 0, addr_len);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// Bind the socket
	if (bind(server_fd, (struct sockaddr*)&addr, addr_len) < 0) {
		fprintf(stderr, "Error while binding socket\n");
		free(data);
		free(tid);
		return (EXIT_FAILURE);
	}

	// Listen on the socket
	if (listen(server_fd, MAX_CONNECTIONS) < 0) {
		fprintf(stderr, "Error while starting listening\n");
		free(data);
		free(tid);
		return (EXIT_FAILURE);
	}
	printf("Listening on port %ld\n", port);

	while (running) {
		int client_socket = accept(server_fd, (struct sockaddr*)&addr, (socklen_t*)&addr_len);
		if (client_socket < 0) {
			if (errno == EINTR) {
				break;
			}
			fprintf(stderr, "Error while accepting\n");
			free(data);
			free(tid);
			return (EXIT_FAILURE);
		}

		pthread_mutex_lock(&mutex_queue);
		myqueue_push(&queue, client_socket);
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex_queue);
	}

	// Send poison pills to threads to stop them
	pthread_mutex_lock(&mutex_queue);
	for (int i = 0; i < n; i++) {
		myqueue_push(&queue, POISON_PILL);
	}
	pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&mutex_queue);

	// Join threads
	for (int i = 0; i < n; i++) {
		if (pthread_join(tid[i], NULL) != 0) {
			fprintf(stderr, "Error joining threads.\n");
			free(data);
			free(tid);
			return EXIT_FAILURE;
		}
	}

	free(data);
	free(tid);
	close(server_fd);
	return EXIT_SUCCESS;
}

void* thread_func(void* args) {
	data_t* data = (data_t*)args;
	while (1) {
		pthread_mutex_lock(&mutex_queue);
		while (myqueue_is_empty(data->queue)) {
			pthread_cond_wait(&cond, &mutex_queue);
		}
		int client_socket = myqueue_pop(data->queue);
		pthread_mutex_unlock(&mutex_queue);

		// Check for poison pill
		if (client_socket == POISON_PILL) {
			break;
		}

		// Handle the client
		handle_client(client_socket, data->donations, data->running);
	}

	pthread_exit(NULL);
}

void handle_client(int client_socket, double* donations, atomic_int* running) {
	char buffer[MAX_BUFFER] = { 0 };
	int val_read = read(client_socket, buffer, MAX_BUFFER - 1);
	if (val_read < 0) {
		fprintf(stderr, "Error reading from client socket\n");
		close(client_socket);
		return;
	}

	// Simulate work
	usleep(100000);

	// Parse request line
	char method[16], path[64];
	sscanf(buffer, "%s %s", method, path);

	if (strcmp(method, "GET") == 0 && strcmp(path, "/") == 0) {
		const char* response_body = "<html><body><h1>Welcome to the server!</h1></body></html>";
		char response[MAX_BUFFER];
		snprintf(response, sizeof(response),
		         "HTTP/1.1 200 OK\r\n"
		         "Content-Type: text/html\r\n"
		         "Content-Length: %ld\r\n"
		         "\r\n"
		         "%s",
		         strlen(response_body), response_body);
		write(client_socket, response, strlen(response));
	} else if (strcmp(method, "POST") == 0 && strcmp(path, "/donate") == 0) {
		char* content_length_str = strstr(buffer, "Content-Length: ");
		if (content_length_str) {
			int content_length = atoi(content_length_str + 16);
			char* body = strstr(buffer, "\r\n\r\n");
			if (body) {
				body += 4;
				double donation = strtod(body, NULL);
				pthread_mutex_lock(&mutex_donation);
				*donations += donation;
				pthread_mutex_unlock(&mutex_donation);

				char response[MAX_BUFFER];
				snprintf(response, sizeof(response),
				         "HTTP/1.1 200 OK\r\n"
				         "Content-Type: text/plain\r\n"
				         "Content-Length: %d\r\n"
				         "\r\n"
				         "Thank you very much for donating $%.2f! The balance is now $%.2f.",
				         val_read, donation, *donations);
				write(client_socket, response, strlen(response));
			}
		}
	} else if (strcmp(method, "POST") == 0 && strcmp(path, "/shutdown") == 0) {

		atomic_fetch_sub(running, 1);

		char response[] = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
		write(client_socket, response, sizeof(response) - 1);
	} else {
		char response[] = "HTTP/1.1 501 Not Implemented\r\nContent-Length: 0\r\n\r\n";
		write(client_socket, response, sizeof(response) - 1);
	}

	close(client_socket);
}
