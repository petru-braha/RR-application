#ifndef _00PRINTER00_
#define _00PRINTER00_

#include <stdio.h>
#include "route.h"

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

void server_print(struct rr_route r)
{
  printf("%d %d %d %d\n",
         (int)r.location_departure,
         (int)r.location_arrival,
         (int)r.time_departure,
         (int)r.time_arrival);
}

void parse_location(const char *buffer,
                    unsigned char location)
{
  // todo: read data/location.txt
}

void client_print(struct rr_route r)
{
  char d[10] = "Bucuresti", a[10] = "Iasi";
  parse_location(d, r.location_departure);
  parse_location(a, r.location_arrival);

  printf("departure: \tarrival:\n%s \t%s\n", d, a);
  printf("%d:%d \t\t%d:%d\n\n",
         (int)r.time_departure / 60,
         (int)r.time_departure % 60,
         (int)r.time_arrival / 60,
         (int)r.time_arrival % 60);
}

#endif
