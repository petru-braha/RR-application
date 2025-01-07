#ifndef _0COMMON0_
#define _0COMMON0_

#include <stdlib.h>
#include <sys/time.h>

#include "dev/tcp_communication.h"
#include "computation.h"
#include "error.h"
#include "printer.h"
#include "route.h"

//------------------------------------------------
// constants:

const int NO_FLAG = 0;
#define BYTES_COMMAND_MAX 40
#define BYTES_OUTCOME_MAX 100
#define BYTES_PATH_MAX 50

// const char path_database[] = "dev/users.txt";

//------------------------------------------------
// methods:

char *convert_line(const char *const line)
{
    int n = strlen(line);
    if ('\n' == line[n - 1])
        n--;
    if (n > 99)
        return NULL;

    char *buffer = (char *)malloc(100);
    buffer[0] = (n / 10) + '0';
    buffer[1] = (n % 10) + '0';
    buffer[2] = 0;
    strcat(buffer, line);
    buffer[2 + n] = 0;

    return buffer;
}

#endif
