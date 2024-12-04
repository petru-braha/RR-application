#define _GNU_SOURCE

#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "include/shared.h"

//------------------------------------------------
//! global variables:

// i need O(1) in looking into
// indexes are upd descriptors, values are tcp descriptors
const int NO_PAIR = 0;
int protocol_pair[100000];

const uint16_t port = 2970;
struct timeval TV = {1, 0};

const int ONE_CLIENT_ONLY = 1;
const int TWO_CLIENT_ONLY = 2;
const int COUNT_CLIENT_MAX = 100000;

typedef struct
{
  fd_set container;
  int count;
} rr_fd;

// reading sets
rr_fd tcp_set, udp_set;

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

void multiplexing(struct sockaddr_in *skadd_udp, socklen_t *len);

//------------------------------------------------
//! start function

// ++clients, no concurency
int main()
{
  // tcp first socket (don't block in accept)
  int sd_tcp =
      socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  call_var(sd_tcp);

  int opt = 1;
  call(setsockopt(sd_tcp, SOL_SOCKET,
                  SO_REUSEADDR, &opt, sizeof(opt)));
  opt++;

  struct sockaddr_in skadd_server;
  bzero(&skadd_server, sizeof(skadd_server));
  skadd_server.sin_family = AF_INET;
  skadd_server.sin_addr.s_addr = htonl(INADDR_ANY);
  skadd_server.sin_port = htons(port);

  call(bind(sd_tcp, (struct sockaddr *)&skadd_server,
            sizeof(struct sockaddr)));
  call(listen(sd_tcp, TWO_CLIENT_ONLY));

  // before loop
  struct sockaddr_in skadd_client;
  socklen_t length = sizeof(skadd_client);
  bzero(&skadd_client, length);

  // loop
  printf("the server is online.\n\n");
  for (; running_condition();)
  {
    multiplexing(&skadd_client, &length);

    // tcp socket
    bzero(&skadd_client, sizeof(skadd_client));
    int sd_client_tcp =
        accept4(sd_tcp, (struct sockaddr *)&skadd_client,
                &length, SOCK_NONBLOCK);
    if (-1 == sd_client_tcp)
      if (EWOULDBLOCK == errno || EAGAIN == errno)
      {
        errno = 0;
        continue;
      }

    /* only if accepted, udp socket
    int sd_client_udp =
        socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    call_var(sd_client_udp);
    call(setsockopt(sd_client_udp, SOL_SOCKET,
                    SO_REUSEADDR, &opt, sizeof(opt)));
    call(bind(sd_client_udp,
              (struct sockaddr *)&skadd_server,
              sizeof(struct sockaddr)));
    protocol_pair[sd_client_udp] = sd_client_tcp; */

    // sets
    FD_SET(sd_client_tcp, &tcp_set.container);
    if (sd_client_tcp > tcp_set.count)
      tcp_set.count = sd_client_tcp + 1;
    /*FD_SET(sd_client_udp, &udp_set.container);
    if (sd_client_udp > udp_set.count)
      udp_set.count = sd_client_udp + 1;*/
  }

  printf("the server is offline.\n");
  call(close(sd_tcp));
  return EXIT_SUCCESS;
}

//------------------------------------------------
//! functions inaccessible from main

void parse_command()
{
}

void build_outcome()
{
}

int send_outcome(char *command, int sd)
{
  // operations
  parse_command();
  build_outcome();
  char outcome[BYTES_OUTCOME_MAX];
  strcpy(outcome, "outcome for ");
  strcat(outcome, command);
  ssize_t bytes = write(sd, outcome, BYTES_OUTCOME_MAX);

  if (0 == strcmp(command, "quit"))
  {
    FD_CLR(sd, &tcp_set.container);
    call(close(sd));
  }
  
  return bytes;
}

void multiplexing(struct sockaddr_in *skadd_udp, socklen_t *len)
{
  char command[BYTES_COMMAND_MAX];

  /* udp commands
  fd_set udp_ccopy = udp_set.container;
  call(select(udp_set.count, &udp_ccopy, NULL, NULL, &TV));
  for (int fd = 0; fd < udp_set.count; fd++)
    if (FD_ISSET(fd, &udp_ccopy))
    {
      // bug: skadd_udp is always null here
      // bug: select() doesn't properly select
      call(recvfrom(fd, command, BYTES_COMMAND_MAX,
                    NO_FLAG, (struct sockaddr *)skadd_udp, len));
      call(send_outcome(command, protocol_pair[fd]));
      printf("debug command - udp:\"%s\" %d %d\n", command, fd, protocol_pair[fd]);
      printf("%d %d\n", FD_ISSET(5, &udp_set.container), FD_ISSET(7, &udp_set.container));

      // close client
      if (0 == strcmp(command, "quit"))
      {
        FD_CLR(protocol_pair[fd], &tcp_set.container);
        FD_CLR(fd, &udp_set.container);
        call(close(protocol_pair[fd]));
        call(close(fd));
        protocol_pair[fd] = NO_PAIR;
      }
    }*/

  /* tcp commands
  fd_set tcp_ccopy = tcp_set.container;
  call(select(tcp_set.count, &tcp_ccopy, NULL, NULL, &TV));
  for (int fd = 0; fd < tcp_set.count; fd++)
    if (FD_ISSET(fd, &tcp_ccopy))
    {
      call(read(fd, command, BYTES_COMMAND_MAX));
      call(send_outcome(command, fd));
      printf("debug command - tcp:\"%s\" %d\n", command, fd);
    }*/

  // receive commands
  fd_set tcp_ccopy = tcp_set.container;
  call(select(tcp_set.count, &tcp_ccopy, NULL, NULL, &TV));
  for (int fd = 0; fd < tcp_set.count; fd++)
    if (FD_ISSET(fd, &tcp_ccopy))
    {
      call(read(fd, command, BYTES_COMMAND_MAX));
      call(send_outcome(command, fd));
    }

  // send commands
}
