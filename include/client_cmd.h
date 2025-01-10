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

/* returns how should the pointer change
 * provides error messages
 */
static unsigned char
command_validation(const char *const buffer,
                   unsigned char *const command)
{
    if (NULL == buffer || NULL == command)
    {
        warning("received null argument");
        NULL != command ? *command = RETRY_COMMAND : 0 ;
        return 0;
    }

    if (0 == strcmp(buffer, UDP_STRING_R))
    {
        *command = UDP_CODE_R;
        return LEN_ROUTES;
    }

    if (0 == strcmp(buffer, UDP_STRING_D))
    {
        *command = UDP_CODE_D;
        return LEN_DEPARTURES;
    }

    if (0 == strcmp(buffer, UDP_STRING_A))
    {
        *command = UDP_CODE_A;
        return LEN_ARRIVALS;
    }

    if (0 == strcmp(buffer, TCP_STRING_R))
    {
        *command = TCP_CODE_R;
        return LEN_REPORT;
    }

    if (0 == strcmp(buffer, TCP_STRING_Q))
    {
        *command = TCP_CODE_Q;
        return LEN_QUIT;
    }

    *command = RETRY_COMMAND;
    return 0;
}

/* returns how should the pointer change
 * provides error messages
 */
static unsigned char
argument_validation(const char *const buffer,
                    const unsigned char command,
                    unsigned short *const argument)
{
    if (RETRY_COMMAND == command)
        return 0;
    if (NULL == buffer || NULL == argument)
    {
        warning("received null argument");
        return 0;
    }

    *argument = RETRY_ARGUMENT;
    if (command >= UDP_CODE_R)
    {
        unsigned short location_code = 0;
        unsigned short bytes =
            stringtocode(buffer, &location_code,
                         path_location);
        if (location_code >= COUNT_LOCATION)
            return 0;

        // success
        *argument = location_code;
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

        // success
        *argument = number;
        return condition - buffer;
    }

    // failure
    *argument = RETRY_ARGUMENT;
    return 0;
}

#endif
