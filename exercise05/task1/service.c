#include <mqueue.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MSG_SIZE 1024
#define MAX_MSGS 10

int main(int argc, char* argv[]) {
	if (argc < 3) {
		fprintf(stderr, "Usage: echo '<numbers_to_sort> ...' | %s '/<queue_name>' <priority>", argv[0]);
		return EXIT_FAILURE;
	}

	int prio = atoi(argv[2]);
	char buffer[MSG_SIZE] = { 0 };
	printf("%ld", sysconf(_SC_MQ_PRIO_MAX));

	// scanf(%u,) -> secure, in contrast to gets
	if (gets(buffer) == NULL) {
		// Error reading input
		perror("Error reading input");
		return EXIT_FAILURE;
	}

	// Creating message queues
	struct mq_attr m_queue;
	m_queue.mq_msgsize = MSG_SIZE;
	m_queue.mq_maxmsg = MAX_MSGS;
	m_queue.mq_flags = 0;
	m_queue.mq_curmsgs = 0; // is ignored by mq_open
	mqd_t mq = mq_open(argv[1], O_WRONLY, 0200, &m_queue);
	if (mq == -1) {
		perror("Opening message queue failed");
		return EXIT_FAILURE;
	}

	if (mq_send(mq, buffer, sizeof(buffer), prio) == -1) {
		perror("MQ Send failed");
	}
	mq_unlink(argv[0]);
	printf("Message has been sent successfully\n");

	return EXIT_SUCCESS;
}
