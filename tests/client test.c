/* comments:
 * client.c - requests-sending application
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

const char *const path_location =
    "../include/data/location.txt";

//------------------------------------------------

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

  call(printf("welcome, you may type commands.\n\n"));
  unsigned char command = 0;
  unsigned short argument0 = 0, argument1 = 0;
  struct rr_route outcome[COUNT_ROUTES_MAX];

  // command loop
  for (; TCP_CODE_Q != command;)
  {
    recv_command(&command, &argument0, &argument1);
    send_command(command, argument0, argument1);
    recv_outcome(command, &argument0, outcome);
    print_data(command, argument0, outcome);
  }

  exit_client(EXIT_SUCCESS);
}

//------------------------------------------------
//! protocol parsing methods

#include "../include/client_cmd.h"

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

    unsigned char size =
        command_validation(buffer, command);
    bytes += size;

    if (TCP_CODE_Q == *command)
    {
      if ('\0' != *(buffer + 4))
      {
        warning("quit does not take arguments");
        continue;
      }

      *argument0 = *argument1 = 0;
      break;
    }

    if (TCP_CODE_Q != *command &&
        false == consume_input(&buffer, size))
    {
      warning("this command takes two arguments");
      continue;
    }
    if (RETRY_COMMAND == *command)
    {
      warning("invalid command");
      continue;
    }

    size = argument_validation0(buffer,
                                *command,
                                argument0);
    bytes += size;
    if (TCP_CODE_Q != *command &&
        false == consume_input(&buffer, size))
    {
      warning("this command takes two arguments");
      continue;
    }
    if (RETRY_ARGUMENT == *argument0)
    {
      warning("invalid first argument");
      continue;
    }

    size = argument_validation1(buffer,
                                *command,
                                *argument0,
                                argument1);
    bytes += size;
    buffer += size; // here we don't cosume, aviod warning

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
      printf("reporting failed.\n");
    else
      printf("reporting succeeded.\n");
    return sizeof(unsigned char);
  }

  if (TCP_CODE_Q == command)
  {
    if (RECV_FAIL == argument0)
      printf("quiting failed to fetch with server.\n");
    else
      printf("quiting succeeded.\n");
    return sizeof(unsigned char);
  }

  if (NULL == data && argument0)
  {
    error("print_data() failed - null data");
    return -1;
  }

  // UDP commands
  printf("%d routes received.\n", argument0);
  for (unsigned short i = 0; i < argument0; i++)
    client_print(data[i], path_location);
  return argument0 * sizeof(struct rr_route);
}
