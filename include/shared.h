#ifndef _0COMMON0_
#define _0COMMON0_

#include <stdlib.h>

//------------------------------------------------
// constants:

#define READ 0
#define WRIT 1

const char path_fifo_client_server[] = "dev/fifo/client-server";
const char path_fifo_server_client[] = "dev/fifo/server-client";
const char path_database[] = "dev/users.txt";

#define MAX_COMMAND_SIZE 64
#define MAX_OUTPUT_SIZE 99
#define MAX_STRING_SIZE 32
#define MAX_PREFIX_SIZE 2

#define MAX MAX_OUTPUT_SIZE

//------------------------------------------------
// methods:

char* convert_line(const char* const line)
{
    int n = strlen(line);
    if('\n' == line[n - 1])
        n--;
    if(n > 99)
        return NULL;
    
    char* buffer = (char*)malloc(MAX);
    buffer[0] = (n / 10) + '0';
    buffer[1] = (n % 10) + '0';
    buffer[2] = 0;
    strcat(buffer, line);
    buffer[2 + n] = 0;

    return buffer;
}

//------------------------------------------------
// error:

#define print_function_name(x) printf("\"%s\"", #x);

#define call(x) if(x < 0) { \
    printf("line %d error: ", __LINE__); print_function_name(x); \
    printf(" failed - %s.\n", strerror(errno)); \
    exit(EXIT_FAILURE); \
}

#define call_var(x) if(x < 0) { \
    printf("line %d error - %s\n", __LINE__, strerror(errno)); \
    exit(EXIT_FAILURE); \
}

#define call_noexit(x) if(x < 0) { \
    printf("line %d error: ", __LINE__); print_function_name(x); \
    printf(" failed - %s.\n", strerror(errno)); \
}

#endif