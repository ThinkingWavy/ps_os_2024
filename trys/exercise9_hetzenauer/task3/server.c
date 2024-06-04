// MADE IN TEAMWORK BY ANNA-LENA HETZENAUER AND ISABELLA SCHMUT
#include <errno.h>
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

#define MSG_BUFFER_SIZE 128
#define CLIENT_NUM 32

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int sd = 0;
int active_clients = 0;

struct listener_info {
  int *sfd;
  int *cfd;
};

struct client_info {
  int *cfd;
  pthread_t *listener_thread;
  struct client_info *all_clients;
  char *client_name;
};

static void cleanup_handler() {
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&mutex);
}

void *listener_func(void *arg) {
  struct listener_info *listener = (struct listener_info *)arg;
  int *cfd = listener->cfd;
  int *sfd = listener->sfd;

  for (int i = 0; i < CLIENT_NUM; i++) {

    pthread_mutex_lock(&mutex);
    pthread_cleanup_push(cleanup_handler, NULL);

    if ((cfd[i] = accept(*sfd, NULL, NULL)) < 0) {
      perror("accept failed");
      exit(1);
    }
    pthread_cleanup_pop(1);
    if (pthread_cond_signal(&cond) != 0) {
      fprintf(stdout, "pthread_cond_signal\n");
      exit(1);
    }
    pthread_mutex_unlock(&mutex);
    sleep(1);
  }
  pthread_exit(NULL);
}

void get_message(const char *str, char sep, char data[2][128]) {
  int x = 0;
  unsigned int start = 0, stop;
  for (stop = 0;; stop++) {
    if (str[stop] == sep) {

      if (x) {
        sprintf(data[x - 1], "%.*s", stop - start, str + start);
      }
      start = stop + 1;
      if (++x == 2)
        break;
    }
  }
  stop = strlen(str);
  sprintf(data[x - 1], "%.*s", stop - start, str + start);
}

void *client_func(void *arg) {
  char in_buffer[128];
  char msg[265];
  active_clients++;
  struct client_info *client = (struct client_info *)arg;

  int *cfd = client->cfd;
  struct client_info *all_clients = client->all_clients;

  if (read(*cfd, client->client_name, MSG_BUFFER_SIZE) < 0) {
    perror("read failed");
    exit(1);
  }

  while (1) {

    bzero(in_buffer, MSG_BUFFER_SIZE);

    if ((read(*cfd, in_buffer, MSG_BUFFER_SIZE)) < 0) {
      perror("read failed");
      exit(1);
    }

    if (strcmp(in_buffer, "/quit\n") == 0) {
      printf("%s disconnected\n", client->client_name);
      active_clients--;
      close(*cfd);
      *cfd = 0;
      break;
    }

    if (strcmp(in_buffer, "/shutdown\n") == 0) {
      printf("%s disconnected\n", client->client_name);
      active_clients--;
      close(*cfd);
      *cfd = 0;
      sd = 1;
      pthread_cancel(*client->listener_thread);
      fprintf(
          stdout,
          "Server is shutting down. Waiting for %d clients to disconnect.\n",
          active_clients);
      break;
    }
    char copy[3] = " ";
    strncpy(copy, in_buffer, 2);
    if (!strcmp(copy, "/w")) {
      char tmp[2][128];
      get_message(in_buffer, ' ', tmp);
      sprintf(msg, "%s (whispers): %s", client->client_name, tmp[1]);
      for (int i = 0; i < CLIENT_NUM; i++) {
        if (!strcmp(all_clients[i].client_name, tmp[0])) {
          write(*all_clients[i].cfd, msg, 2 * MSG_BUFFER_SIZE);
        }
      }
    } else {
      sprintf(msg, "%s: %s", client->client_name, in_buffer);

      for (int i = 0; i < CLIENT_NUM; i++) {
        if ((*all_clients[i].cfd != 0) &&
            (*all_clients[i].cfd != *client->cfd)) {
          write(*all_clients[i].cfd, msg, 2 * MSG_BUFFER_SIZE);
        }
      }
    }
  }
  pthread_exit(NULL);
}

int main(int argc, char **argv) {

  if (argc < 2) {
    perror("Please enter the port");
    exit(1);
  }

  int port = strtol(argv[1], NULL, 10);

  pthread_t listener_thread;
  struct listener_info listener_info;
  pthread_t client_thread[CLIENT_NUM];
  struct client_info client_info[CLIENT_NUM];
  char client_names[CLIENT_NUM][MSG_BUFFER_SIZE];

  int *cfd = malloc(sizeof(int) * CLIENT_NUM);
  int *sfd = malloc(sizeof(int));

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY); // on server

  *sfd = socket(AF_INET, SOCK_STREAM, 0);

  if (*sfd < 0) {
    perror("socket() failed");
    exit(1);
  }

  // to prevent EADDRINUSE
  int addr_ru = 1;
  if (setsockopt(*sfd, SOL_SOCKET, SO_REUSEADDR, (void *)&addr_ru,
                 sizeof(addr_ru)) < 0) {
    perror("setsockopt");
    exit(1);
  }

  if (bind(*sfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind() failed");
    exit(1);
  }

  if (listen(*sfd, 1) < 0) {
    perror("listen failed");
    exit(1);
  }

  listener_info.sfd = sfd;
  listener_info.cfd = cfd;

  for (int i = 0; i < CLIENT_NUM; i++) {
    client_info[i].cfd = &cfd[i];
    client_info[i].all_clients = client_info;
    client_info[i].listener_thread = &listener_thread;
    client_info[i].client_name = client_names[i];
  }

  if (pthread_create(&listener_thread, NULL, listener_func,
                     (void *)&listener_info) != 0) {
    fprintf(stderr, "pthread_create");
    exit(0);
  }

  for (int i = 0; i < CLIENT_NUM; i++) {
    pthread_mutex_lock(&mutex);
    while (cfd[i] == 0 && sd == 0) {
      pthread_cond_wait(&cond, &mutex);
    }
    if (sd == 1) {
      sd = i;
      break;
    }

    if (pthread_create(&client_thread[i], NULL, client_func,
                       (void *)&client_info[i]) != 0) {
      fprintf(stderr, "pthread_create");
      exit(0);
    }

    pthread_mutex_unlock(&mutex);
  }
  if (pthread_join(listener_thread, NULL) != 0) {
    fprintf(stderr, "pthread_join");
    exit(0);
  }
  for (int i = 0; i < sd; i++) {
    if (pthread_join(client_thread[i], NULL) != 0) {
      fprintf(stderr, "pthread_join");
      exit(0);
    }
  }

  close(*sfd);
  free(sfd);
  free(cfd);
  exit(0);
}
