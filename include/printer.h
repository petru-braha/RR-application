#ifndef _00PRINTER00_
#define _00PRINTER00_

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

#endif
