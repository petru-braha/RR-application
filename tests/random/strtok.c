#include <stdio.h>
#include <string.h>

int main()
{
  char *ptr = NULL;
  char line[] = "10 11 12 13";
  ptr = strtok(line, " \n");
  while (ptr)
  {
    printf("%s\n", ptr);
    ptr = strtok(NULL, " \n");
  }
}
