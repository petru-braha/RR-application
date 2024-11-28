#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include <string.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <stdbool.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include "include/shared.h"
#include "include/command.h"

#define CHILD 0
#define PARNT 1
#define PORT 2728

// a server should always be online
bool running_condition()
{
    int fd = open("include/dev/key.txt", O_RDONLY);
    call_var(fd);

    char key = 0;
    call(read(fd, &key, 1));
    call(close(fd));

    return key != '0';
}

char *conv_addr(struct sockaddr_in address)
{
    static char str[25];
    char port[7];

    /* adresa IP a clientului */
    strcpy(str, inet_ntoa(address.sin_addr));
    /* portul utilizat de client */
    bzero(port, 7);
    sprintf(port, ":%d", ntohs(address.sin_port));
    strcat(str, port);
    return (str);
}

/*
char *iteration(const char *const command, const int command_size)
{
  int pipe_server_child[2];
  int socket_child_server[2];

  pipe(pipe_server_child);
  socketpair(AF_UNIX, SOCK_STREAM, 0, socket_child_server);

  char *output = 0;

  // parent
  pid_t pid = fork();
  if (pid)
  {
    close(socket_child_server[CHILD]);
    close(pipe_server_child[READ]);

    write(pipe_server_child[WRIT], command, command_size);

    char number[3];
    read(socket_child_server[1], number, 2);
    number[2] = 0;
    int output_size = atoi(number);
    char buffer[MAX_OUTPUT_SIZE];
    read(socket_child_server[1], buffer, output_size);
    buffer[output_size] = 0;
    output = (char *)malloc(2 + output_size);
    strcpy(output, number);
    strcat(output, buffer);

    // patch
    if (strstr(buffer, "welcome"))
      strcpy(server_username, buffer + 9);
    if ('!' != buffer[strlen(buffer) - 1])
      server_username[strlen(server_username) - 1] = 0;
    if (strstr(buffer, "we will miss"))
      for (size_t i = 0; i < MAX_STRING_SIZE; i++)
        server_username[i] = 0;

    close(pipe_server_child[WRIT]);
    close(socket_child_server[PARNT]);

    int child_return_value = -1;
    wait(&child_return_value);
  }

  // child
  else
  {
    close(pipe_server_child[WRIT]);
    close(socket_child_server[PARNT]);

    char buffer[MAX_COMMAND_SIZE];
    read(pipe_server_child[0], buffer, command_size);
    buffer[command_size] = 0;

    char *message = parse_command(buffer);
    write(socket_child_server[0], message, strlen(message));
    free(message);

    close(pipe_server_child[READ]);
    close(socket_child_server[CHILD]);
    exit(EXIT_SUCCESS);
  }

  return output;
}
*/

int main()
{
    struct sockaddr_in skadd_server;
    struct sockaddr_in skadd_client;
    bzero(&skadd_server, sizeof(skadd_server));
    bzero(&skadd_client, sizeof(skadd_client));

    int sd_server_tcp = socket(AF_INET, SOCK_STREAM, 0);
    int sd_server_udp = socket(AF_INET, SOCK_DGRAM, 0);
    call_var(sd_server_tcp);
    call_var(sd_server_udp);

    int option = 1;
    setsockopt(sd_server_tcp, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    skadd_server.sin_family = AF_INET;
    skadd_server.sin_addr.s_addr = htonl(INADDR_ANY);
    skadd_server.sin_port = htons(PORT);

    call(bind(sd_server_tcp, (struct sockaddr*)&skadd_server, sizeof(struct sockaddr)));
    call(listen(sd_server_tcp, 5));

    fd_set actfds;
    FD_ZERO(&actfds);
    fd_set readfds;
    FD_SET(sd_server_tcp, &actfds);

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    printf("the server is online ba.\n\n");

    int count_fd = sd_server_tcp + 1;
    
    for (; running_condition();)
    {
      printf("%d", running_condition());
    }
    
    printf("%d gata executia\n", running_condition());
    return 0;
    
    for (; running_condition();)
    {
        /* ajustam multimea descriptorilor efectiv utilizati */
        bcopy((char *)&actfds, (char *)&readfds, sizeof(readfds));
        call(select(count_fd, &readfds, NULL, NULL, &tv));

        /* vedem daca e pregatit socketul pentru a-i accepta pe clienti */
        if (FD_ISSET(sd_server_tcp, &readfds))
        {
            /* pregatirea structurii client */
            int len = sizeof(skadd_client);
            bzero(&skadd_client, sizeof(skadd_client));

            int std_client_tcp = accept(sd_server_tcp, (struct sockaddr*)&skadd_client, &len);
            call_var(std_client_tcp);

            if (count_fd <= std_client_tcp)
                count_fd = std_client_tcp + 1;

            /* includem in lista de descriptori activi si acest socket */
            FD_SET(std_client_tcp, &actfds);

            printf("[server] S-a conectat clientul cu descriptorul %d, de la adresa %s.\n", std_client_tcp, conv_addr(skadd_client));
            fflush(stdout);
        }

        /* vedem daca e pregatit vreun socket client pentru a trimite raspunsul */
        for (int fd = 0; fd < count_fd; fd++)
        {
            if (fd != sd_server_tcp && FD_ISSET(fd, &readfds))
            {
                //if (sayHello(fd))
                {
                    printf("[server] S-a deconectat clientul cu descriptorul %d.\n", fd);
                    close(fd);
                    FD_CLR(fd, &actfds);
                }
            }
        }
    }

    printf("the server is offline.\n");
    return EXIT_SUCCESS;
}