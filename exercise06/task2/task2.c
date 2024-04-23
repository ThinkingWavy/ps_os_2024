#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "myqueue.h"

typedef struct data {
	myqueue* queue;
	pthread_t thread;
	long sum;
} data_t;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* thread_func();

int main(int argc, char** argv) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <number consumer> <number elements>\n", argv[0]);
		return EXIT_FAILURE;
	}

	int c = (int)strtol(argv[1], NULL, 10);
	int n = (int)strtol(argv[2], NULL, 10);

	if (n <= 0 || c <= 0) {
		fprintf(stderr, "Parameters must be positive!\n");
		return EXIT_FAILURE;
	}

	pthread_t* tid = calloc(n, sizeof(pthread_t));
	data_t* data = calloc(c, sizeof(data_t));
	myqueue queue;
	myqueue_init(&queue);

	// spawning threads
	for (int i = 0; i < c; i++) {
		data[i].queue = &queue;
		data[i].thread = i;
		if (pthread_create(tid + i, NULL, &thread_func, data + i) != 0) {
			fprintf(stderr, "Error creating threads!");
			// clean up after waiting on threads
			free(data);
			free(tid);
			return EXIT_FAILURE;
		}
	}

	// Filling the queue with values
	for (int i = 0; i < n; i++) {
		int num = (i % 2 == 0) ? 1 : -1;
		pthread_mutex_lock(&mutex);
		myqueue_push(&queue, num);
		pthread_mutex_unlock(&mutex);
	}
	for (int i = 0; i < c; i++) {
		pthread_mutex_lock(&mutex);
		myqueue_push(&queue, INT_MAX);
		pthread_mutex_unlock(&mutex);
	}

	// waiting and evaluating the threads
	int sum = 0;
	for (int i = 0; i < c; i++) {
		if (pthread_join(tid[i], NULL) != 0) {
			fprintf(stderr, "Error joining threads.\n");
			return EXIT_FAILURE;
		} else {
			sum += data[i].sum;
		}
	}

	printf("Final sum: %d\n", sum);
	free(data);
	free(tid);
	pthread_mutex_destroy(&mutex);
	return EXIT_SUCCESS;
}

void* thread_func(void* args) {
	data_t* data = (data_t*)args;
	// waiting for queue to be filled
	while (myqueue_is_empty(data->queue)) {
		sleep(1);
	}
	// summing up queue elements
	while (1) {
		int temp = 0;
		pthread_mutex_lock(&mutex);
		if (!myqueue_is_empty(data->queue)) {
			temp = myqueue_pop(data->queue);
		}
		pthread_mutex_unlock(&mutex);
		if (temp == INT_MAX) {
			break;
		} else {
			data->sum += temp;
		}
	}
	fprintf(stdout, "Consumer %lu sum: %ld\n", data->thread, data->sum);
	pthread_exit(NULL);
}