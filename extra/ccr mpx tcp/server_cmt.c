// a concurrent server with i/o multiplexing, tcp
#define _GNU_SOURCE

#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "../include/shared.h"

//------------------------------------------------
//! global variables

// const int NO_PAIR = 0;
// int protocol_pair[100000];

extern int errno;

const uint16_t port = 2970;

const int ONE_CLIENT_ONLY = 1;
const int TWO_CLIENT_ONLY = 2;
const int COUNT_CLIENT_MAX = 1024;
// const int COUNT_CLIENT_MAX = 100000;

typedef struct
{
  fd_set container;
  int count;
} rr_fd;

struct timeval TV = {1, 0};
rr_fd descriptors;

//------------------------------------------------
//! methods

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

// a second main thread
void *multiplexing(void *);
void *multiplexing1(void *);

// main thread
int main()
{
  FD_ZERO(&descriptors.container);
  int sd_listen = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  call_var(sd_listen);
  int option = 1;
  call(setsockopt(sd_listen, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)));

  struct sockaddr_in skadd_server;
  skadd_server.sin_family = AF_INET;
  skadd_server.sin_addr.s_addr = htonl(INADDR_ANY);
  skadd_server.sin_port = htons(port);

  call(bind(sd_listen, (struct sockaddr *)&skadd_server, sizeof(struct sockaddr)));
  call(listen(sd_listen, COUNT_CLIENT_MAX));

  // struct sockaddr_in skadd_client;
  // int length = sizeof(skadd_client);

  // loops: i/o multiplexing and non-blocking accepts
  pthread_t multiplexing_thread;
  call0(pthread_create(&multiplexing_thread, NULL, &multiplexing, NULL));

  call(printf("the server is online.\n\n"));
  for (; running_condition();)
  {
    // int sd_client = accept(sd_listen, (struct sockaddr *)&skadd_client, &length);
    int sd_client = accept(sd_listen, NULL, NULL);
    call_noblock(sd_client);
    if (-1 == sd_client)
      continue;
    call(ioctl(sd_client, FIONBIO, &option));

    FD_SET(sd_client, &descriptors.container);
    if (sd_client >= descriptors.count)
      descriptors.count = sd_client + 1;
  }

  // the server closes, an admin key was used
  call0(pthread_join(multiplexing_thread, NULL));
  for (int fd = 0; fd < descriptors.count; fd++)
    if (FD_ISSET(fd, &descriptors.container))
    {
      // bad stuff
    }

  call(close(sd_listen));
  call(printf("the server is offline.\n\n"));
  return EXIT_SUCCESS;
}

//------------------------------------------------
//! other

void *serve_client(int sd)
{
  if (!FD_ISSET(sd, &descriptors.container))
    return NULL;

  char command[BYTES_COMMAND_MAX];
  call(read(sd, command, BYTES_COMMAND_MAX));
  // printf("debug: \"%s\" %d.\n", command, sd);

  char outcome[BYTES_OUTCOME_MAX];
  strcpy(outcome, "raspuns la ");
  strcat(outcome, command);
  call(write(sd, outcome, BYTES_OUTCOME_MAX));

  if (0 == strcmp(command, "quit"))
  {
    FD_CLR(sd, &descriptors.container);
    call(close(sd));
  }

  return NULL;
}

void *multiplexing(void *)
{
  for (; running_condition();)
  {
    fd_set tcp_fd;
    memcpy(&tcp_fd, &descriptors.container, sizeof(descriptors.container));

    int count_selected = select(descriptors.count, &tcp_fd, NULL, NULL, &TV);
    call_var(count_selected);
    for (int sd = 4; sd < descriptors.count && count_selected; sd++)
      if (FD_ISSET(sd, &tcp_fd))
      {
        serve_client(sd);
        count_selected--;
      }
  }

  return NULL;
}

//------------------------------------------------

void *serve_client1(void *fd)
{
  pthread_detach(pthread_self());

  int sd = *(int *)fd;
  if (!FD_ISSET(sd, &descriptors.container))
    return NULL;

  char command[BYTES_COMMAND_MAX];
  call(read(sd, command, BYTES_COMMAND_MAX));

  char outcome[BYTES_OUTCOME_MAX];
  strcpy(outcome, "raspuns la ");
  strcat(outcome, command);
  call(write(sd, outcome, BYTES_OUTCOME_MAX));

  if (0 == strcmp(command, "quit"))
  {
    FD_CLR(sd, &descriptors.container);
    call(close(sd));
  }

  return NULL;
}

void *multiplexing1(void *)
{
  for (; running_condition();)
  {
    fd_set tcp_fd = descriptors.container;
    select(descriptors.count, &tcp_fd, NULL, NULL, &TV);
    for (int sd = 4; sd < descriptors.count; sd++)
      if (FD_ISSET(sd, &tcp_fd))
      {
        pthread_t t;
        call0(pthread_create(&t, NULL, &serve_client1, &sd));
      }
  }

  return NULL;
}
