#ifndef _00API00_
#define _00API00_

#include <time.h>
#include <stdbool.h>
#include "route.h"
#include "shared.h"

extern struct rr_route schedule[COUNT_ROUTES_MAX];
extern unsigned short count_routes;

// today
unsigned short routes(struct rr_route *const data,
                      const unsigned char l_d,
                      const unsigned char l_a)
{
    if (NULL == data)
    {
        error("routes() received null data");
        return RECV_FAIL;
    }

    unsigned short index_route = 0;
    for (unsigned short i = 0; i < count_routes; i++)
    {
        if (l_d != get_location(schedule[i].departure_data))
            break;
        if (l_a == get_location(schedule[i].arrival_data))
            data[index_route++] = schedule[i];
    }

    set_last(&data[index_route]);
    return index_route;
}

// in the next hour
unsigned short departures(struct rr_route *const data,
                          const unsigned short l_d)
{
    if (NULL == data)
        return RECV_FAIL;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    const unsigned short current_time =
        (tm.tm_hour) * 60 + tm.tm_min;

    unsigned short index_route = 0;
    for (unsigned short i = 0; i < count_routes; i++)
    {
        if (l_d != get_location(schedule[i].departure_data))
            continue;
        if (current_time >
            get_time(schedule[i].departure_data))
            continue;
        if (current_time + 60 >
            get_time(schedule[i].departure_data))
            data[index_route++] = schedule[i];
    }

    set_last(&data[index_route]);
    return index_route;
}

// in the next hour
unsigned short arrivals(struct rr_route *const data,
                        const unsigned short l_a)
{
    if (NULL == data)
        return RECV_FAIL;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    const unsigned short current_time =
        (tm.tm_hour) * 60 + tm.tm_min;

    unsigned short index_route = 0;
    for (unsigned short i = 0; i < count_routes; i++)
    {
        if (l_a != get_location(schedule[i].arrival_data))
            continue;
        if (current_time >
            get_time(schedule[i].arrival_data))
            continue;
        if (current_time + 60 >
            get_time(schedule[i].arrival_data))
            data[index_route++] = schedule[i];
    }

    set_last(&data[index_route]);
    return index_route;
}

unsigned char report(const unsigned short id_train,
                     const unsigned char minutes)
{
    // invalid arguments
    if (id_train >= count_routes || minutes >= DELEY_MAX)
        return RECV_FAIL;

    // already faulted
    unsigned char *data = &schedule[id_train].delay_data;
    if (*data >= DELEY_MAX)
        return RECV_FAIL;

    // can not report something that arrived already
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    if (tm.tm_hour * 60 + tm.tm_min >
        get_time(schedule[id_train].arrival_data))
        return RECV_FAIL;

    // success
    unsigned short temp = (unsigned short)*data;
    if (temp + (unsigned short)minutes >= DELEY_MAX)
        *data = DELEY_MAX;
    else
        schedule[id_train].delay_data += minutes;
    return PTCP_SUCCESS;
}

//------------------------------------------------

unsigned char tcp_parse(const unsigned char command,
                        const unsigned short argument0,
                        const unsigned char argument1)
{
    if (TCP_CODE_R == command)
        return report(argument0, argument1);
    if (TCP_CODE_Q == command)
        return PTCP_SUCCESS;
    return RECV_FAIL;
}

unsigned short udp_parse(const unsigned char command,
                         const unsigned char argument0,
                         const unsigned char argument1,
                         struct rr_route *const data)
{
    if (NULL == data)
    {
        warning("udp_parse() received null pointer");
        return RECV_FAIL;
    }

    switch (command)
    {
    case UDP_CODE_R:
        return routes(data, argument0, argument1);
    case UDP_CODE_D:
        return departures(data, argument0);
    case UDP_CODE_A:
        return arrivals(data, argument1);

    default: // maybe udp
        if (command > TCP_CODE_Q)
            return routes(data, argument0, argument1);
        break;
    }

    // invalid command
    set_last(&data[0]);
    return RECV_FAIL;
}

#endif
