#ifndef _00API00_
#define _00API00_

#include <stdbool.h>
#include "error.h"
#include "route.h"

#define UDP_ROUTES 250
#define UDP_DEPARTURES 251
#define UDP_ARRIVALS 252
#define TCP_REPORT 5
#define TCP_QUIT 6

extern struct rr_route schedule[COUNT_ROUTES_MAX];

[[deprecated("parsing should be simple - just one byte")]]
void parse_command(const char *command, char *outcome)
{
    if (0 == strcmp(command, "report") ||
        0 == strcmp(command, "quit"))
    {
        strcat(strcpy(outcome, "TCP: "), command);
        return;
    }

    strcpy(outcome, "UDP: ");
    if (0 == strcmp(command, "routes") ||
        0 == strcmp(command, "departures") ||
        0 == strcmp(command, "arrivals"))
    {
        strcat(outcome, command);
        return;
    }

    strcat(outcome, "invalid command");
}

void parse(const unsigned char command,
           const unsigned short *const argument0,
           const unsigned short *const argument1,
           struct rr_route *const data,
           size_t *const count)
{

    // UDP command
    switch (command)
    {
    case UDP_ROUTES:
        routes(data, count, argument0, argument1);
        return;
    case UDP_DEPARTURES:
        departures(data, count, argument0);
        return;
    case UDP_ARRIVALS:
        arrivals(data, count, argument1);
        return;

    default:
        // maybe udp
        if (command >= 200)
            routes(data, count, argument0, argument1);
        return;
    }

    if (TCP_REPORT == command)
    {
        report(argument0, argument1);
        return;
    }

    // quit() is treated in tcp_communication()
    // invalid command
    set_last(&data[0]);
    count = 0;
}

void routes(struct rr_route *data, size_t *const count,
            const unsigned char l_d,
            const unsigned char l_a)
{
    for (size_t i = 0; i < COUNT_ROUTES_MAX; i++)
    {
        if (schedule[i].location_departure == l_d &&
            schedule[i].location_arrival == l_a)
            ;
        // write(sd, NULL, 0); // print
    }
}

// in the next hour
void departures(struct rr_route *data, size_t &count,
                const unsigned char l_d)
{
    for (size_t i = 0; i < COUNT_ROUTES_MAX; i++)
    {

        /*
        if (schedule[i].location_departure == l_d &&
                schedule[i].time_departure.hours == now.hours ||

            (schedule[i].time_departure.hours == now.hours + 1 &&
             schedule[i].time_departure.minutes <= now.minutes))
            //write(sd, NULL, 0); // print*/
    }
}

// in the next hour
void arrivals(struct rr_route *data, size_t &count,
              const unsigned char l_a)
{
    for (size_t i = 0; i < COUNT_ROUTES_MAX; i++)
    {
    }
}

void report(const unsigned short id_train,
            const unsigned char minutes)
{
}

#endif
