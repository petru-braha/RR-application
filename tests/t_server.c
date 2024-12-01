#define _GNU_SOURCE
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "../include/shared.h"

// i need O(1) in looking into
// indexes are upd descriptors, values are tcp descriptors
int protocol_pair[100000];

const uint16_t port0 = 2970;
struct timeval TV = {2, 0};

const int ONE_CLIENT_ONLY = 1;
const int COUNT_CLIENT_MAX = 100000;

typedef struct
{
    fd_set container;
    int count;
} rr_fd;

// a server should always be online
bool running_condition()
{
    int fd = open("../include/dev/key.txt", O_RDONLY);
    call_var(fd);

    char key = '0';
    call(read(fd, &key, 1));
    call(close(fd));

    return key != '0';
}


// copies the sets, no need to do it mannually
static void recv_command(char *command, rr_fd tcp_read_set, rr_fd udp_read_set, void *skadd_udp, socklen_t *len)
{
  bzero(command, sizeof(command));
  call(select(tcp_read_set.count, &tcp_read_set.container, NULL, NULL, &TV));
  for (int fd = 0; fd < tcp_read_set.count; fd++)
    if (FD_ISSET(fd, &tcp_read_set.container))
    {
      call(read(fd, command, BYTES_COMMAND_MAX));
      return;
    }

  call(select(udp_read_set.count, &udp_read_set.container, NULL, NULL, &TV));
  for (int fd = 0; fd < udp_read_set.count; fd++)
    if (FD_ISSET(fd, &udp_read_set.container))
    {
      call(recvfrom(fd, command, BYTES_COMMAND_MAX, NO_FLAG, (struct sockaddr *)skadd_udp, len));
      return;
    }
}

int send_outcome(char *command, int sd)
{
    // perform operations
    char outcome[BYTES_OUTCOME_MAX];
    strcpy(outcome, "outcome for ");
    strcat(outcome, command);
    return write(sd, outcome, BYTES_OUTCOME_MAX);
}

// ++clients => no concurency/multiplexing
int main()
{
    for (size_t i = 0; i < COUNT_CLIENT_MAX; i++)
        protocol_pair[i] = -1;
    printf("the server is online.\n\n");

    // tcp first socket (don't block in accept)
    int sd_tcp =
        socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    call_var(sd_tcp);

    int opt = 1;
    call(setsockopt(sd_tcp, SOL_SOCKET,
                    SO_REUSEADDR, &opt, sizeof(opt)));

    struct sockaddr_in skadd_server;
    bzero(&skadd_server, sizeof(skadd_server));
    skadd_server.sin_family = AF_INET;
    skadd_server.sin_addr.s_addr = htonl(INADDR_ANY);
    skadd_server.sin_port = htons(port0);

    call(bind(sd_tcp, (struct sockaddr *)&skadd_server,
              sizeof(struct sockaddr)));
    call(listen(sd_tcp, ONE_CLIENT_ONLY));

    // before loop
    struct sockaddr_in skadd_client;
    socklen_t length = sizeof(skadd_client);
    bzero(&skadd_client, length);

    // container
    fd_set tcp_c;
    fd_set udp_c;
    FD_ZERO(&tcp_c);
    FD_ZERO(&udp_c);

    // loop
    for (; running_condition();)
    {
        printf("asteapta sa vina cineva.\n");

        // tcp
        int sd_client_tcp =
            accept4(sd_tcp, (struct sockaddr *)&skadd_client,
                   &length, SOCK_NONBLOCK);
        if (-1 == sd_client_tcp)
            if (EWOULDBLOCK == errno || EAGAIN == errno)
            {
                errno = 0;
                continue;
            }

        // only if accepted
        // container
        FD_SET(sd_client_tcp, &tcp_c);

        // rr_fd
        rr_fd tcp_read_set = {tcp_c, sd_client_tcp + 1};
        rr_fd udp_read_set = {udp_c, 1};

        // read commands
        char command[BYTES_COMMAND_MAX];

        int optt = 2;
        // setsockopt(sd_client_tcp, SOL_SOCKET, SOCK_NONBLOCK, &opt, 4);
        // setsockopt(sd_client_tcp, SOL_SOCKET, O_NONBLOCK, &opt, 4);
        // fcntl(sd_client_tcp, F_SETFD, SOCK_NONBLOCK);
        // fcntl(sd_client_tcp, F_SETFD, fcntl(sd_client_tcp, F_GETFD, 0) | O_NONBLOCK);
        int bytes = -1;
        while (-1 == bytes)
        {
            printf("asteapta sa scrie cineva.\n");
            bytes = read(sd_client_tcp, command, BYTES_COMMAND_MAX);
            if (errno != EWOULDBLOCK && errno != EACCES)
            {
                printf("crapa!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
                exit(1);
            }
        }

        // delete association between client's sockets
        // protocol_pair[sd_client_udp] = -1;
        FD_CLR(sd_client_tcp, &tcp_c);
        // FD_CLR(sd_client_udp, &udp_c);
        call(close(sd_client_tcp));
        // call(close(sd_client_udp));
        break;
    }

    printf("the server is offline.\n");
    call(close(sd_tcp));
    return EXIT_SUCCESS;
}

// alternates between tcp and udp
// connects to just one client => no concurency/multiplexing
int maain()
{
    printf("the server is online.\n\n");

    // tcp first socket
    int sd_tcp = 0;
    sd_tcp = socket(AF_INET, SOCK_STREAM, 0);
    call_var(sd_tcp);

    int opt = 1;
    call(setsockopt(sd_tcp, SOL_SOCKET,
                    SO_REUSEADDR, &opt, sizeof(opt)));

    struct sockaddr_in skadd_server;
    bzero(&skadd_server, sizeof(skadd_server));
    skadd_server.sin_family = AF_INET;
    skadd_server.sin_addr.s_addr = htonl(INADDR_ANY);
    skadd_server.sin_port = htons(port0);

    call(bind(sd_tcp, (struct sockaddr *)&skadd_server,
              sizeof(struct sockaddr)));
    call(listen(sd_tcp, ONE_CLIENT_ONLY));

    // loop
    struct sockaddr_in skadd_client;
    socklen_t length = sizeof(skadd_client);
    bzero(&skadd_client, length);

    for (; running_condition();)
    {
        // tcp
        int sd_client_tcp =
            accept(sd_tcp, (struct sockaddr *)&skadd_client,
                   &length);
        call_var(sd_client_tcp);

        // udp
        int sd_client_udp = socket(AF_INET, SOCK_DGRAM, 0);
        call_var(sd_client_udp);
        call(bind(sd_client_udp,
                  (struct sockaddr *)&skadd_server,
                  sizeof(struct sockaddr)));

        // container
        fd_set tcp_c;
        fd_set udp_c;
        FD_ZERO(&tcp_c);
        FD_ZERO(&udp_c);
        FD_SET(sd_client_tcp, &tcp_c);
        FD_SET(sd_client_udp, &udp_c);

        // rr_fd
        rr_fd tcp_read_set = {tcp_c, sd_client_tcp + 1};
        rr_fd udp_read_set = {udp_c, sd_client_udp + 1};

        // read commands
        char command[BYTES_COMMAND_MAX];
        /*
          for (int condition = 1; condition;)
          {
            recv_command(command, tcp_read_set, udp_read_set, &skadd_client, &length);
            if (0 == command[0])
              continue;
            call(printf("debug: \"%s\".\n", command));
            call(send_outcome(command, sd_client_tcp));
            condition = strcmp(command, "quit");
          }
          */
        recvfrom(sd_client_udp, command, BYTES_COMMAND_MAX, NO_FLAG, (struct sockaddr *)&skadd_client, &length);
        call(printf("debug: \"%s\".\n", command));
        call(send_outcome(command, sd_client_tcp));
        recvfrom(sd_client_udp, command, BYTES_COMMAND_MAX, NO_FLAG, (struct sockaddr *)&skadd_client, &length);
        call(printf("debug: \"%s\".\n", command));
        call(send_outcome(command, sd_client_tcp));

        call(close(sd_client_tcp));
        call(close(sd_client_udp));
    }

    printf("the server is offline.\n");
    call(close(sd_tcp));
    return EXIT_SUCCESS;
}
