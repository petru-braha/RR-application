/* comments:
 * client.c - requests-sending application
 * author - Braha Petru Bogdan - <petrubraha@gmail.com> (c)
 * compilation command: gcc -std=c11 client.c -o cl
 * run command example: ./cl 127.0.0.1 2970
 * run command example: ./cl 10.100.0.30 2970
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

#include "include/communication.h"
#include "include/computation.h"
#include "include/error.h"
#include "include/printer.h"
#include "include/route.h"
#include "include/shared.h"

#define path_location "include/data/location.txt"

int sd_tcp, sd_udp;
struct sockaddr_in skadd_server;

//------------------------------------------------

static void exit_client(const int status);

// four procedures
static ssize_t recv_command(
    unsigned char *const command,
    unsigned short *const argument0,
    unsigned short *const argument1);

ssize_t send_command(
    const unsigned char command,
    const unsigned short argument0,
    const unsigned short argument1);

ssize_t recv_outcome(
    const unsigned char command,
    unsigned short *const argument0,
    struct rr_route *const data);

static int print_data(
    const unsigned char command,
    const unsigned short argument0,
    const struct rr_route *const data);

//------------------------------------------------

int main(int argc, char *argv[])
{
  // base case
  if (3 != argc)
  {
    warning("syntax: <ip address> <port>");
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
  if (ERR_CODE == sd_udp ||
      ERR_CODE == connect(
                      sd_udp,
                      (struct sockaddr *)&skadd_server,
                      sizeof(struct sockaddr)))
  {
    close(sd_tcp);
    error("socket() failed - tcp success, udp failure");
    return EXIT_FAILURE;
  }

  call(printf("welcome.\n\n"));
  unsigned char command = 0;
  unsigned short argument0 = 0, argument1 = 0;
  struct rr_route outcome[COUNT_ROUTES_MAX];

  // command loop
  for (; TCP_CODE_Q != command;)
  {
    // no call procedures - errors are checked inside
    recv_command(&command, &argument0, &argument1);
    send_command(command, argument0, argument1);
    recv_outcome(command, &argument0, outcome);
    call(print_data(command, argument0, outcome));
  }

  exit_client(EXIT_SUCCESS);
}

//------------------------------------------------
//! protocol parsing methods

#include "include/client_cmd.h"

/* each command has at most two arguments
 * provides error messages
 */
static ssize_t recv_command(
    unsigned char *const command,
    unsigned short *const argument0,
    unsigned short *const argument1)
{
  ssize_t bytes = 0;
  char buffer[BYTES_COMMAND_MAX];
  for (*command = RETRY_COMMAND,
      *argument0 = RETRY_ARGUMENT,
      *argument1 = RETRY_ARGUMENT;

       RETRY_COMMAND == *command ||
       RETRY_ARGUMENT == *argument0 ||
       RETRY_ARGUMENT == *argument1;)
  {
    memset(buffer, 0, BYTES_COMMAND_MAX);
    char *string = buffer;
    printf("your command: ");
    fflush(stdout);
    call(read(STDIN_FILENO, string, BYTES_COMMAND_MAX));
    repair(string);
    bytes = command_validation(string, command);
    if (RETRY_COMMAND == *command)
    {
      warning("invalid command");
      continue;
    }
    if (TCP_CODE_Q == *command)
      break;

    // command accepted, check argument0
    string = buffer + bytes;
    printf("your argument0: ");
    fflush(stdout);
    call(read(STDIN_FILENO, string, BYTES_COMMAND_MAX));
    repair(string);
    bytes += argument_validation(string, *command, argument0);
    if (RETRY_ARGUMENT == *argument0)
    {
      warning("invalid first argument");
      continue;
    }
    if (UDP_CODE_D == *command || UDP_CODE_A == *command)
      break;

    // argument0 accepted, check argument1
    string = buffer + bytes;
    printf("your argument1: ");
    fflush(stdout);
    call(read(STDIN_FILENO, string, BYTES_COMMAND_MAX));
    repair(string);
    bytes += argument_validation(string, *command, argument1);
    if ((UDP_CODE_R == *command && *argument0 == *argument1) ||
        (TCP_CODE_R == *command && 0 == *argument1))
      *argument1 = RETRY_ARGUMENT;
    if (RETRY_ARGUMENT == *argument1)
      warning("invalid second argument");
  }

  printf("\n");
  return bytes;
}

//------------------------------------------------
// sending

// sends four bytes
ssize_t send_tcp(
    const unsigned char command,
    const unsigned short argument0,
    const unsigned char argument1)
{
  ssize_t bytes = write_all(sd_tcp, &command,
                            sizeof(command));
  bytes += write_all(sd_tcp, &argument0,
                     sizeof(argument0));
  bytes += write_all(sd_tcp, &argument1,
                     sizeof(argument1));

  if (errno || 4 != bytes)
  {
    error("send_tcp() failed - server disconnected");
    if (ECONNRESET != errno && errno)
      error(strerror(errno));
    exit_client(errno);
  }

  return bytes;
}

ssize_t send_command(
    const unsigned char command,
    const unsigned short argument0,
    const unsigned short argument1)
{
  // tcp communication if the client sends data
  if (TCP_CODE_R == command || TCP_CODE_Q == command)
    return send_tcp(command, argument0,
                    (unsigned char)argument1);

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

  if (errno || sizeof(temp) != bytes)
  {
    error("recv_tcp() failed - server disconnected");
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
                           sizeof(*argument0), NO_FLAG,
                           (struct sockaddr *)&skadd_server,
                           &length);
  bytes += recvfrom(sd_udp, data,
                    *argument0 * sizeof(struct rr_route),
                    NO_FLAG,
                    (struct sockaddr *)&skadd_server,
                    &length);

  if (ERR_CODE == bytes)
  {
    error("recv_udp() failed - server disconnected");
    if (ECONNRESET != errno && errno)
      error(strerror(errno));
    exit_client(errno);
  }

  return bytes;
}

static int print_data(
    const unsigned char command,
    const unsigned short argument0,
    const struct rr_route *const data)
{
  if (TCP_CODE_R == command)
  {
    if (RECV_FAIL == argument0)
      return printf("reporting failed.\n\n");
    return printf("reporting succeeded.\n\n");
  }

  if (TCP_CODE_Q == command)
  {
    if (RECV_FAIL == argument0)
      return printf("quiting failed to fetch with server.\n\n");
    return printf("quiting succeeded.\n\n");
  }

  if (NULL == data && argument0)
    return ERR_CODE;

  printf("%d routes received.\n", argument0);
  for (unsigned short i = 0; i < argument0; i++)
    client_print(data[i], path_location);
  printf("\n");
  return argument0 * sizeof(struct rr_route);
}

//------------------------------------------------

static void exit_client(const int status)
{
  if (ERR_CODE == close(sd_tcp) ||
      ERR_CODE == close(sd_udp))
    error("close() failed");
  exit(status);
}
