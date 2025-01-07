#include <stdio.h>
#include <time.h>

// success
/* https://stackoverflow.com/questions/1442116/how-to-get-the-date-and-time-values-in-a-c-program */
int main()
{
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  printf("now: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  printf("now: %02d:%02d:%02d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
}
