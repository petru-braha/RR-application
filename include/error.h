#ifndef _00ERROR00_
#define _00ERROR00_

#include <stdio.h>

#define warning(message) printf("warning: %s.\n", message)
#define error(message) printf("line %d error: %s.\n", __LINE__, message)

#define print_function_name(x) printf("\"%s\"", #x)

#define call(x)                                     \
    if (x < 0)                                      \
    {                                               \
        printf("line %d error: ", __LINE__);        \
        print_function_name(x);                     \
        printf(" failed - %s.\n", strerror(errno)); \
        exit(EXIT_FAILURE);                         \
    }

#define call0(x)                                    \
    if (x != 0)                                     \
    {                                               \
        printf("line %d error: ", __LINE__);        \
        print_function_name(x);                     \
        printf(" failed - %s.\n", strerror(errno)); \
        exit(EXIT_FAILURE);                         \
    }

#define call_var(x)                                                \
    if (x < 0)                                                     \
    {                                                              \
        printf("line %d error: %s.\n", __LINE__, strerror(errno)); \
        exit(EXIT_FAILURE);                                        \
    }

#define call_noexit(x)                              \
    if (x < 0)                                      \
    {                                               \
        printf("line %d error: ", __LINE__);        \
        print_function_name(x);                     \
        printf(" failed - %s.\n", strerror(errno)); \
        errno = 0;                                  \
    }

#define call_noblock(x)                                 \
    if (x < 0)                                          \
    {                                                   \
        if (EWOULDBLOCK != errno)                       \
        {                                               \
            printf("line %d error: ", __LINE__);        \
            print_function_name(x);                     \
            printf(" failed - %s.\n", strerror(errno)); \
            exit(EXIT_FAILURE);                         \
        }                                               \
        errno = 0;                                      \
    }

#endif
