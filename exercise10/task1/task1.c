#define _DEFAULT_SOURCE

#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
	atomic_int max_n;
	atomic_int n;
	atomic_int current_max;
	atomic_int current_max_count;
	volatile int rolls[];
} game_state;

struct ThreadData {
	int id;
	game_state* state_data;
	pthread_t thread;
	pthread_barrier_t* barrier;
};
typedef struct ThreadData ThreadData;

void cleanup(void* data, pthread_barrier_t barrier) {
	free(data);
	pthread_barrier_destroy(&barrier);
}

void* thread_func(void* param) {
	ThreadData* thread_data = (ThreadData*)param;
	game_state* state = thread_data->state_data;
	atomic_int* num_players = &state->n;
	bool loser = false;

	while (*num_players > 1) {
		if (loser == false) {
			int r = rand() % 6 + 1;
			state->rolls[thread_data->id] = r;
			printf("Player %d: rolled a %d\n", thread_data->id, state->rolls[thread_data->id]);
		}

		if (pthread_barrier_wait(thread_data->barrier) == PTHREAD_BARRIER_SERIAL_THREAD) {
			state->current_max = 0;
			state->current_max_count = 0;
			for (int i = 0; i < state->max_n; i++) {
				if (state->rolls[i] > state->current_max) {
					state->current_max = state->rolls[i];
					state->current_max_count = 1;
				} else if (state->rolls[i] == state->current_max) {
					state->current_max_count++;
				}
			}
			// check if all players had same number
			if (state->current_max_count == state->n) {
				printf("Repeating round\n");
			}
		}

		pthread_barrier_wait(thread_data->barrier);

		// each player eliminates himself if its current roll equals the current_max
		if (loser == false && state->current_max_count != state->n) {
			if (state->rolls[thread_data->id] == state->current_max) {
				*num_players -= 1;
				printf("Eliminating player %d\n", thread_data->id);
				state->rolls[thread_data->id] = 0;
				loser = true;
			}
		}

		if (pthread_barrier_wait(thread_data->barrier) == PTHREAD_BARRIER_SERIAL_THREAD) {
			printf("=============================== \n");
		}
		pthread_barrier_wait(thread_data->barrier);
	}
	if (loser == false) {
		printf("Player %d won the game\n", thread_data->id);
	}
	pthread_exit(NULL);
}

int main(int argc, char const* argv[]) {
	// input parsing
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <number_players> \n", argv[0]);
		return EXIT_FAILURE;
	}

	int num_players = strtol(argv[1], NULL, 10);

	if (num_players <= 0) {
		fprintf(stderr, "Error: Number of players must be greater than zero\n");
		return EXIT_FAILURE;
	}

	// set up
	game_state* state = calloc(1, sizeof(game_state) + sizeof(int) * num_players);
	state->max_n = num_players;
	state->n = num_players;

	ThreadData data[num_players];
	pthread_barrier_t barrier;

	if (pthread_barrier_init(&barrier, NULL, num_players) != 0) {
		fprintf(stderr, "Error: Failed to initialize barrier!\n");
		cleanup(state, barrier);
		return EXIT_FAILURE;
	}

	srand(time(NULL));

	// creating player threads
	for (int i = 0; i < num_players; i++) {
		data[i].state_data = state;
		data[i].id = i;
		data[i].barrier = &barrier;
		if (pthread_create(&data[i].thread, NULL, &thread_func, &data[i]) != 0) {
			{
				fprintf(stderr, "Error while creating pthread!");
				cleanup(state, barrier);
				exit(EXIT_FAILURE);
			}
		}
	}

	// waiting for game to finish
	for (int i = 0; i < num_players; i++) {
		if (pthread_join(data[i].thread, NULL) != 0) {
			fprintf(stderr, "Error while joining pthread!");
			cleanup(state, barrier);
			exit(EXIT_FAILURE);
		}
	}

	cleanup(state, barrier);
}