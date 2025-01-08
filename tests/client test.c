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

#include "../include/communication.h"
#include "../include/computation.h"
#include "../include/error.h"
#include "../include/printer.h"
#include "../include/route.h"
#include "../include/shared.h"

//------------------------------------------------

const char *const path_location =
    "../include/data/location.txt";

#define RETRY_COMMAND 0                 // invalid commands
#define RETRY_ARGUMENT COUNT_ROUTES_MAX // invalid argument
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
    error("socket() failed - tcp success, udp failure");
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
    condition = TCP_CODE_Q == command;
  }

  exit_client(EXIT_SUCCESS);
}

//------------------------------------------------
//! protocol parsing methods

/* to be applied only for:
 * the command and first argument
 * DOES NOT provides error messages
 */
static bool consume_input(char **const buffer,
                          const unsigned char size)
{
  *buffer += size;
  if ('\0' == (*buffer)[0])
    return false;
  (*buffer)++;
  return true;
}

/* returns how should the pointer change
 * DOES NOT provides error messages
 */
static unsigned char
command_validation(const char *const buffer,
                   unsigned char *const command)
{
  if (buffer == strstr(buffer, UDP_STRING_R))
  {
    *command = UDP_CODE_R;
    return LEN_ROUTES;
  }

  if (buffer == strstr(buffer, UDP_STRING_D))
  {
    *command = UDP_CODE_D;
    return LEN_DEPARTURES;
  }

  if (buffer == strstr(buffer, UDP_STRING_A))
  {
    *command = UDP_CODE_A;
    return LEN_ARRIVALS;
  }

  if (buffer == strstr(buffer, TCP_STRING_R))
  {
    *command = TCP_CODE_R;
    return LEN_REPORT;
  }

  if (buffer == strstr(buffer, TCP_STRING_Q))
  {
    *command = TCP_CODE_Q;
    return LEN_QUIT;
  }

  *command = RETRY_COMMAND;
  return 0;
}

/* returns how should the pointer change
 * DOES NOT provides error messages
 */
static unsigned char
argument_validation0(const char *const buffer,
                     const unsigned char command,
                     unsigned short *const argument0)
{
  *argument0 = RETRY_ARGUMENT;
  if (NULL == buffer)
  {
    warning("received null buffer");
    return 0;
  }

  if (RETRY_COMMAND == command)
    return 0;

  if (command >= UDP_CODE_R)
  {
    unsigned short location_code = RETRY_ARGUMENT;
    unsigned short bytes =
        stringtocode(buffer, &location_code,
                     path_location);
    if (COUNT_LOCATION == location_code)
      return 0;

    // success
    *argument0 = location_code;
    return bytes;
  }

  if (TCP_CODE_R == command)
  {
    char *condition = NULL;
    int number_base = 10;
    unsigned long number =
        strtoul(buffer, &condition, number_base);
    if (' ' != *condition)
    {
      warning("strtoul() failed");
      return 0;
    }

    // success
    *argument0 = number;
    return condition - buffer;
  }

  if (TCP_CODE_Q == command)
  {
    if ('\0' != buffer[4])
      warning("quit does not take arguments");
    else // success
      *argument0 = TCP_CODE_Q;
    return 0;
  }

  // failure
  return 0;
}

/* returns how should the pointer change
 * DOES NOT provides error messages
 */
static unsigned char
argument_validation1(const char *const buffer,
                     const unsigned char command,
                     const unsigned short argument0,
                     unsigned short *const argument1)
{
  *argument1 = RETRY_ARGUMENT;
  if (NULL == buffer)
  {
    warning("received null buffer");
    return 0;
  }

  if (RETRY_COMMAND == command ||
      RETRY_ARGUMENT == argument0)
    return 0;

  if (command >= UDP_CODE_R)
  {
    unsigned short location_code = RETRY_ARGUMENT;
    unsigned short bytes =
        stringtocode(buffer, &location_code,
                     path_location);
    if (COUNT_LOCATION == location_code)
      return 0;

    if (location_code == argument0)
    {
      warning("there are no trains with no use");
      return 0;
    }

    *argument1 = location_code; // success
    return bytes;
  }

  if (TCP_CODE_R == command)
  {
    char *condition = NULL;
    int number_base = 10;
    unsigned long number =
        strtoul(buffer, &condition, number_base);
    if ('\0' != *condition)
    {
      warning("strtoul() failed");
      return 0;
    }

    *argument1 = number; // success
    return condition - buffer;
  }

  if (TCP_CODE_Q == command)
  {
    if ('\0' != buffer[4])
      warning("quit() does not take arguments");
    else // success
      *argument1 = TCP_CODE_Q;
    return 0;
  }

  // failure
  return 0;
}

/* each command has at most two arguments
 * provides error messages
 */
static ssize_t
recv_command(unsigned char *const command,
             unsigned short *const argument0,
             unsigned short *const argument1)
{
  ssize_t bytes = 0;
  char *const line = malloc(BYTES_COMMAND_MAX);
  *command = RETRY_COMMAND;

  for (*command = RETRY_COMMAND,
      *argument0 = RETRY_ARGUMENT,
      *argument1 = RETRY_ARGUMENT;

       RETRY_COMMAND == *command ||
       RETRY_ARGUMENT == *argument0 ||
       RETRY_ARGUMENT == *argument1;)
  {
    explicit_bzero(line, BYTES_COMMAND_MAX);
    char *buffer = line;
    bytes = read(STDIN_FILENO, line, BYTES_COMMAND_MAX);
    repair(line);
    if (false ==
        consume_input(&buffer,
                      command_validation(buffer,
                                         command)))
    {
      warning("this command takes two arguments");
      continue;
    }
    if (RETRY_COMMAND == *command)
    {
      warning("invalid command");
      continue;
    }

    if (false ==
        consume_input(&buffer,
                      argument_validation0(buffer,
                                           *command,
                                           argument0)))
    {
      warning("this command takes two arguments");
      continue;
    }
    if (RETRY_ARGUMENT == *argument0)
    {
      warning("invalid first argument");
      continue;
    }

    // here we don't cosume, aviod warning
    buffer += argument_validation1(buffer,
                                   *command,
                                   *argument0,
                                   argument1);
    if (RETRY_ARGUMENT == *argument1 || '\0' != buffer[0])
      warning("invalid second argument");
  }

  free(line);
  return bytes;
}

//------------------------------------------------
// sending

// sends four bytes
ssize_t send_tcp(const unsigned char command,
                 const unsigned short argument0,
                 const unsigned short argument1)
{
  ssize_t bytes = write_all(sd_tcp, &command,
                            sizeof(command));
  bytes += write_all(sd_tcp, &argument0,
                     sizeof(argument0));
  bytes += write_all(sd_tcp,
                     (unsigned char *)&argument1,
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
  if (TCP_CODE_R == command || TCP_CODE_Q == command)
    return send_tcp(command, argument0, argument1);

  // udp communication if queries
  // sends three bytes
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
    client_print(data[i], path_location);
  return argument0 * sizeof(struct rr_route);
}
