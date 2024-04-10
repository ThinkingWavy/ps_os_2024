#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define USAGE \
	"Please use the adder as follows: ./task3b <file_with_numbers1> ... <file_with_numbersN>\n"

#define MAX_NUM 1000

typedef struct thread_args {
	int num_thread;
	int sum;
	char name[];
} thread_args_t;

void* thread_add(void* args) {
	thread_args_t* structure = (thread_args_t*)args;

	// Open the file for reading
	FILE* file = fopen(structure->name, "r");
	if (file == NULL) {
		fprintf(stderr, "Error opening file %s\n", structure->name);
		pthread_exit(NULL);
	}

	printf("%s: #%d, sum=%d\n", structure->name, structure->num_thread, structure->sum);
	// Read each line and add numbers
	int num;
	structure->sum = 0; // Initialize sum to 0
	while (fscanf(file, "%d", &num) == 1) {
		structure->sum += num;
	}

	// Close the file
	fclose(file);

	pthread_exit(NULL);
}

int main(int argc, char const* argv[]) {
	if (argc < 2 || argc > 100) {
		fprintf(stderr, USAGE);
		return EXIT_FAILURE;
	}

	int N = argc - 1;
	thread_args_t args[N];
	pthread_t tid[N];

	/*Initialising structs and creating threads*/
	for (int i = 0; i < N; i++) {
		args[i].num_thread = i;
		args[i].sum = 0;
		strcpy(args[i].name, argv[i + 1]);
		if (pthread_create(&tid[i], NULL, &thread_add, (void*)&args[i])) {
			perror("Error while creating new threads!");
			return EXIT_FAILURE;
		}
	}

	/*Joing threads and printing products*/
	for (int i = 0; i < N; i++) {
		if (pthread_join(tid[i], NULL)) {
			perror("Error while joining threads!");
			return EXIT_FAILURE;
		}
		/* Somehow we didn't get the solution below to work. The product should get written back
		into the struct and to my knowledge should be accessible from out of the thread again. As
		far as I understood the solution in the link below that is how it can be done.
		https://stackoverflow.com/questions/2251452/how-to-return-a-value-from-pthread-threads-in-c
		*/
		printf("sum %d = %d\n", i + 1, args[i].sum);
	}

	return EXIT_SUCCESS;
}