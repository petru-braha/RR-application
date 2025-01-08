#include "../../include/printer.h"
#include "../../include/shared.h"

const char *const path_location =
    "../../include/data/location.txt";

#define RETRY_COMMAND 0                 // invalid commands
#define RETRY_ARGUMENT COUNT_ROUTES_MAX // invalid argument
#define RECV_FAIL 0

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
    // + 1 skips the ' ' character
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

        // success
        *argument1 = location_code;
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

        // success
        *argument1 = number;
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

int main()
{
    unsigned char c = 0;
    unsigned short a = 0, b = 0;
    recv_command(&c, &a, &b);
}
