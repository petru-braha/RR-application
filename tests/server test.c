/* comments:
 * server.c - a concurrent server with i/o multiplexing, and two transport protocols
 * author - Braha Petru Bogdan - <petrubraha@gmail.com> (c)
 * compilation command: gcc server\ test.c -I/usr/include/libxml2 -lxml2 -o sv
 */

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

#include "../include/communication.h"
#include "../include/error.h"
#include "../include/printer.h"
#include "../include/route.h"
#include "../include/server_xml.h"
#include "../include/server_api.h"

#define path_thread "../include/dev/key.txt"
#define path_binary "../include/dev/write_xml"
#define name_random "random schedule.xml"
#define path_to_build "../include/data/"

typedef struct
{
    fd_set container;
    int count;
} rr_fd;
rr_fd descriptors;
int sd_udp;

//------------------------------------------------

void check_xml(int *argc, char *argv[],
               char *const path_xml);
bool running_condition();
int maintenance(pthread_t *th0, pthread_t *th1,
                const int argc,
                const char *const path_xml);

// three threads
void *udp_communication(void *);
void *multiplexing(void *);
int main(int argc, char *argv[]);

//------------------------------------------------

int main(int argc, char *argv[])
{
    char path_xml[BYTES_PATH_MAX];
    strcpy(path_xml, path_to_build);

    check_xml(&argc, argv, path_xml);
    read_xml(path_xml);

    // server address
    const uint16_t port = 2970;
    struct sockaddr_in skadd_server;
    skadd_server.sin_family = AF_INET;
    skadd_server.sin_addr.s_addr = htonl(INADDR_ANY);
    skadd_server.sin_port = htons(port);

    FD_ZERO(&descriptors.container);

    // tcp
    const int ONE_CLIENT_ONLY = 1;
    const int TWO_CLIENT_ONLY = 2;
    const int COUNT_CLIENT_MAX = 1024;

    int sd_listen =
        socket(AF_INET,
               SOCK_STREAM | SOCK_NONBLOCK, 0);
    call_var(sd_listen);

    call(bind(sd_listen,
              (struct sockaddr *)&skadd_server,
              sizeof(struct sockaddr)));
    call(listen(sd_listen, COUNT_CLIENT_MAX));

    int option = 1;
    call(setsockopt(sd_listen,
                    SOL_SOCKET, SO_REUSEADDR,
                    &option, sizeof(option)));

    // udp
    sd_udp = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    call_var(sd_udp);
    call(bind(sd_udp,
              (struct sockaddr *)&skadd_server,
              sizeof(struct sockaddr)));

    // loops: i/o multiplexing and non-blocking accepts
    pthread_t multiplexing_thread;
    call0(pthread_create(&multiplexing_thread, NULL,
                         &multiplexing, NULL));
    pthread_t udp_thread;
    call0(pthread_create(&udp_thread, NULL,
                         &udp_communication, NULL));

    call(printf("the server is online.\n\n"));
    bool maintenance_flag = true;
    for (; running_condition();)
    {
        // maintenance time
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        if (0 == tm.tm_hour && 0 == tm.tm_min &&
            maintenance_flag)
        {
            if (ERR_CODE ==
                maintenance(&multiplexing_thread,
                            &udp_thread, argc, path_xml))
                error("maintenance() failed");
            maintenance_flag = false;
        }
        else
            maintenance_flag = true;

        // additional error check
        if (errno)
        {
            warning(strerror(errno));
            errno = 0;
        }

        // accepted client
        int sd_client = accept(sd_listen, NULL, NULL);
        if (errno && EWOULDBLOCK != errno)
            error(strerror(errno));
        errno = 0;
        if (ERR_CODE == sd_client)
            continue;
        if (ERR_CODE == ioctl(sd_client, FIONBIO, &option))
        {
            error(strerror(errno));
            errno = 0;
            continue;
        }

        // sets
        FD_SET(sd_client, &descriptors.container);
        if (sd_client >= descriptors.count)
            descriptors.count = sd_client + 1;
    }

    // the server closes, an admin key was used
    // using call() is now accepted
    call0(pthread_join(multiplexing_thread, NULL));
    call0(pthread_join(udp_thread, NULL));
    for (int fd = 0; fd < descriptors.count; fd++)
        if (FD_ISSET(fd, &descriptors.container))
        {
            warning("not closed socket");
            close(fd);
        }

    call(close(sd_listen));
    call(close(sd_udp));
    call(printf("the server is offline.\n\n"));
    return EXIT_SUCCESS;
}

// receives three bytes sends two bytes at minimum
void *udp_communication(void *)
{
    // todo received udp socket as parameter not global
    unsigned char buffer[3];
    struct sockaddr_in skaddr_client;
    socklen_t length = sizeof(skaddr_client);

    for (; running_condition();)
    {
        int bytes =
            recvfrom(sd_udp, buffer,
                     3 * sizeof(char), NO_FLAG,
                     (struct sockaddr *)&skaddr_client,
                     &length);

        if (ERR_CODE == bytes)
        {
            if (EWOULDBLOCK != errno)
                error("recvfrom() failed");
            errno = 0;
            continue;
        }

        struct rr_route data[COUNT_ROUTES_MAX];
        unsigned short count =
            udp_parse(buffer[0], buffer[1],
                      buffer[2], data);

        sendto(sd_udp, &count, sizeof(count), NO_FLAG,
               (struct sockaddr *)&skaddr_client,
               length);

        sendto(sd_udp, data,
               count * sizeof(struct rr_route),
               NO_FLAG,
               (struct sockaddr *)&skaddr_client,
               length);
    }
}

// receives four bytes, sends one byte
void *tcp_communication(const int sd)
{
    if (!FD_ISSET(sd, &descriptors.container))
        return NULL;

    unsigned char command = 0, argument1 = 0;
    unsigned short argument0 = 0;
    ssize_t bytes = read_all(sd, &command, sizeof(command));
    bytes += read_all(sd, &argument0, sizeof(argument0));
    bytes += read_all(sd, &argument1, sizeof(argument1));
    if (errno || bytes != 4)
    {
        warning("client disconnected while receving command");
        if (ECONNRESET != errno && errno)
            error(strerror(errno));

        FD_CLR(sd, &descriptors.container);
        if (ERR_CODE == close(sd))
            error("close() failed");
        errno = 0;
        return NULL;
    }

    unsigned char outcome =
        tcp_parse(command, argument0, argument1);
    bytes = write_all(sd, &outcome, sizeof(outcome));
    if (errno || bytes != sizeof(outcome))
    {
        warning("client disconnected while sending outcome");
        if (ECONNRESET != errno && errno)
            error(strerror(errno));

        FD_CLR(sd, &descriptors.container);
        if (ERR_CODE == close(sd))
            error("close() failed");
        errno = 0;
        return NULL;
    }

    if (TCP_CODE_Q == command)
    {
        FD_CLR(sd, &descriptors.container);
        if (ERR_CODE == close(sd))
            error("close() failed");
    }

    return NULL;
}

void *multiplexing(void *)
{
    struct timeval TV = {1, 0};
    for (; running_condition();)
    {
        fd_set tcp_fd;
        memcpy(&tcp_fd, &descriptors.container,
               sizeof(descriptors.container));

        int count_selected =
            select(descriptors.count, &tcp_fd,
                   NULL, NULL, &TV);

        if (ERR_CODE == count_selected)
            error("select() failed");
        for (int sd = 4;
             sd < descriptors.count &&
             count_selected;
             sd++)
            if (FD_ISSET(sd, &tcp_fd))
            {
                tcp_communication(sd);
                count_selected--;
            }
    }

    return NULL;
}

//------------------------------------------------

void check_xml(int *argc, char *argv[],
               char *const path_xml)
{
    if (*argc > 2)
    {
        error("at most one xml file name is expected.\n");
        exit(EXIT_FAILURE);
    }

    if (1 == *argc)
    {
        strcat(path_xml, name_random);
        write_xml(path_binary);
        return;
    }

    // 2 == argc
    char path_tmp[BYTES_PATH_MAX];
    strcpy(path_tmp, path_xml);
    strcat(path_tmp, argv[1]);
    if (0 == test_xml(path_tmp))
    {
        strcat(path_xml, argv[1]);
        return;
    }

    // random generation
    *argc = 1; // later generations
    strcat(path_xml, name_random);
    write_xml(path_binary);
}

/* a server should always be online
 * this should always return true
 * provides error messages
 */
bool running_condition()
{
    char key = '0';
    int fd = open(path_thread, O_RDONLY);
    if (ERR_CODE == fd ||
        ERR_CODE == read_all(fd, &key, sizeof(key)) ||
        ERR_CODE == close(fd))
    {
        error("running_condition() failed");
        return false;
    }

    return '1' == key;
}

/* even though clients are not served at this time
 * if something goes wrong here,
 * the server shouldn't stop
 * DOES NOT provide error message
 * returns the count of routes newly accessed
 */
int maintenance(pthread_t *th0, pthread_t *th1,
                const int argc,
                const char *const path_xml)
{
    if (NULL == th0 || NULL == th1 || NULL == path_xml)
        return ERR_CODE;

    // stop serving clients
    char key = '0'; // security--
    int fd = open(path_thread, O_WRONLY);
    if (ERR_CODE == fd ||
        sizeof(key) != write_all(fd, &key, sizeof(key)) ||
        ERR_CODE == close(fd))
        return ERR_CODE;

    if (pthread_join(*th0, NULL) ||
        pthread_join(*th1, NULL))
        return ERR_CODE;

    for (int fd = 0; fd < descriptors.count; fd++)
        if (FD_ISSET(fd, &descriptors.container))
        {
            warning("not closed socket");
            close(fd);
        }

    // check the new schedule for today
    if (1 == argc)
        write_xml(path_binary);
    read_xml(path_xml);

    // restart threads
    if (pthread_create(th0, NULL,
                       &multiplexing, NULL) ||
        pthread_create(th0, NULL,
                       &udp_communication, NULL))
        return ERR_CODE;

    // restart serving clients
    key = '1'; // security--
    fd = open(path_thread, O_WRONLY);
    if (ERR_CODE == fd ||
        sizeof(key) != write_all(fd, &key, sizeof(key)) ||
        ERR_CODE == close(fd))
        return ERR_CODE;

    // success
    return count_routes;
}
