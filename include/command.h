#ifndef _0COMMAND0_
#define _0COMMAND0_

#include <stdbool.h>
#include "shared.h"
#include "route.h"

//------------------------------------------------
// help:

char *get_argument(const char *const command)
{
    return strstr(command, " : ") + 3;
}

//------------------------------------------------
// api:

void parse_command(const char const *command, char *outcome)
{
    if (0 == strcmp(command, "report") ||
        0 == strcmp(command, "quit"))
    {
        strcat(strcpy(outcome, "TCP: "), command);
        return;
    }

    if (0 == strcmp(command, "routes") ||
        0 == strcmp(command, "departures") ||
        0 == strcmp(command, "arrivals"))
    {
        strcat(strcpy(outcome, "UDP: "), command);
        return;
    }

    strcpy(outcome, "invalid command");
}

/*
const size_t count_routes_max = 100;
struct rr_route schedule[count_routes_max];

void routes(int location_departure, int location_arrival, int sd)
{
    for (size_t i = 0; i < count_routes_max; i++)
    {
        if (schedule[i].location_departure == location_departure &&
            schedule[i].location_arrival == location_arrival)
            write(sd, NULL, 0); // print
    }
}

// in the next hour
void departures(int location_departure, struct rr_time now, int sd)
{
    for (size_t i = 0; i < count_routes_max; i++)
    {
        if (schedule[i].location_departure == location_departure &&

                schedule[i].time_departure.hours == now.hours ||

            (schedule[i].time_departure.hours == now.hours + 1 &&
             schedule[i].time_departure.minutes <= now.minutes))
            write(sd, NULL, 0); // print

    }
}

// in the next hour
void arrival(int location_arrival, struct rr_time now, int sd);

void quit(int sd);
*/
#endif
