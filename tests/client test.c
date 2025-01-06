/* client.c - requests-sending application
 * author - Braha Petru Bogdan - <petrubraha@gmail.com> (c)
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "include/shared.h"
#include "include/printer.h"
#include "include/route.h"

#define UDP_ROUTES 250
#define UDP_DEPARTURES 251
#define UDP_ARRIVALS 252
#define TCP_REPORT 5
#define TCP_QUIT 6
#define RETRY 0

int sd_tcp, sd_udp;
struct sockaddr_in skadd_server;

//------------------------------------------------

static ssize_t recv_command(char *command);
ssize_t send_command(const char command);
ssize_t recv_outcome(const char command,
                     char *outcome);

//------------------------------------------------

static void exit_client(int status)
{
  call(close(sd_tcp));
  call(close(sd_udp));
  exit(status);
}

int main(int argc, char *argv[])
{
  // base case
  if (argc != 3)
  {
    // no call required it is already a failing case
    printf("syntax: <ip address> <port>.\n");
    return EXIT_FAILURE;
  }

  skadd_server.sin_family = AF_INET;
  skadd_server.sin_addr.s_addr = inet_addr(argv[1]);
  skadd_server.sin_port = htons(atoi(argv[2]));

  // tcp
  sd_tcp = socket(AF_INET, SOCK_STREAM, 0);
  call_var(sd_tcp);
  call(connect(sd_tcp, (struct sockaddr *)&skadd_server,
               sizeof(struct sockaddr)));
  // udp
  sd_udp = socket(AF_INET, SOCK_DGRAM, 0);
  if (-1 == sd_udp ||
      -1 == connect(sd_udp,
                    (struct sockaddr *)&skadd_server,
                    sizeof(struct sockaddr)))
  {
    call(close(sd_tcp));
    error("udp socket failed, the first one didn't");
    return EXIT_FAILURE;
  }

  // command loop
  call(printf("welcome, now you can send commands.\n\n"));
  char command;
  struct rr_route outcome[COUNT_ROUTES_MAX];
  for (int condition = 1; condition;)
  {
    call(recv_command(&command));
    send_command(command);
    recv_outcome(command, outcome);
    call(printf("%s\n", outcome));
    condition = strcmp(command, "quit");
  }

  exit_client(0);
}

//------------------------------------------------
//! other

unsigned char valid_command(char *const buffer)
{
  // repair
  size_t index = strlen(buffer) - 1;
  buffer[index] = '\0';

  if (0 == strcmp(buffer, "routes"))
    return UDP_ROUTES;
  if (0 == strcmp(buffer, "departures"))
    return UDP_DEPARTURES;
  if (0 == strcmp(buffer, "arrivals"))
    return UDP_ARRIVALS;
  if (0 == strcmp(buffer, "report"))
    return TCP_REPORT;
  if (0 == strcmp(buffer, "quit"))
    return TCP_QUIT;
  return RETRY;
}

static ssize_t recv_command(char *const command)
{
  char buffer[BYTES_COMMAND_MAX];
  ssize_t bytes = read_all(STDIN_FILENO, buffer,
                           BYTES_COMMAND_MAX);
  *command = valid_command(buffer);

  while (RETRY == *command)
  {
    bytes = read_all(STDIN_FILENO, buffer,
                     BYTES_COMMAND_MAX);
    *command = valid_command(buffer);
  }

  return bytes;
}

//------------------------------------------------
// sending

ssize_t send_tcp(const char *const command)
{
  ssize_t bytes = write_all(sd_tcp, command,
                            sizeof(char));

  if (errno || bytes < 1)
  {
    error("server disconnected while sending command");
    if (ECONNRESET != errno && errno)
      error(strerror(errno));
    exit_client(errno);
  }

  return bytes;
}

ssize_t send_command(const char command)
{
  // tcp communication if the client sends data
  if (TCP_REPORT == command || TCP_QUIT == command)
    return send_tcp(&command);
  
  // udp communication for queries
  return sendto(sd_udp, &command,
                sizeof(char), NO_FLAG,
                (struct sockaddr *)&skadd_server,
                sizeof(skadd_server));
}

//------------------------------------------------
// receiving

ssize_t recv_tcp(char *outcome)
{
  ssize_t bytes = read_all(sd_tcp, outcome,
                           BYTES_OUTCOME_MAX);

  if (errno || bytes < 1)
  {
    error("server disconnected while receiving output");
    if (ECONNRESET != errno && errno)
      error(strerror(errno));
    exit_client(errno);
  }

  return bytes;
}

ssize_t recv_outcome(const char command, char *outcome)
{
  bzero(outcome, BYTES_OUTCOME_MAX);

  if (0 == strcmp(command, "report"))
    return recv_tcp(outcome);
  if (0 == strcmp(command, "quit"))
    return recv_tcp(outcome);

  socklen_t length = sizeof(skadd_server);
  ssize_t bytes = recvfrom(sd_udp, outcome,
                           BYTES_OUTCOME_MAX, NO_FLAG,
                           (struct sockaddr *)&skadd_server,
                           &length);

  if (-1 == bytes)
  {
    error("server disconnected while receiving output");
    if (ECONNRESET != errno && errno)
      error(strerror(errno));
    exit_client(errno);
  }

  return bytes;
}
