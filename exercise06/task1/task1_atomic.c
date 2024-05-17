#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

#define THREADS 1000
#define ITERATIONS 25000
#define EVEN_INCREASE 42
#define ODD_INCREASE -41

atomic_uint counter = 0;

void* thread_func(void* param);

int main(void) {
	pthread_t pt_id[THREADS];

	for (int i = 0; i < THREADS; i++) {
		if (pthread_create(pt_id + i, NULL, &thread_func, &counter) != 0) {
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

	printf("Counter = %d \n", counter);
	return EXIT_SUCCESS;
}

void* thread_func(void* param) {
	atomic_uint* p_counter = param;
	for (int i = 0; i < ITERATIONS; i++) {
		if (i % 2 == 0) {
			atomic_fetch_add(p_counter, EVEN_INCREASE);
		} else {
			atomic_fetch_add(p_counter, ODD_INCREASE);
		}
	}
	return 0;
}