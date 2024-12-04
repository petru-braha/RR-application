#ifndef _00COMPUTATION00_
#define _00COMPUTATION00_

#include <time.h>

void computation()
{
  for (int i = 0; i < 1000; i++)
    for (int j = 0; j < 1000; j++)
      for (int k = 0; k < 1000; k++)
        ;
}

// 20 iterations
double much_computation()
{
  clock_t time = clock();
  for (int i = 0; i < 20; i++)
    computation();

  time = clock() - time;
  return (double)time / CLOCKS_PER_SEC;
}

#endif
