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

#include "../include/computation.h"
#include "../include/error.h"
#include "../include/printer.h"
#include "../include/route.h"
#include "../include/shared.h"
#include "../include/route.h"

//------------------------------------------------

#define UDP_ROUTES 250
#define UDP_DEPARTURES 251
#define UDP_ARRIVALS 252
#define TCP_REPORT 5
#define TCP_QUIT 6
#define RETRY 0

#define RECV_FAIL 0

int sd_tcp, sd_udp;
struct sockaddr_in skadd_server;

//------------------------------------------------

static ssize_t recv_command(unsigned char *const command,
                            unsigned short *const argument0,
                            unsigned short *const argument1);
ssize_t send_command(const unsigned char command,
                     const unsigned short argument0,
                     const unsigned short argument1);
ssize_t recv_outcome(const unsigned char command,
                     unsigned short *const argument0,
                     struct rr_route *const data);
static ssize_t print_data(const unsigned char command,
                          const unsigned short argument0,
                          const struct rr_route *const data);

//------------------------------------------------

static void exit_client(const int status)
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

  call(printf("welcome, now you can send commands.\n\n"));
  unsigned char command = 0;
  unsigned short argument0 = 0, argument1 = 0;
  struct rr_route outcome[COUNT_ROUTES_MAX];

  // command loop
  for (int condition = 1; condition;)
  {
    recv_command(&command, &argument0, &argument1);
    send_command(command, argument0, argument1);
    recv_outcome(command, &argument0, outcome);
    print_data(command, argument0, outcome);
    condition = strcmp(command, "quit");
  }

  exit_client(EXIT_SUCCESS);
}

//------------------------------------------------
//! other

unsigned char command_validation(char *const string)
{
  // repair
  size_t index = strlen(string) - 1;
  string[index] = '\0';

  // copy
  char buffer[BYTES_COMMAND_MAX];
  strcpy(buffer, string);

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

unsigned short argument_validation()
{
}

// each command has at most two arguments
static ssize_t recv_command(unsigned char *const command,
                            unsigned short *const argument0,
                            unsigned short *const argument1)
{
  char string[BYTES_COMMAND_MAX];
  ssize_t bytes = read_all(STDIN_FILENO, string,
                           BYTES_COMMAND_MAX);
  *command = valid_command(string);

  while (RETRY == *command)
  {
    bytes = read_all(STDIN_FILENO, string,
                     BYTES_COMMAND_MAX);
    *command = valid_command(string);
  }

  return bytes;
}

//------------------------------------------------
// sending

// sends four bytes
ssize_t send_tcp(const unsigned char command,
                 const unsigned short argument0,
                 const unsigned short argument1)
{
  ssize_t bytes = write_all(sd_tcp, command,
                            sizeof(command));
  bytes += write_all(sd_tcp, argument0,
                     sizeof(argument0));
  bytes += write_all(sd_tcp,
                     (unsigned char)argument1,
                     sizeof(unsigned char));

  if (errno || bytes < 1)
  {
    error("server disconnected while sending command");
    if (ECONNRESET != errno && errno)
      error(strerror(errno));
    exit_client(errno);
  }

  return bytes;
}

ssize_t send_command(const unsigned char command,
                     const unsigned short argument0,
                     const unsigned short argument1)
{
  // tcp communication if the client sends data
  if (TCP_REPORT == command)
    return send_tcp(command, argument0, argument1);
  if (TCP_QUIT == command)
    return send_tcp(command, argument0, argument1);

  // udp communication for queries
  unsigned char a0 = (unsigned char)argument0,
                a1 = (unsigned char)argument1;
  unsigned char string[3] = {command, a0, a1};
  return sendto(sd_udp, &string,
                sizeof(string), NO_FLAG,
                (struct sockaddr *)&skadd_server,
                sizeof(skadd_server));
}

//------------------------------------------------
// receiving

// receives one byte
ssize_t recv_tcp(const unsigned char command,
                 unsigned short *const outcome)
{
  unsigned char temp = 0;
  ssize_t bytes = read_all(sd_tcp, &temp,
                           sizeof(temp));

  if (errno || bytes != 1)
  {
    error("server disconnected while receiving output");
    if (ECONNRESET != errno && errno)
      error(strerror(errno));
    exit_client(errno);
  }

  *outcome = (unsigned short)temp;
  return bytes;
}

ssize_t recv_outcome(const unsigned char command,
                     unsigned short *const argument0,
                     struct rr_route *const data)
{
  if (TCP_REPORT == command || TCP_QUIT == command)
    return recv_tcp(command, argument0);

  socklen_t length = sizeof(skadd_server);
  ssize_t bytes = recvfrom(sd_udp, argument0,
                           sizeof(unsigned short), NO_FLAG,
                           (struct sockaddr *)&skadd_server,
                           &length);
  bytes += recvfrom(sd_udp, data,
                    *argument0 * sizeof(struct rr_route),
                    NO_FLAG,
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

static ssize_t print_data(const unsigned char command,
                          const unsigned short argument0,
                          const struct rr_route *const data)
{
  if (TCP_REPORT == command)
  {
    if (RECV_FAIL == argument0)
      error("reporting failed");
    else
      printf("reporting was correctly performed");
    return sizeof(unsigned char);
  }

  if (TCP_QUIT == command)
  {
    if (RECV_FAIL == argument0)
      error("exiting failed to fetch with server");
    else
      printf("exiting was correctly performed");
    return sizeof(unsigned char);
  }

  // UDP commands
  printf("%d routes received:\n", argument0);
  for (unsigned short i = 0; i < argument0; i++)
    client_print(data[i]);
  return argument0 * sizeof(struct rr_route);
}
