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

const uint16_t port0 = 2970;
struct timeval TV = {2, 0};
pthread_t thread[100];
const int ONE_CLIENT_ONLY = 1;

struct rr_thread
{
  int id, sd_client_tcp, sd_client_udp;
  struct sockaddr_in skadd_client;
};

typedef struct
{
  fd_set container;
  int count;
} rr_fd;

// a server should always be online
static bool running_condition()
{
  int fd = open("include/dev/key.txt", O_RDONLY);
  call_var(fd);

  char key = 0;
  call(read(fd, &key, 1));
  call(close(fd));

  return key != '0';
}

void *client_execution(void *thread_data)
{
  struct rr_thread td = *(struct rr_thread *)thread_data;
  pthread_detach(pthread_self());

  char command[100];
  char outcome[100];

  while (strcmp(command, "quit"))
  {
    printf("server received: \"%s\" %d %d\n", command, strlen(command), (int)strcmp(command, "report"));
    bzero(command, 100);

    // command is null
    // get command

    // tcp
    if (0 == strcmp(command, "report"))
    {
      printf("tcp command\n");
      call(read(td.sd_client, command, 100));
      strcpy(outcome, "tcp - raspuns la ");
    }

    // udp
    else
    {
      printf("udp command\n");
      socklen_t sk_add_length = sizeof(td.skadd_client);
      recvfrom(td.sd_client, command, 100, 0,
               (struct sockaddr *)&td.skadd_client,
               &sk_add_length);
      strcpy(outcome, "udp - raspuns la ");
    }

    strcat(outcome, command);
    call(write(td.sd_client, outcome, 100));
  }

  call(close(td.sd_client));
  return NULL;
};

int main()
{
  // server addresses
  struct sockaddr_in skadd_server_tcp;
  bzero(&skadd_server_tcp, sizeof(skadd_server_tcp));

  skadd_server_tcp.sin_family = AF_INET;
  skadd_server_tcp.sin_addr.s_addr = htonl(INADDR_ANY);
  skadd_server_tcp.sin_port = htons(PORT1);
  
  skadd_server_udp.sin_family = AF_INET;
  skadd_server_udp.sin_addr.s_addr = htonl(INADDR_ANY);
  skadd_server_udp.sin_port = htons(PORT2);
  
  // server sockets
  int sd_server_tcp = socket(AF_INET, SOCK_STREAM, 0);
  int sd_server_udp = socket(AF_INET, SOCK_DGRAM, 0);
  call_var(sd_server_tcp);
  call_var(sd_server_udp);
  
  call(bind(sd_server_tcp, (struct sockaddr *)&skadd_server_tcp,
            sizeof(struct sockaddr)));
  call(bind(sd_server_udp, (struct sockaddr *)&skadd_server_udp,
            sizeof(struct sockaddr)));
  call(listen(sd_server_tcp, 5));

  // loop
  struct sockaddr_in skadd_client;
  bzero(&skadd_client, sizeof(skadd_client));
  printf("the server is online.\n\n");
  
  int index_thread = 0;
  int count_fd = sd_server_tcp + 1;
  for (; running_condition();)
  {
    int length = sizeof(skadd_client);
    int sd_client =
        accept(sd_server_tcp, (struct sockaddr *)&skadd_client, &length);
    call_var(sd_client);

    struct rr_thread td = 
      {index_thread++, sd_client, skadd_client, sd_udp};
    pthread_create(&thread[index_thread], NULL, &client_execution, &td);
  }

  close(sd_server_tcp);
  close(sd_server_udp);
  printf("the server is offline.\n");
  return EXIT_SUCCESS;
}


// one client // tcp udp tcp udp
int main()
{
  // tcp first socket
  int sd_tcp = 0;
  sd_tcp = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == sd_tcp)
    error();

  struct sockaddr_in skadd_server;
  bzero(&skadd_server, sizeof(skadd_server));
  skadd_server.sin_family = AF_INET;
  skadd_server.sin_addr.s_addr = htonl(INADDR_ANY);

  skadd_server.sin_port = htons(port0);
  if (-1 == bind(sd_tcp, (struct sockaddr *)&skadd_server, sizeof(struct sockaddr)))
    error();
  if (-1 == listen(sd_tcp, 1))
    error();

  // loop
  struct sockaddr_in skadd_client;
  socklen_t length = sizeof(skadd_client);
  bzero(&skadd_client, length);

  // tcp
  int sd_client_tcp = accept(sd_tcp, (struct sockaddr *)&skadd_client, &length);
  if (-1 == sd_client_tcp)
    error();

  // udp
  int sd_client_udp = socket(AF_INET, SOCK_DGRAM, 0);
  if (-1 == sd_client_udp)
    error();
  if (-1 == bind(sd_client_udp, (struct sockaddr *)&skadd_server, sizeof(struct sockaddr)))
    error();

  socklen_t leen_udp = sizeof(struct sockaddr);

  fd_set udp_read_set;
  fd_set tcp_read_set;
  FD_ZERO(&udp_read_set);
  FD_SET(sd_client_tcp, &tcp_read_set);
  FD_SET(sd_client_udp, &udp_read_set);

  char command[BYTES_COMMAND_MAX];
  for (int condition = 1; condition;)
  {
    recv_command_skadd_server(command);
    printf("\"%s\" ", command);
    condition = strcmp(command, "quit");
  }

  close(sd_client_tcp);
  close(sd_tcp);
  close(sd_client_udp);

  return 0;
}