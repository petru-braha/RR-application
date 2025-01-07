#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

fd_set tcp_c;

// success
int main()
{
  const int nr = 1024 * 10 + 510; //? < 520

  for (size_t i = 0; i < nr; i++)
    FD_SET(i, &tcp_c);
  for (size_t i = 0; i < nr; i++)
    if (!FD_ISSET(i + 2, &tcp_c))
      printf("cacat\n");
  return EXIT_SUCCESS;
}
