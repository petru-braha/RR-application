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

#define LEN_ROUTES 6
#define LEN_DEPARTURES 10
#define LEN_ARRIVALS 8
#define LEN_REPORT 6
#define LEN_QUIT 4

#define UDP_STRING_R "routes"
#define UDP_STRING_D "departures"
#define UDP_STRING_A "arrivals"
#define TCP_STRING_R "report"
#define TCP_STRING_Q "quit"

#define UDP_CODE_R 250
#define UDP_CODE_D 251
#define UDP_CODE_A 252
#define TCP_CODE_R 5
#define TCP_CODE_Q 6
#define RETRY_COMMAND 0
#define RETRY_ARGUMENT COUNT_ROUTES_MAX

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
    condition = strcmp(command, TCP_STRING_Q);
  }

  exit_client(EXIT_SUCCESS);
}

//------------------------------------------------
//! protocol parsing methods

// DOES NOT provides error messages
unsigned char command_validation(unsigned char *buffer)
{
  if (buffer == strstr(buffer, UDP_STRING_R))
  {
    strcpy(buffer, buffer + LEN_ROUTES);
    return UDP_CODE_R;
  }

  if (buffer == strstr(buffer, UDP_STRING_D))
  {
    strcpy(buffer, buffer + LEN_DEPARTURES);
    return UDP_CODE_D;
  }

  if (buffer == strstr(buffer, UDP_STRING_A))
  {
    strcpy(buffer, buffer + LEN_ARRIVALS);
    return UDP_CODE_A;
  }

  if (buffer == strstr(buffer, TCP_STRING_R))
  {
    strcpy(buffer, buffer + LEN_REPORT);
    return TCP_CODE_R;
  }

  if (buffer == strstr(buffer, TCP_STRING_Q))
  {
    strcpy(buffer, buffer + LEN_QUIT);
    return TCP_CODE_Q;
  }

  return RETRY_COMMAND;
}

// DOES NOT provides error messages
unsigned short argument_validation(unsigned char *buffer,
                                   const unsigned char command)
{
  if (RETRY_COMMAND == command)
    return RETRY_ARGUMENT;

  unsigned char *pointer = strchr(buffer, ' ');
  unsigned char old_value = *pointer;
  *pointer = '\0';
  int number = atoi(buffer);

  *pointer = old_value;

  if (command >= UDP_CODE_R)
  {
    int number = atoi(buffer);
    // strcpy
    // return

    if (0) // fail
      return
  }

  if (TCP_CODE_Q == command)
  {
    if (buffer[4] != '\0')
    {
      warning("quit does not take arguments");
      return RETRY_ARGUMENT;
    }

    return TCP_CODE_Q;
  }

  return RETRY_ARGUMENT;
}

/* each command has at most two arguments
 * provides error messages
 */
static ssize_t recv_command(unsigned char *const command,
                            unsigned short *const argument0,
                            unsigned short *const argument1)
{
  *command = RETRY_COMMAND;

  ssize_t bytes = 0;
  char line[BYTES_COMMAND_MAX];
  for (; RETRY_COMMAND == *command ||
         COUNT_ROUTES_MAX == *argument0 ||
         COUNT_ROUTES_MAX == *argument1;)
  {
    bytes = read_all(STDIN_FILENO, line, sizeof(line));
    if (BYTES_COMMAND_MAX != bytes)
    {
      error("read_all() failed - standard input");
      continue;
    }

    *command = command_validation(line);
    if (RETRY_COMMAND == *command)
    {
      warning("invalid command");
      continue;
    }

    *argument0 = argument_validation(line, command);
    if (RETRY_ARGUMENT == *argument0)
    {
      warning("invalid first argument");
      continue;
    }

    *argument1 = argument_validation(line, command);
    if (RETRY_ARGUMENT == *argument0)
      warning("invalid second argument");
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
  if (TCP_CODE_R == command)
    return send_tcp(command, argument0, argument1);
  if (TCP_CODE_Q == command)
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
  if (TCP_CODE_R == command || TCP_CODE_Q == command)
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
  if (TCP_CODE_R == command)
  {
    if (RECV_FAIL == argument0)
      error("reporting failed");
    else
      printf("reporting was correctly performed");
    return sizeof(unsigned char);
  }

  if (TCP_CODE_Q == command)
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
