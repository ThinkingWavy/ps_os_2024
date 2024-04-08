#define _POSIX_C_SOURCE
#define _BSD_SOURCE

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void handler(int signum) {
	switch (signum) {
		case (SIGINT): write(1, "The signal SIGINT was sent!\n", 29); break;
		case (SIGCONT): write(1, "The signal SIGCONT was sent!\n", 30); break;
		case (SIGKILL): write(1, "The signal SIGKILL was sent!\n", 30); break;
		case (SIGSTOP): write(1, "The signal SIGSTOP was sent!\n", 30); break;
		case (SIGUSR1): write(1, "The signal SIGUSR1 was sent!\n", 30); break;
		case (SIGUSR2): write(1, "The signal SIGUSR2 was sent!\n", 30); break;
	}
}

int main(void) {
	struct sigaction sig;
	memset(&sig, 0, sizeof(sig));

	sig.sa_handler = handler;
	if (sigaction(SIGINT, &sig, NULL) == -1) { // id: 2
		perror("Setting Signal handler for SIGINT failed");
	}
	if (sigaction(SIGCONT, &sig, NULL) == -1) { // id: 18
		perror("Setting Signal handler for SIGCONT failed");
	}
	if (sigaction(SIGSTOP, &sig, NULL) == -1) { // id: 19
		perror("Setting Signal handler for SIGSTOP failed");
	}
	if (sigaction(SIGKILL, &sig, NULL) == -1) { // id: 9 -> not catchable
		perror("Setting Signal handler for SIGKILL failed");
	}
	if (sigaction(SIGUSR1, &sig, NULL) == -1) { // user custom  signal
		perror("Setting Signal handler for SIGUSR1 failed");
	}
	if (sigaction(SIGUSR2, &sig, NULL) == -1) { // user custom signal
		perror("Setting Signal handler for SIGUSR2 failed");
	}

	while (1) {
		usleep(100);

		if () }
	return EXIT_SUCCESS;
}