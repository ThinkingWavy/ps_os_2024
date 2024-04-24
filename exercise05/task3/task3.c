#include <fcntl.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

const int flags_shm_open = O_CREAT | O_RDWR;
const int flags_map = MAP_SHARED;
const int map_protection = PROT_READ | PROT_WRITE;
const char* NAME = "/zanolin_camillo3";

typedef struct TODO {
	uint64_t result;
	sem_t sem_fd_1;
	sem_t sem_fd_2;
	uint64_t buffer[];
} todo_t;

void validate_result(uint64_t result, const uint64_t K, const uint64_t N);

int main(int argc, char** argv) {
	uint64_t N;
	uint64_t K;
	uint64_t L;
	// Validate argument amount
	if (argc != 4) {
		fprintf(stderr, "Usage: %s <integer> <number buffer operations> <buffer length>\n", argv[0]);
		return EXIT_FAILURE;
	}

	// Parsing parameters
	N = strtol(argv[1], NULL, 10);
	K = strtol(argv[2], NULL, 10);
	L = strtol(argv[3], NULL, 10);

	// Validate parameters
	if (K <= 0 || L <= 0) {
		fprintf(stderr,
		        "Number of buffer operations and/or buffer length must be greater than zero!\n");
		return EXIT_FAILURE;
	}

	const size_t shared_memory_size = sizeof(todo_t) + L * sizeof(uint64_t);

	// set up shared memory
	int fd_shared = shm_open(NAME, flags_shm_open, 600);
	if (fd_shared == -1) {
		fprintf(stderr, "Couldn't open shared memory\n");
		return EXIT_FAILURE;
	}

	// truncate Memory
	if (ftruncate(fd_shared, shared_memory_size) != 0) {
		close(fd_shared);
		unlink(NAME);
		fprintf(stderr, "Unable to truncate ring buffer\n");
		return EXIT_FAILURE;
	}

	// mapping memory
	todo_t* shared_mem = mmap(NULL, shared_memory_size, map_protection, flags_map, fd_shared, 0);
	if (shared_mem == MAP_FAILED) {
		fprintf(stderr, "shared memory mapping failed\n");
		close(fd_shared);
		unlink(NAME);
		return EXIT_FAILURE;
	}

	// initialize semaphores and checking for errors
	if (sem_init(&shared_mem->sem_fd_1, 1, 0) != 0 || sem_init(&shared_mem->sem_fd_2, 1, L) != 0) {
		fprintf(stderr, "Error initializing semaphore!\n");
	}

	// FORKING:
	int pid[2];
	pid[0] = fork();
	if (pid[0] != 0) {
		// I am parent
		pid[1] = fork();
		if (pid[1] != 0) {
			// I am still parent
			if (pid[0] == -1 || pid[1] == -1) {
				fprintf(stderr, "Fork failed\n");
				close(fd_shared);
				unlink(NAME);
				return EXIT_FAILURE;
			}
			// waiting for all children to finish up
			while (wait(NULL) > 0)
				;

			validate_result(shared_mem->result, K, N);
		}

		if (pid[1] == 0) {
			// I am child B aka summing
			for (uint64_t i = 1; i <= K; i++) {
				sem_wait(&shared_mem->sem_fd_1);
				shared_mem->result += shared_mem->buffer[i % L];
				sem_post(&shared_mem->sem_fd_2);
			}
			printf("Result: %ld\n", shared_mem->result);
			exit(EXIT_SUCCESS);
		}

	} else {
		// I am child A aka populating the buffer
		for (uint64_t i = 1; i <= K; i++) {
			sem_wait(&shared_mem->sem_fd_2);
			shared_mem->buffer[i % L] = i * N;
			sem_post(&shared_mem->sem_fd_1);
		}
		exit(EXIT_SUCCESS);
	}

	// Clean up
	sem_destroy(&shared_mem->sem_fd_1);
	sem_destroy(&shared_mem->sem_fd_2);
	munmap(shared_mem, shared_memory_size);
	close(fd_shared);
	shm_unlink(NAME);
	return EXIT_SUCCESS;
}

void validate_result(uint64_t result, const uint64_t K, const uint64_t N) {
	for (uint64_t i = 1; i <= K; i++) {
		result -= N * i;
	}
	printf("Checksum: %ld \n", result);
}