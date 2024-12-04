// concurrent server with no i/o multiplexing
#define _GNU_SOURCE

#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "include/shared.h"

// const int NO_PAIR = 0;
// int protocol_pair[100000];
// struct timeval TV = {1, 0};

extern int errno;

const uint16_t port = 2970;

const int ONE_CLIENT_ONLY = 1;
const int TWO_CLIENT_ONLY = 2;
const int COUNT_CLIENT_MAX = 1024;
// const int COUNT_CLIENT_MAX = 100000;

// a server should always be online
bool running_condition()
{
  int fd = open("include/dev/key.txt", O_RDONLY);
  call_var(fd);

  char key = '0';
  call(read(fd, &key, 1));
  call(close(fd));

  return key != '0';
}

void *launch_communication(void *);

//------------------------------------------------
//! start function

int main()
{
  struct sockaddr_in skadd_server;
  pthread_t th[COUNT_CLIENT_MAX];

  int sd_listen = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  call_var(sd_listen);

  // int option = 1;
  // setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

  skadd_server.sin_family = AF_INET;
  skadd_server.sin_addr.s_addr = htonl(INADDR_ANY);
  skadd_server.sin_port = htons(port);

  call(bind(sd_listen, (struct sockaddr *)&skadd_server, sizeof(struct sockaddr)) == -1);
  call(listen(sd_listen, COUNT_CLIENT_MAX));

  struct sockaddr_in skadd_client;
  // loop
  printf("the server is online.\n\n");
  for (; running_condition();)
  {
    int length = sizeof(skadd_client);

    int sd_client = accept(sd_listen, (struct sockaddr *)&skadd_client, &length);
    call_noblock(sd_client);
    if (-1 == sd_client)
      continue;

    pthread_t thread_read;
    call0(pthread_create(&thread_read, NULL, &launch_communication, &sd_client));
  }

  call(close(sd_listen));
  printf("the server is offline.\n\n");
  return EXIT_SUCCESS;
}

//------------------------------------------------
//! functions inaccessible from main

void client_communication(int sd)
{
  char command[BYTES_COMMAND_MAX];
  for (int condition = 1; condition;)
  {
    if (read(sd, command, BYTES_COMMAND_MAX) <= 0)
    {
      printf("[Thread %d]\n", sd - 4);
      perror("Eroare la read() de la client.\n");
    }

    printf("[Thread %d]Mesajul a fost receptionat...\n", sd - 4);

    char outcome[BYTES_OUTCOME_MAX];
    strcpy(outcome, "raspuns la ");
    strcat(outcome, command);
    if (write(sd, outcome, BYTES_OUTCOME_MAX) <= 0)
    {
      printf("[Thread %d] ", sd - 4);
      perror("[Thread]Eroare la write() catre client.\n");
    }
    else
      printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", sd - 4);

    condition = strcmp(command, "quit");
  }
}

void *launch_communication(void *arg)
{
  int sd = *(int *)arg;
  printf("[thread]- %d - Asteptam mesajul...\n", sd - 4);
  fflush(stdout);

  pthread_detach(pthread_self());
  client_communication(sd);
  close(sd);
  return NULL;
}
