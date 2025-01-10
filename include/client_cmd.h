#ifndef _00CMD00_
#define _00CMD00_

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "printer.h"
#include "route.h"
#include "shared.h"

#define RETRY_COMMAND 0                 // invalid commands
#define RETRY_ARGUMENT COUNT_ROUTES_MAX // invalid argument

/* comments:
 * this file contains functions to validate input
 * command inputs from the client application
 */

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
            return 0;

        // success
        *argument0 = number;
        return condition - buffer;
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
    if (NULL == buffer || '\0' == buffer[0])
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
            return 0;
        if (0 == number)
        {
            warning("invalid report");
            return 0;
        }
        if (number >= UCHAR_MAX)
        {
            warning("a train can be at most 180 minutes late");
            return 0;
        }

        *argument1 = number; // success
        return condition - buffer;
    }

    // failure
    return 0;
}

#endif
