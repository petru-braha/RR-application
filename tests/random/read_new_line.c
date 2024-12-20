#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "../../include/shared.h"

int main()
{
    char command[BYTES_COMMAND_MAX];
    ssize_t bytes = read(STDIN_FILENO, command, BYTES_COMMAND_MAX);
    printf("received %d bytes with \"%s\".\n", bytes, command);
    return EXIT_SUCCESS;
}
