#define _POSIX_C_SOURCE 199309L
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

// Credit for time measurement implementation: https://stackoverflow.com/a/41959179

// Dice-roll simulation
double DR_p(int T, int64_t S) {
	int64_t hit_count = 0;
	for (int64_t i = 0; i < S; ++i) {
		const int roll = rand() % 6 + 1;
		if (roll == T) {
			hit_count++;
		}
	}
	return (double)hit_count / S;
}

int main(int argc, char** argv) {
	struct timespec start, end;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <amount_childs> <steps>\n", argv[0]);
		return EXIT_FAILURE;
	}

	int64_t amount_childs = atoi(argv[1]);
	int64_t steps = atoi(argv[2]);

	int* children = malloc(sizeof(int) * amount_childs);

	clock_gettime(CLOCK_MONOTONIC_RAW, &start); // start time measurement with idependent clock
	                                            // ->(time zone, winter-sommer time, etc)

	for (int64_t i = 0; i < amount_childs; i++) {
		// srand(getpid()); -> for every forked process the same seed is used
		children[i] = fork();
		double result;

		switch (children[i]) {
			case -1: // error case
				fprintf(stderr, "Failed to fork!\n");
				free(children);
				return EXIT_FAILURE;
				break;
			case 0: // child case
				srand(getpid());
				int target_num = rand() % 6 + 1;
				result = DR_p(target_num, steps);
				clock_gettime(CLOCK_MONOTONIC_RAW, &end);
				double time = (end.tv_nsec - start.tv_nsec) / 1000000000.0 + // conversion ns to s -> *10^9
				              (end.tv_sec - start.tv_sec);

				printf("Child %4ld PID = %6d. DR_p(%d, %ld) = %.6f. Elapsed time = %f (s).\n", i, getpid(),
				       target_num, steps, result, time);
				exit(0); // child process terminates
				break;

			default: break; // parent case
		}
	}

	// wait for children to finish
	int64_t running_childs = amount_childs;
	while (running_childs > 0) {
		wait(0);
		running_childs--;
	}
	printf("Done.\n");
	free(children);
	return EXIT_SUCCESS;
}