#ifndef _00FILES00_
#ifndef _00GENERATE00_
#define _00GENERATE00_

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include "error.h"
#include "route.h"

int write_file(
    const char *const path,
    const char *const path_location);
int read_txt(const char *const path);
int test_txt(const char *const path);

struct rr_route schedule[COUNT_ROUTES_MAX];
unsigned short count_routes = 0;

int write_file(
    const char *const path,
    const char *const path_location)
{
  if (NULL == path || NULL == path_location)
  {
    error("write_file() failed - null path");
    return EXIT_FAILURE;
  }

  pid_t process = fork();
  if (0 == process)
  {
    execl(path, path_location, NULL);
    error("write_file() failed - execl()");
    return EXIT_FAILURE;
  }
  else
  {
    int rt;
    wait(&rt);
    return rt;
  }
}

int test_txt(const char *const path)
{
  int document = open(path, O_RDONLY);
  if (ERR_CODE == document)
    return EXIT_FAILURE;

  char byte;
  byte = read(document, &byte, sizeof(byte));
  close(document);

  if (ERR_CODE == byte || 0 == byte)
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

int read_txt(const char *const path)
{
  FILE *document = fopen(path, "r");
  if (NULL == document)
  {
    error("read_txt() failed - wrong path.\n");
    exit(EXIT_FAILURE);
  }

  char *data = NULL;
  char *pulled = NULL;
  char *wicked = NULL;

  ssize_t bytes = 0;
  size_t line_length = 0;
  const int number_base = 10;
  unsigned short index_route = 0;

  while (ERR_CODE !=
         (bytes = getline(&data, &line_length, document)))
  {
    unsigned short time = 0;
    struct rr_route new_route = {0, 0, 0, 0};

    // location
    pulled = strtok(data, " \n\0");
    if (NULL == pulled)
      continue;

    new_route.departure_data =
        strtoul(pulled, &wicked, number_base);
    if (*wicked ||
        new_route.departure_data >= COUNT_LOCATION) // error
      continue;

    // location
    pulled = strtok(NULL, " \n\0");
    if (NULL == pulled)
      continue;

    new_route.arrival_data =
        strtoul(pulled, &wicked, number_base);
    if (*wicked ||
        new_route.arrival_data >= COUNT_LOCATION) // error
      continue;

    // extra check
    if (new_route.departure_data ==
        new_route.arrival_data)
      continue;

    // time
    pulled = strtok(NULL, " \n\0");
    if (NULL == pulled)
      continue;

    time = strtoul(pulled, &wicked, number_base);
    new_route.departure_data += COUNT_LOCATION * time;
    if (*wicked || time >= TIME_MAX) // error
      continue;

    // time
    pulled = strtok(NULL, " \n\0");
    if (NULL == pulled)
      continue;

    time = strtoul(pulled, &wicked, number_base);
    new_route.arrival_data += COUNT_LOCATION * time;
    if (*wicked || time >= TIME_MAX) // error
      continue;

    // success
    new_route.id_train = index_route;
    schedule[index_route++] = new_route;
  }

  if (data)
    free(data);

  set_last(&schedule[index_route]);
  count_routes = index_route;

  if (fclose(document))
  {
    error("read_txt() failed - fclose()");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}

#endif
#endif
