#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

int main()
{
  ssize_t bytes = 0;
  size_t length = 0;
  char *line = NULL;
  while (-1 != (bytes = getline(&line, &length, stdin)))
  {
    printf("%s", line);
  }

  if (line)
    free(line);
}
