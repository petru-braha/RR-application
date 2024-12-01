#include <unistd.h>
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

#include "include/shared.h"

const uint16_t port0 = 2970;
struct timeval TV = {2, 0};

const int ONE_CLIENT_ONLY = 1;

typedef struct
{
  fd_set container;
  int count;
} rr_fd;

// copies the sets, no need to do it mannually
static void recv_command(char *command, rr_fd tcp_read_set, rr_fd udp_read_set, void *skadd_udp, socklen_t *len)
{
  bzero(command, sizeof(command));
  call(select(tcp_read_set.count, &tcp_read_set.container, NULL, NULL, &TV));
  for (int fd = 0; fd < tcp_read_set.count; fd++)
    if (FD_ISSET(fd, &tcp_read_set.container))
    {
      call(read(fd, command, BYTES_COMMAND_MAX));
      return;
    }

  call(select(udp_read_set.count, &udp_read_set.container, NULL, NULL, &TV));
  for (int fd = 0; fd < udp_read_set.count; fd++)
    if (FD_ISSET(fd, &udp_read_set.container))
    {
      call(recvfrom(fd, command, BYTES_COMMAND_MAX, NO_FLAG, (struct sockaddr *)skadd_udp, len));
      return;
    }
}

int send_outcome(char *command, int sd)
{
  // perform operations
  char outcome[BYTES_OUTCOME_MAX];
  strcpy(outcome, "outcome for ");
  strcat(outcome, command);
  return write(sd, outcome, BYTES_OUTCOME_MAX);
}

// alternates between tcp and udp
// connects to just one client => no concurency/multiplexing
int main()
{
  printf("the server is online.\n\n");

  // tcp first socket
  int sd_tcp = 0;
  sd_tcp = socket(AF_INET, SOCK_STREAM, 0);
  call_var(sd_tcp);

  int opt = 1;
  call(setsockopt(sd_tcp, SOL_SOCKET,
                  SO_REUSEADDR, &opt, sizeof(opt)));

  struct sockaddr_in skadd_server;
  bzero(&skadd_server, sizeof(skadd_server));
  skadd_server.sin_family = AF_INET;
  skadd_server.sin_addr.s_addr = htonl(INADDR_ANY);
  skadd_server.sin_port = htons(port0);

  call(bind(sd_tcp, (struct sockaddr *)&skadd_server,
            sizeof(struct sockaddr)));
  call(listen(sd_tcp, ONE_CLIENT_ONLY));

  // loop
  struct sockaddr_in skadd_client;
  socklen_t length = sizeof(skadd_client);
  bzero(&skadd_client, length);

  // tcp
  int sd_client_tcp =
      accept(sd_tcp, (struct sockaddr *)&skadd_client,
             &length);
  call_var(sd_client_tcp);

  // udp
  int sd_client_udp = socket(AF_INET, SOCK_DGRAM, 0);
  call_var(sd_client_udp);
  call(bind(sd_client_udp,
            (struct sockaddr *)&skadd_server,
            sizeof(struct sockaddr)));

  // container
  fd_set tcp_c;
  fd_set udp_c;
  FD_ZERO(&tcp_c);
  FD_ZERO(&udp_c);
  FD_SET(sd_client_tcp, &tcp_c);
  FD_SET(sd_client_udp, &udp_c);

  // rr_fd
  rr_fd tcp_read_set = {tcp_c, sd_client_tcp + 1};
  rr_fd udp_read_set = {udp_c, sd_client_udp + 1};

  // read commands
  char command[BYTES_COMMAND_MAX];

  for (int condition = 1; condition;)
  {
    recv_command(command, tcp_read_set, udp_read_set, &skadd_client, &length);
    if (0 == command[0])
      continue;
    call(printf("debug: \"%s\".\n", command));
    call(send_outcome(command, sd_client_tcp));
    condition = strcmp(command, "quit");
  }

  call(close(sd_client_tcp));
  call(close(sd_client_udp));
  call(close(sd_tcp));

  printf("the server is offline.\n");
  return EXIT_SUCCESS;
}

/* questions:
0. sizeof(struct sockaddr) / sizeof(skadd_server) ?
1. order problem of commands

- notes:
- for each client set up sd_tcp and sd_udp
- port0 is for the first socket of tcp
- port1 is for the sockets of udp
2. multiple clients will need those ports => SO_REUSE_ADDRESS

*/