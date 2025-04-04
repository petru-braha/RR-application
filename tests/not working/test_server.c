// a concurrent server with i/o multiplexing, and two transport protocols
// sends back only tcp

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

int sd_udp = -1;

//------------------------------------------------
//! methods

// a server should always be online
bool running_condition()
{
  int fd = open("../include/dev/key.txt", O_RDONLY);
  call_var(fd);

  char key = '0';
  call(read(fd, &key, 1));
  call(close(fd));

  return key != '0';
}

// a second main thread
void *multiplexing(void *);

// indexes are sockets
#define COUNT_CLIENTS 1024
struct sockaddr_in socket_to_address[COUNT_CLIENTS];
const int COUNT_RESERVED_PORTS = 1024;

typedef struct
{
  char *command;
  struct sockaddr_in skadd_client;
} udp_help;

int search_descriptor(struct sockaddr_in *skadd_client)
{
  for (int i = 0; i < COUNT_CLIENTS; i++)
  {
    if (socket_to_address[i].sin_addr.s_addr == skadd_client->sin_addr.s_addr)
      if (socket_to_address[i].sin_port == skadd_client->sin_port)
        return i;
  }

  return -1;
}

void *treat_udp(void *arg)
{
  pthread_detach(pthread_self());

  udp_help structure = *(udp_help *)arg;
  char *command = structure.command;

  char outcome[BYTES_OUTCOME_MAX];
  strcpy(outcome, "raspuns TCP la cerere UDP: ");
  strcat(outcome, command);

  int tcp_sd = search_descriptor(&structure.skadd_client) + 4;

  printf("debug: %s\n", command);
  printf("debug: %s:%u.\n", inet_ntoa(structure.skadd_client.sin_addr), ntohs(structure.skadd_client.sin_port));

  if (-1 + 4 == tcp_sd)
    printf("warning: received a command from strange location.\n");

  call(write(tcp_sd, outcome, BYTES_OUTCOME_MAX));
  return NULL;
}

void *udp_communication(void *)
{
  char command[BYTES_COMMAND_MAX];
  struct sockaddr_in skaddr_client;
  socklen_t length = sizeof(skaddr_client);

  for (; running_condition();)
  {
    int bytes = recvfrom(sd_udp, command, BYTES_COMMAND_MAX, NO_FLAG,
                         (struct sockaddr *)&skaddr_client, &length);
    if (-1 == bytes)
    {
      if (EWOULDBLOCK != errno)
        call_var(bytes);
      errno = 0;
      continue;
    }

    udp_help arg = {command, skaddr_client};
    pthread_t udp_thread;
    pthread_create(&udp_thread, NULL, &treat_udp, &arg);
  }
}

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

  // udp
  sd_udp = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
  call_var(sd_udp);
  call(bind(sd_udp, (struct sockaddr *)&skadd_server, sizeof(struct sockaddr)));

  // loops: i/o multiplexing and non-blocking accepts
  pthread_t udp_thread;
  call0(pthread_create(&udp_thread, NULL, &udp_communication, NULL));
  pthread_t multiplexing_thread;
  call0(pthread_create(&multiplexing_thread, NULL, &multiplexing, NULL));

  struct sockaddr_in skadd_client;
  socklen_t length = sizeof(skadd_client);
  memset(&skadd_client, 0, sizeof(skadd_client));

  call(printf("the server is online.\n\n"));
  for (; running_condition();)
  {
    // accepted client
    int sd_client = accept(sd_listen, (struct sockaddr *)&skadd_client, &length);
    call_noblock(sd_client);
    if (-1 == sd_client)
      continue;
    call(ioctl(sd_client, FIONBIO, &option));

    printf("connected: %s:%u.\n", inet_ntoa(skadd_client.sin_addr), ntohs(skadd_client.sin_port));

    // sets
    FD_SET(sd_client, &descriptors.container);
    if (sd_client >= descriptors.count)
      descriptors.count = sd_client + 1;

    memcpy(&socket_to_address[sd_client - 4], &skadd_client, sizeof(skadd_client));
  }

  // the server closes, an admin key was used
  call0(pthread_join(multiplexing_thread, NULL));
  call0(pthread_join(udp_thread, NULL));
  for (int fd = 0; fd < descriptors.count; fd++)
    if (FD_ISSET(fd, &descriptors.container))
      call(printf("warning: %d is not closed.\n", fd));

  call(close(sd_listen));
  call(close(sd_udp));
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

  char outcome[BYTES_OUTCOME_MAX];
  strcpy(outcome, "raspuns TCP la cerere TCP: ");
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
