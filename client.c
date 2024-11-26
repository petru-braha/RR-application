#include "dev/apps.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

#include "include/printer.h"

const size_t BYTES_COMMAND_MAX = 20;
const size_t BYTES_OUTCOME_MAX = 100;

int main(int argc, char *argv[])
{
  // base case
  if (argc != 3)
  {
    printf("syntax: <ip address> <port>.\n");
    return EXIT_FAILURE;
  }

  // ux
  printf("welcome dear client.\n");

  int port = atoi(argv[2]);
  int sd = socket(AF_INET, SOCK_STREAM, 0);
  call_var(sd);

  struct sockaddr_in skadd_server;
  skadd_server.sin_family = AF_INET;
  skadd_server.sin_addr.s_addr = inet_addr(argv[1]);
  skadd_server.sin_port = htons(port);

  call(connect(sd, &skadd_server, sizeof(struct sockaddr)));

  char command[BYTES_COMMAND_MAX];
  char outcome[BYTES_OUTCOME_MAX];
  bzero(command, BYTES_COMMAND_MAX);
  bzero(command, BYTES_COMMAND_MAX);
  call(read(STDIN_FILENO, command, BYTES_COMMAND_MAX));

  for (; command[0] != 'q';)
  {
    call(write(sd, command, BYTES_COMMAND_MAX));
    call(read(sd, outcome, BYTES_OUTCOME_MAX));
    call(read(STDIN_FILENO, command, BYTES_COMMAND_MAX));
  }

  close(sd);
  return EXIT_SUCCESS;
}
