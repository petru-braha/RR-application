
#include <stdio.h>
#include <unistd.h>
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

int main()
{
    unsigned char c = 0;
    unsigned short a = 0, b = 0;
    recv_command(&c, &a, &b);
}

/* treated:
 * wrong number of arguments
 * wrong argument types (numbers)
 * invalid arguments (non-existant in database)

todo:\
- wrong number of spaces ?
- report 1 ' ' - should print number of arg
*/
