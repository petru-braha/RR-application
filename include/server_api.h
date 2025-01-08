#ifndef _00API00_
#define _00API00_

#include <time.h>
#include <stdbool.h>
#include "shared.h"

extern struct rr_route schedule[COUNT_ROUTES_MAX];
extern unsigned short count_routes;

// today
void routes(struct rr_route *data,
            unsigned short *const count,
            const unsigned short *l_d,
            const unsigned short *l_a)
{
    if (NULL == data)
    {
        error("routes() received null data");
        *count = 0;
        return;
    }

    if (NULL == l_d || NULL == l_a)
    {
        warning("routes() received null arguments");
        unsigned short location0 = 0, location1 = 1;
        l_d = &location0;
        l_a = &location1;
    }

    unsigned short index_route = 0;
    for (unsigned short i = 0; i < count_routes; i++)
    {
        if (get_location(schedule[i].departure_data) !=
            (unsigned char)*l_d)
            break;
        if (get_location(schedule[i].arrival_data) ==
            (unsigned char)*l_a)
            data[index_route++] = schedule[i];
    }

    set_last(&data[index_route]);
    *count = index_route;
}

// in the next hour
void departures(struct rr_route *data,
                unsigned short *const count,
                const unsigned short *l_d)
{
    if (NULL == data)
    {
        error("departures() received null data");
        *count = 0;
        return;
    }

    if (NULL == l_d)
    {
        warning("departures() received null arguments");
        unsigned short location0 = 0;
        l_d = &location0;
    }

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    unsigned short next_hour =
        (tm.tm_hour + 1) * 60 + tm.tm_min;

    unsigned short index_route = 0;
    for (unsigned short i = 0; i < count_routes; i++)
        if (get_time(schedule[i].departure_data) ==
            next_hour)
            data[index_route++] = schedule[i];

    set_last(&data[index_route]);
    *count = index_route;
}

// in the next hour
void arrivals(struct rr_route *data,
              unsigned short *const count,
              const unsigned short *l_d)

{
    if (NULL == data)
    {
        error("arrivals() received null data");
        *count = 0;
        return;
    }

    if (NULL == l_d)
    {
        warning("arrivals() received null arguments");
        unsigned short location0 = 0;
        l_d = &location0;
    }

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    unsigned short next_hour =
        (tm.tm_hour + 1) * 60 + tm.tm_min;

    unsigned short index_route = 0;
    for (unsigned short i = 0; i < count_routes; i++)
        if (get_time(schedule[i].arrival_data) ==
            next_hour)
            data[index_route++] = schedule[i];

    set_last(&data[index_route]);
    *count = index_route;
}

void report(unsigned short *const flag,
            const unsigned short *const id_train,
            const unsigned short *const minutes)
{
    if (NULL == id_train || NULL == minutes)
    {
        *flag = 0;
        return;
    }

    if (*id_train >= count_routes || *minutes >= DELEY_MAX)
    {
        *flag = 0;
        return;
    }

    unsigned char *data = &schedule[*id_train].delay_data;
    if (*data >= DELEY_MAX)
        return;
    unsigned short temp = (unsigned short)*data;
    if (temp += *minutes >= DELEY_MAX)
        *data = DELEY_MAX;
    else
        schedule[*id_train].delay_data += *minutes;
}

//------------------------------------------------

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
           const unsigned short *argument0,
           const unsigned short *argument1,
           struct rr_route *const data,
           unsigned short *const count)
{
    if (NULL == count)
    {
        error("parse() failed - count is nullptr");
        return;
    }

    // UDP command
    switch (command)
    {
    case UDP_CODE_R:
        routes(data, count, argument0, argument1);
        return;
    case UDP_CODE_D:
        departures(data, count, argument0);
        return;
    case UDP_CODE_A:
        arrivals(data, count, argument1);
        return;

    default:
        // maybe udp
        if (command >= 200)
            routes(data, count, argument0, argument1);
        return;
    }

    if (TCP_CODE_R == command)
    {
        // it is expected: NULL == data
        if(data)
            warning("tcp command treated as query");
        report(count, argument0, argument1);
        return;
    }

    // quit() is treated in tcp_communication()
    // invalid command
    set_last(&data[0]);
    *count = 0;
}

#endif
