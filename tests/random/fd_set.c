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

#include "../include/shared.h"

// i need O(1) in looking into
// indexes are upd descriptors, values are tcp descriptors
int protocol_pair[100000];

const uint16_t port0 = 2970;
struct timeval TV = {2, 0};

const int ONE_CLIENT_ONLY = 1;
const int COUNT_CLIENT_MAX = 100000;

typedef struct
{
  fd_set container;
  int count;
} rr_fd;

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

void parse_command();
void build_outcome();

// responsable to call parse_command() and build(outcome)
int send_outcome(char *command, int sd)
{
  // perform operations
  char outcome[BYTES_OUTCOME_MAX];
  strcpy(outcome, "outcome for ");
  strcat(outcome, command);
  return write(sd, outcome, BYTES_OUTCOME_MAX);
}

fd_set tcp_c;
/*
void treat_client(struct sockaddr *skadd_udp, socklen_t *len)
{
  char command[BYTES_COMMAND_MAX];

  // udp commands
  fd_set udp_ccopy = udp_c;
  call(select(udp_read_set.count, &udp_ccopy, NULL, NULL, &TV));
  for (int fd = 0; fd < udp_read_set.count; fd++)
    if (FD_ISSET(fd, &udp_ccopy))
    {
      call(recvfrom(fd, command, BYTES_COMMAND_MAX, NO_FLAG, skadd_udp, len));
      send_outcome(command, protocol_pair[fd]);

      // close client
      if (0 == strcmp(command, "quit"))
      {
        FD_CLR(protocol_pair[fd], &tcp_c);
        FD_CLR(fd, &udp_c);
        FD_CLR(fd, &udp_ccopy);
        close(protocol_pair[fd]);
        close(fd);
        protocol_pair[fd] = -1;
      }
    }

  // tcp commands
  fd_set tcp_ccopy = tcp_c;
  call(select(tcp_read_set.count, &tcp_ccopy, NULL, NULL, &TV));
  for (int fd = 0; fd < tcp_read_set.count; fd++)
    if (FD_ISSET(fd, &tcp_ccopy))
    {
      call(read(fd, command, BYTES_COMMAND_MAX));
      send_outcome(command, fd);
    }
}
*/

int main()
{
  const int nr = 1024 * 10 + 510; //? < 520

  for (size_t i = 0; i < nr; i++)
    FD_SET(i, &tcp_c);
  for (size_t i = 0; i < nr; i++)
    if (!FD_ISSET(i + 2, &tcp_c))
      printf("cacat\n");
  return EXIT_SUCCESS;
}
