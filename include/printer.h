#ifndef _00PRINTER00_
#define _00PRINTER00_

#include <stdio.h>
#include <string.h>
#include "error.h"
#include "route.h"

#define BYTES_COMMAND_MAX 40

/*
const char synopsis0[] = "0. [id_train, time_departure_confirmed, time_arrival_confirmed] routes(location_departure, location_arrival)";
const char synopsis1[] = "1. [id_train, time_departure_confirmed, location_arrival] departures(location_departure)";
const char synopsis2[] = "2. [id_train, time_arrival_confirmed, location_departure] arrivals(location_arrival)";
const char synopsis3[] = "3. void report(id_train, minutes)";
const char synopsis4[] = "4. void quit()";

void print_protocol()
{
  printf("%s\n%s\n%s\n%s\n%s", synopsis0, synopsis1,
         synopsis2, synopsis3, synopsis4);
}
*/

void repair(unsigned char *const buffer)
{
  if (NULL == buffer)
  {
    error("repair() failed - null buffer");
    return;
  }

  size_t index = strlen(buffer);
  if (index)
    buffer[index - 1] = '\0';
}

void codetostring(unsigned char *const string,
                  const unsigned char code,
                  const char *const path)
{
  if (code >= COUNT_LOCATION)
  {
    error("codetostring() failed - invalid code");
    return;
  }

  FILE *file = fopen(path, "r");
  if (NULL == file)
  {
    error("codetostring() failed - fopen()");
    return;
  }

  char location[BYTES_COMMAND_MAX];
  for (unsigned short index_location = 0;
       index_location < code + 1;
       index_location++)
  {
    if (location !=
        fgets(location, BYTES_COMMAND_MAX, file))
    {
      error("codetostring() failed - fgets()");
      fclose(file);
      return;
    }
  }

  fclose(file);
  repair(location);
  strcpy(string, location);
}

void stringtocode(const unsigned char *const string,
                  unsigned short *const code,
                  const char *const path)
{
  *code = COUNT_LOCATION;
  FILE *file = fopen(path, "r");
  if (NULL == file)
  {
    error("stringtocode() failed - fopen()");
    return;
  }

  char location[BYTES_COMMAND_MAX];
  for (unsigned short index_location = 0;
       index_location < COUNT_LOCATION;
       index_location++)
  {
    if (location !=
        fgets(location, BYTES_COMMAND_MAX, file))
    {
      error("stringtocode() failed - fgets()");
      fclose(file);
      return;
    }

    repair(location);
    if (0 == strcmp(string, location))
    {
      fclose(file);
      *code = index_location;
      return;
    }
  }

  fclose(file);
}

// decoding a single rr_route
void client_print(struct rr_route r,
                  const char *const path)
{
  // store how many minute it was late
  // todo check for flags and print and delete/store
  char d[BYTES_COMMAND_MAX], a[BYTES_COMMAND_MAX];
  codetostring(d, get_location(r.departure_data), path);
  codetostring(a, get_location(r.arrival_data), path);

  unsigned short d_time = get_time(r.departure_data),
                 a_time = get_time(r.arrival_data);

  printf("%s(%02d:%02d) -> ", d,
         d_time / 60, d_time % 60);
  printf("%s(%02d:%02d)\n", a,
         a_time / 60, a_time % 60);
}

void server_print(struct rr_route r)
{
  printf("%d %d %d %d\n",
         get_location(r.departure_data),
         get_location(r.arrival_data),
         get_time(r.departure_data),
         get_time(r.arrival_data));
}

#endif
