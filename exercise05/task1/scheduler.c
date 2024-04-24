#include <ctype.h>
#include <mqueue.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// #ifndef _POSIX_SOURCE
// 	#define _POSIX_SOURCE
// #endif
#define MSG_SIZE 1024
#define MAX_MSGS 10
#define MESSAGE_SIZE_ZERO_ERROR -2
#define NOT_A_NUMBER_ERROR -3

bool should_run = true;

typedef struct mesg_buffer {
	long numbers[MSG_SIZE];
	char mesg_text[MSG_SIZE];
} message;

static void handler() {
	should_run = false;
}

// Comparator function for qsort
int compare(const void* a, const void* b) {
	return (*(int*)a - *(int*)b);
}

int parse_numbers(void* data) {
	message* input = (message*)data;
	int count = 0;
	char* token = strtok(input->mesg_text, " ");
	while (token != NULL) {
		bool isValid = true;
		for (size_t i = 0; i < strlen(token); i++) {
			if (!isdigit(token[i]) && token[i] != '-') {
				isValid = 0;
				break;
			}
		}
		if (isValid) {
			input->numbers[count++] = atol(token);
		} else {
			printf("\nInvalid input: '%s' is not a number.\n", token);
			return NOT_A_NUMBER_ERROR;
		}
		token = strtok(NULL, " ");
	}

	if (count == 0) {
		printf("\nInvalid input: No numbers entered!\n");
		return MESSAGE_SIZE_ZERO_ERROR;
	}

	return count;
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s /<msq_name> \n", argv[0]);
		return EXIT_FAILURE;
	}

	// Creating singal handler for interuption
	struct sigaction sig;
	memset(&sig, 0, sizeof(sig));
	sig.sa_handler = handler;
	if (sigaction(SIGINT, &sig, NULL) == -1) {
		perror("Setting Signal handler for SIGINT failed");
	}

	// Creating message queue
	struct mq_attr m_queue;
	m_queue.mq_msgsize = MSG_SIZE;
	m_queue.mq_flags = 0;
	m_queue.mq_maxmsg = MAX_MSGS;
	m_queue.mq_curmsgs = 0; // is ignored by mq_open
	mqd_t mq = mq_open(argv[1], O_CREAT | O_RDONLY, 0644, &m_queue);
	if (mq == -1) {
		perror("Opening message queue failed");
		return EXIT_FAILURE;
	}

	printf("\n\nMQ Opening succeeded: Waiting for messages\n");
	printf("__________________________________________\n");
	message* buffer = malloc(sizeof(message));
	for (int i = 0; i < MSG_SIZE; i++) {
		buffer->numbers[i] = 0;
		buffer->mesg_text[i] = '\0';
	}

	// Polling for messages
	while (should_run) {
		unsigned int prio;
		if (mq_receive(mq, buffer->mesg_text, sizeof(buffer->mesg_text), &prio) == -1) {
			perror("mq_receive");
			continue;
		}

		// Performing required task
		printf("\nScheduling task with priority:%d\n", prio);
		fflush(stdout);
		int count = parse_numbers(buffer);

		if (count == MESSAGE_SIZE_ZERO_ERROR || count == NOT_A_NUMBER_ERROR) {
			printf("Task failed: Message does not contain all numbers!\n");
		} else {
			qsort(buffer->numbers, count, sizeof(long), compare);
			for (int i = 0; i < count; i++) {
				printf("\rSorting progress: %d%%", (100 * (i + 1) / count));
				fflush(stdout);
				// TO ENHANCE: use nanosleep (usleep is deprecated)
				usleep(500000);
			}
			printf("\nSorted numbers: ");
			for (int i = 0; i < count; i++) {
				printf("%ld ", buffer->numbers[i]);
			}
			printf("\n");
		}
		// Clean buffer
		for (int i = 0; i < MSG_SIZE; i++) {
			buffer->numbers[i] = 0;
			buffer->mesg_text[i] = '\0';
		}
	}

	// cleanup
	free(buffer);
	mq_close(mq);
	mq_unlink(argv[0]);
	printf("Clean up successful, shutting down\n");

	return EXIT_SUCCESS;
}
