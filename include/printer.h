#ifndef _00PRINTER00_
#define _00PRINTER00_

// command signatures
const char command0[] = "0. [id_train, time_departure_confirmed, time_arrival_confirmed] routes(location_departure, location_arrival)";
const char command1[] = "1. [id_train, time_departure_confirmed, location_arrival] departures(location_departure)";
const char command2[] = "2. [id_train, time_arrival_confirmed, location_departure] arrivals(location_arrival)";
const char command3[] = "3. void quit()";

void print_protocol()
{
  printf("%s\n%s\n%s\n%s\n", command0, command1,
  command2, command3);
}

#endif
