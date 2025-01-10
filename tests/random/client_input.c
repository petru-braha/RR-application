
#include <stdio.h>
#include <unistd.h>
#include "../../include/error.h"

#define path_location "../../include/data/location.txt"
#include "../../include/client_cmd.h"

/* each command has at most two arguments
 * provides error messages
 */
static ssize_t
recv_command(unsigned char *const command,
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
        explicit_bzero(buffer, BYTES_COMMAND_MAX);
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

    return bytes;
}

// success
int main()
{
    unsigned char c = 0;
    unsigned short a = 0, b = 0;
    recv_command(&c, &a, &b);
    printf("%d %d %d.\n", c, a, b);
}
