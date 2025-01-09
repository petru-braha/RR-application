#ifndef _00PRINTER00_
#define _00PRINTER00_

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "error.h"
#include "route.h"
#include "shared.h"

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
  if (index && '\n' == buffer[index - 1])
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

// returns strlen(location)
unsigned short stringtocode(const char *const string,
                            unsigned short *const code,
                            const char *const path)
{
  *code = COUNT_LOCATION;
  FILE *file = fopen(path, "r");
  if (NULL == file)
  {
    error("stringtocode() failed - fopen()");
    return 0;
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
      return 0;
    }

    repair(location);
    if (string == strstr(string, location))
    {
      fclose(file);
      *code = index_location;
      return strlen(location);
    }
  }

  fclose(file);
}

// decoding a single rr_route
void client_print(struct rr_route r,
                  const char *const path)
{
  char d[BYTES_COMMAND_MAX], a[BYTES_COMMAND_MAX];
  codetostring(d, get_location(r.departure_data), path);
  codetostring(a, get_location(r.arrival_data), path);

  unsigned short d_time = get_time(r.departure_data),
                 a_time = get_time(r.arrival_data);

  printf("%s(%02d:%02d) -> ", d,
         d_time / 60, d_time % 60);
  printf("%s(%02d:%02d) - ", a,
         a_time / 60, a_time % 60);

  // status print
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  unsigned short current_time =
      tm.tm_hour * 60 + tm.tm_min;

  // not departed
  if (current_time < get_time(r.departure_data))
  {
    if (r.delay_data)
      if (r.delay_data >= DELEY_MAX)
        printf("faulted - won't be available;\n");
      else
        printf("will depart %d minutes later;\n",
               r.delay_data);
    else // no reports on it
      printf("will depart in %d minutes;\n",
             get_time(r.departure_data) - current_time);
    return;
  }

  // departed and not arrived
  if (current_time < get_time(r.arrival_data))
  {
    if (r.delay_data)
      if (r.delay_data >= DELEY_MAX)
        printf("faulted - won't arrive to destination;\n");
      else
        printf("will be %d minutes late;\n",
               r.delay_data);
    else // no reports on it
      printf("will arrive in %d minutes;\n",
             get_time(r.arrival_data) - current_time);
    return;
  }

  // departed and supposedly arrived
  if (r.delay_data)
    if (r.delay_data >= DELEY_MAX)
      printf("faulted - didn't arrive to destination;\n");
    else
      printf("arrived %d minutes later;\n",
             r.delay_data);
  else // no reports on it
    printf("arrived at the right time;\n");
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
