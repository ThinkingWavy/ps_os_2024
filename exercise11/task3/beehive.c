#include "myqueue.h"
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // for usleep

// using ChatGPT: https://chatgpt.com/share/efeb2a72-e120-4245-a7e3-f1c7c959a461
// and https://chatgpt.com/share/a032de6d-f9e1-4ad8-ba8b-f6506e695715

#define WORK_MAX 500
#define WORK_MIN 100
#define HIVE_MAX 500
#define HIVE_MIN 100

#define BEAR_ATTACK_CHANCE 10
#define BEAR_SUCCESS_CHANCE 50

#define _POSIX_C_SOURCE 200809L // For PTHREAD_BARRIER detection

typedef struct {
	size_t width;
	size_t height;
	size_t number_flowers;
} field_geometry_t;

typedef struct {
	size_t position;
	bool has_nectar;
} flower_t;

typedef struct bee_data {
	int id;
	field_geometry_t field_dim;
	flower_t* flower_field;
	myqueue* flower_queue;
	pthread_mutex_t* queue_mutex;
	pthread_mutex_t* field_mutex;
	pthread_barrier_t* barrier;
	atomic_bool* must_terminate;
	atomic_size_t* number_collected_flowers;
	atomic_bool* beehive_destroyed;
} bee_data_t;

void cleanup(flower_t* field, bee_data_t* data, pthread_t* thread, pthread_mutex_t* field_mutex,
             pthread_mutex_t* queue_mutex, pthread_barrier_t* barrier, myqueue* queue) {
	free(field);
	free(data);
	free(thread);
	free(queue);
	pthread_mutex_destroy(queue_mutex);
	pthread_mutex_destroy(field_mutex);
	pthread_barrier_destroy(barrier);
}

void calculate_positions(size_t pos, field_geometry_t dimensions, size_t* result) {
	const size_t error_value = (size_t)-1;

	// check for boundaries
	size_t top = (pos >= dimensions.width) ? pos - dimensions.width : error_value;
	size_t bottom = (pos + dimensions.width < dimensions.width * dimensions.height)
	                    ? pos + dimensions.width
	                    : error_value;
	size_t left = (pos % dimensions.width > 0) ? pos - 1 : error_value;
	size_t right = (pos % dimensions.width < dimensions.width - 1) ? pos + 1 : error_value;

	result[0] = top;
	result[1] = bottom;
	result[2] = left;
	result[3] = right;
}

void check_nearby_flowers(size_t pos, field_geometry_t dimensions, flower_t* flower_field,
                          myqueue* queue) {
	size_t positions[4];
	calculate_positions(pos, dimensions, positions);
	const size_t error_value = (size_t)-1;

	for (size_t i = 0; i < 4; i++) {
		if (positions[i] == error_value) continue;
		if (flower_field[positions[i]].has_nectar) {
			myqueue_push(queue, positions[i]);
			printf("(%zu, %zu), ", positions[i] % dimensions.width, positions[i] / dimensions.width);
		}
	}
	printf("\n");
}

enum sleep_enum {
	HIVE,
	WORK
};

void sleep_func(enum sleep_enum var) {
	int min, max;
	if (var == WORK) {
		max = WORK_MAX;
		min = WORK_MIN;
	} else {
		max = HIVE_MAX;
		min = HIVE_MIN;
	}
	int random_number = (rand() % (max - min + 1)) + min;
	usleep(random_number * 1000);
}

bool bear_attack() {
	int attack_chance = rand() % 100;
	if (attack_chance < BEAR_ATTACK_CHANCE) {
		printf("A bear has been encountered!\n");
		int success_chance = rand() % 100;
		if (success_chance < BEAR_SUCCESS_CHANCE) {
			printf("The bees have repelled the bear!\n");
			return false;
		} else {
			printf("The bear has destroyed the beehive!\n");
			return true;
		}
	}
	return false;
}

void* thread_func(void* arg) {
	bee_data_t* bee_local = (bee_data_t*)arg;
	myqueue* queue = bee_local->flower_queue;
	pthread_mutex_t* queue_mutex = bee_local->queue_mutex;
	pthread_mutex_t* field_mutex = bee_local->field_mutex;
	pthread_barrier_t* barrier = bee_local->barrier;

	while (!atomic_load(bee_local->must_terminate)) {
		bool no_flower;
		pthread_mutex_lock(queue_mutex);
		no_flower = myqueue_is_empty(queue);
		if (no_flower) {
			pthread_mutex_unlock(queue_mutex);
			printf("Bee %d is working in the beehive.\n", bee_local->id);
			sleep_func(HIVE);
		} else {
			size_t pos = myqueue_pop(queue);
			pthread_mutex_unlock(queue_mutex);

			printf("Bee %d is flying to position (%zu,%zu) \n", bee_local->id,
			       pos % bee_local->field_dim.width, pos / bee_local->field_dim.width);
			sleep_func(WORK);

			pthread_mutex_lock(field_mutex);
			if (bee_local->flower_field[pos].has_nectar) {
				bee_local->flower_field[pos].has_nectar = false;
				atomic_fetch_add(bee_local->number_collected_flowers, 1);
				printf(
				    "Bee %d collected nectar at position (%zu, %zu) and reports potential food sources: ",
				    bee_local->id, pos % bee_local->field_dim.width, pos / bee_local->field_dim.width);
				check_nearby_flowers(pos, bee_local->field_dim, bee_local->flower_field,
				                     bee_local->flower_queue);
			}
			pthread_mutex_unlock(field_mutex);
		}
		if (pthread_barrier_wait(barrier) == PTHREAD_BARRIER_SERIAL_THREAD) {
			if (bear_attack()) {
				atomic_store(bee_local->must_terminate, true);
				atomic_store(bee_local->beehive_destroyed, true);
			}
		}
		pthread_barrier_wait(barrier);
	}
	return NULL;
}

int main(int argc, char const* argv[]) {
	// input parsing
	if (argc != 4) {
		fprintf(stderr, "Usage: %s <flower field width> <flower field height> <number of bees> \n",
		        argv[0]);
		return EXIT_FAILURE;
	}

	srand(time(NULL));

	const size_t number_of_bees = atoi(argv[3]);
	const field_geometry_t global_field = { .width = atoi(argv[1]),
		                                      .height = atoi(argv[2]),
		                                      .number_flowers = atoi(argv[1]) * atoi(argv[2]) };

	// Initialize flower field
	flower_t* global_flower_field =
	    calloc(global_field.height * global_field.width, sizeof(flower_t));
	for (size_t i = 0; i < global_field.height * global_field.width; i++) {
		global_flower_field[i].has_nectar = true;
		global_flower_field[i].position = i;
	}

	// Initialize the mutexes
	pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t field_mutex = PTHREAD_MUTEX_INITIALIZER;

	// Initialize the barrier
	pthread_barrier_t barrier;
	pthread_barrier_init(&barrier, NULL, number_of_bees);

	// Initialize the queue
	myqueue* queue = malloc(sizeof(myqueue));
	myqueue_init(queue);

	// Initialize the thread data for the bees
	bee_data_t* bee_data = calloc(number_of_bees, sizeof(bee_data_t));
	pthread_t* bees = calloc(number_of_bees, sizeof(pthread_t));

	// Push the first flower position
	myqueue_push(queue, global_field.height * global_field.width / 2);

	// Kill switch variables
	atomic_bool global_terminate = false;
	atomic_size_t global_flower_count = 0;
	atomic_bool global_beehive_destroyed = false;

	// creating bee threads
	for (int i = 0; i < number_of_bees; i++) {
		bee_data[i].id = i;
		bee_data[i].flower_queue = queue;
		bee_data[i].queue_mutex = &queue_mutex;
		bee_data[i].field_mutex = &field_mutex;
		bee_data[i].barrier = &barrier;
		bee_data[i].field_dim = global_field;
		bee_data[i].flower_field = global_flower_field;
		bee_data[i].must_terminate = &global_terminate;
		bee_data[i].number_collected_flowers = &global_flower_count;
		bee_data[i].beehive_destroyed = &global_beehive_destroyed;
		if (pthread_create(&bees[i], NULL, thread_func, &bee_data[i]) != 0) {
			fprintf(stderr, "Error while creating pthread!");
			cleanup(global_flower_field, bee_data, bees, &queue_mutex, &field_mutex, &barrier, queue);
			exit(EXIT_FAILURE);
		}
		printf("Spawning bee number %d\n", i);
	}

	while ((atomic_load(&global_flower_count) < global_field.number_flowers) &&
	       !atomic_load(&global_beehive_destroyed))
		;
	atomic_store(&global_terminate, true);

	// Joining threads
	for (size_t i = 0; i < number_of_bees; i++) {
		pthread_join(bees[i], NULL);
	}

	if (atomic_load(&global_beehive_destroyed)) {
		printf("%zu bees collected nectar from %zu/%zu flowers.\n", number_of_bees,
		       atomic_load(&global_flower_count), global_field.number_flowers);
		printf("THe beehive was destroyed.\n");
	} else {
		printf("The bees collected the nectar from %zu/%zu flowers successfully.\n",
		       atomic_load(&global_flower_count), global_field.number_flowers);
		printf("All %zu bees are back in the hive. Congratulations to the diligent bees!\n",
		       number_of_bees);
	}
	cleanup(global_flower_field, bee_data, bees, &field_mutex, &queue_mutex, &barrier, queue);
	return EXIT_SUCCESS;
}