#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define THREADS 1000
#define ITERATIONS 25000
#define EVEN_INCREASE 42
#define ODD_INCREASE -41

void* thread_func(void* param);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(void) {

	pthread_t pt_id[THREADS];
	unsigned int* counter = calloc(1, sizeof(unsigned int));

	for (int i = 0; i < THREADS; i++) {
		if (pthread_create(pt_id + i, NULL, &thread_func, counter) != 0) {
			fprintf(stderr, "Error creating threads!");
			return EXIT_FAILURE;
		}
	}
	for (int i = 0; i < THREADS; i++) {
		if (pthread_join(pt_id[i], NULL) != 0) {
			fprintf(stderr, "Error joining threads.\n");
			return EXIT_FAILURE;
		};
	}

	printf("Counter = %d \n", *counter);
	free(counter);
	return EXIT_SUCCESS;
}

void* thread_func(void* param) {
	unsigned int* counter = param;
	for (int i = 0; i < ITERATIONS; i++) {
		int temp;
		if (i % 2 == 0) {
			temp = EVEN_INCREASE;
		} else {
			temp = ODD_INCREASE;
		}
		pthread_mutex_lock(&mutex);
		*counter += temp;
		pthread_mutex_unlock(&mutex);
	}
	return 0;
}