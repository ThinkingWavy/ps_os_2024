// MADE IN TEAMWORK BY ANNA-LENA HETZENAUER AND ISABELLA SCHMUT
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#define SERVER_PATH "/tmp/server"
#define BUFFER_LENGTH 128

struct write_info {
  pthread_t *read_thread;
  int *cfd;
};

void *read_func(void *arg) {
  int *sfd = (int *)arg;
  char in_buffer[BUFFER_LENGTH];
  while (1) {

    bzero(in_buffer, BUFFER_LENGTH);

    if ((read(*sfd, in_buffer, BUFFER_LENGTH)) < 0) {
      perror("read failed");
      exit(1);
    }

    printf("%s", in_buffer);
  }
  pthread_exit(NULL);
}

void *write_func(void *arg) {
  struct write_info *writer = (struct write_info *)arg;
  int *sfd = writer->cfd;
  char buffer[BUFFER_LENGTH];
  while (1) {
    bzero(buffer, sizeof(buffer));
    fgets(buffer, BUFFER_LENGTH, stdin);
    if (write(*sfd, buffer, BUFFER_LENGTH) < 0) {
      perror("write failed");
      exit(1);
    }
    if (strcmp(buffer, "/quit\n") == 0 || strcmp(buffer, "/shutdown\n") == 0) {
      fprintf(stdout, "done\n");
      pthread_cancel(*writer->read_thread);

      break;
    }
  }
  pthread_exit(NULL);
}

int main(int argc, char **argv) {

  if (argc < 3) {
    perror("Please enter the port");
    exit(1);
  }

  int port = strtol(argv[1], NULL, 10);

  pthread_t write_thread;
  pthread_t read_thread;
  struct write_info writer;

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // on client

  int sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd < 0) {
    perror("socket() failed");
    exit(1);
  }

  // setting address space to 0
  if (connect(sfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("connect() failed");
    exit(1);
  }

  if (write(sfd, argv[2], BUFFER_LENGTH) < 0) {
    perror("write failed");
    exit(1);
  }
  writer.cfd = &sfd;
  writer.read_thread = &read_thread;

  if (pthread_create(&read_thread, NULL, read_func, (void *)&sfd) != 0) {
    fprintf(stderr, "pthread_create");
    exit(0);
  }

  if (pthread_create(&write_thread, NULL, write_func, (void *)&writer) != 0) {
    fprintf(stderr, "pthread_create");
    exit(0);
  }

  if (pthread_join(read_thread, NULL) != 0) {
    fprintf(stderr, "pthread_join");
    exit(0);
  }

  if (pthread_join(write_thread, NULL) != 0) {
    fprintf(stderr, "pthread_join");
    exit(0);
  }

  if (sfd != -1)
    close(sfd);
}