/* client.c - requests-sending application
 * author - Braha Petru Bogdan - <petrubraha@gmail.com> (c)
 * bibliography - see README.md file
 */

#include <time.h>

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

#include "../../include/shared.h"
#include "../../include/printer.h"

int sd_tcp, sd_udp;
struct sockaddr_in skadd_server;

//------------------------------------------------

static ssize_t recv_command(char *command)
{
  bzero(command, BYTES_COMMAND_MAX);
  int bytes = read(STDIN_FILENO, command,
                   BYTES_COMMAND_MAX);

  // repair
  size_t index = strlen(command) - 1;
  command[index] = '\0';
  return bytes;
}

ssize_t send_command(char *command)
{
  // tcp communication if the client sends data
  if (0 == strcmp(command, "report"))
    return write(sd_tcp, command,
                 BYTES_COMMAND_MAX);

  if (0 == strcmp(command, "quit"))
    return write(sd_tcp, command,
                 BYTES_COMMAND_MAX);

  // udp communication for queries
  return sendto(sd_udp, command,
                BYTES_COMMAND_MAX, NO_FLAG,
                (struct sockaddr *)&skadd_server,
                sizeof(skadd_server));
}

// tcp return
ssize_t recv_outcome(char *outcome, char *command)
{
  bzero(outcome, BYTES_OUTCOME_MAX);

  if (0 == strcmp(command, "report"))
    return read(sd_tcp, outcome, BYTES_OUTCOME_MAX);
  if (0 == strcmp(command, "quit"))
    return read(sd_tcp, outcome, BYTES_OUTCOME_MAX);

  socklen_t length = sizeof(skadd_server);
  return recvfrom(sd_udp, outcome,
                  BYTES_OUTCOME_MAX, NO_FLAG,
                  (struct sockaddr *)&skadd_server,
                  &length);
}

//------------------------------------------------

int main()
{
  skadd_server.sin_family = AF_INET;
  skadd_server.sin_addr.s_addr = inet_addr("127.0.0.1");
  skadd_server.sin_port = htons(2970);

  // tcp
  sd_tcp = socket(AF_INET, SOCK_STREAM, 0);
  call_var(sd_tcp);
  call(connect(sd_tcp, (struct sockaddr *)&skadd_server,
               sizeof(struct sockaddr)));
  // udp
  sd_udp = socket(AF_INET, SOCK_DGRAM, 0);
  call_var(sd_udp);
  call(connect(sd_udp, (struct sockaddr *)&skadd_server,
               sizeof(struct sockaddr)));

  char command[BYTES_COMMAND_MAX];
  char outcome[BYTES_OUTCOME_MAX];

  strcpy(command, "report");
  call(send_command(command));

  // test area
  clock_t time = clock();
  call(recv_outcome(outcome, command));
  time = clock() - time;

  printf("cl0 finished with: %s in %ld.\n", outcome, time);

  call(close(sd_tcp));
  call(close(sd_udp));
  return EXIT_SUCCESS;
}
