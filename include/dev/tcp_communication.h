#ifndef _00TCP_C00_
#define _00TCP_C00_

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

ssize_t read_all(const int fd, char *buffer, const size_t count_bytes)
{
    if (0 == count_bytes || NULL == buffer)
        return -1;

    ssize_t bytes = 0;
    while (bytes != count_bytes)
    {
        ssize_t b = read(fd, buffer + bytes, count_bytes);

        if (-1 == b && EWOULDBLOCK == errno)
        {
            errno = 0;
            continue;
        }

        if (errno || b < 1) // error
            return b;

        bytes += b;
    }

    return bytes;
}

ssize_t write_all(const int fd, const char const *buffer, const size_t count_bytes)
{
    if (0 == count_bytes || NULL == buffer)
        return -1;

    ssize_t bytes = 0;
    while (bytes != count_bytes)
    {
        ssize_t b = write(fd, buffer + bytes, count_bytes);

        if (-1 == b && EWOULDBLOCK == errno)
        {
            errno = 0;
            continue;
        }

        if (errno || b < 1) // error
            return b;

        bytes += b;
    }

    return bytes;
}

#endif
