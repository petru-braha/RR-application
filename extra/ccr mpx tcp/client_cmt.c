#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "include/shared.h"
#include "include/printer.h"

int sd_tcp, sd_udp;
struct sockaddr_in skadd_server;

static int recv_command(char *command)
{
  bzero(command, BYTES_COMMAND_MAX);
  int bytes = read(STDIN_FILENO, command, BYTES_COMMAND_MAX);

  // repair
  size_t index = strlen(command) - 1;
  command[index] = '\0';
  return bytes;
}

int send_command(char *command)
{
  int bytes = 0;

  // tcp communication if the client sends data
  // if (0 == strcmp(command, "report"))
  {
    bytes = write(sd_tcp, command, BYTES_COMMAND_MAX);
    return bytes;
  }

  // udp communication for queries
  bytes = sendto(sd_udp, command, BYTES_COMMAND_MAX, NO_FLAG,
                 (struct sockaddr *)&skadd_server,
                 sizeof(skadd_server));

  return bytes;
}

// tcp return
int recv_outcome(char *outcome)
{
  bzero(outcome, BYTES_OUTCOME_MAX);
  return read(sd_tcp, outcome, BYTES_OUTCOME_MAX);
}

// ./client 127.0.0.1 2970
int main(int argc, char *argv[])
{
  // base case
  if (argc != 3)
  {
    // no call required it is already a failing case
    printf("syntax: <ip address> <port>.\n");
    return EXIT_FAILURE;
  }

  // tcp and udp protocols initializations
  uint16_t port = atoi(argv[2]);
  skadd_server.sin_family = AF_INET;
  skadd_server.sin_addr.s_addr = inet_addr(argv[1]);
  skadd_server.sin_port = htons(port);

  // tcp
  sd_tcp = socket(AF_INET, SOCK_STREAM, 0);
  call_var(sd_tcp);
  call(connect(sd_tcp, (struct sockaddr *)&skadd_server,
               sizeof(struct sockaddr)));
  /* udp
  sd_udp = socket(AF_INET, SOCK_DGRAM, 0);
  call_var(sd_udp);
  call(connect(sd_udp, (struct sockaddr *)&skadd_server,
               sizeof(struct sockaddr)));*/

  // ux
  call(printf("welcome dear client.\n"));
  call(printf("please type in your queries.\n\n"));

  char command[BYTES_COMMAND_MAX];
  char outcome[BYTES_OUTCOME_MAX];
  for (int condition = 1; condition;)
  {
    call(recv_command(command));
    call(send_command(command));
    call(recv_outcome(outcome));
    call(printf("%s\n", outcome));
    condition = strcmp(command, "quit");
  }

  call(close(sd_tcp));
  // call(close(sd_udp));
  return EXIT_SUCCESS;
}
