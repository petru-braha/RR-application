#ifndef _00ITINERARY00_
#define _00ITINERARY00_

#include <stdbool.h>
#include <limits.h>
#include "error.h"

// 60 MB
#define COUNT_ROUTES_MAX 10000

#define COUNT_LOCATION 41
#define LOCATION_MIN 0
#define LOCATION_MAX 40

#define TIME_MIN 0
#define TIME_MAX 1441
#define DELEY_MAX 180

// no need to store if departed/arrived
// it can be deduced by the local time
// no need to store if late/fail
// it can be deduced by late minutes

unsigned char get_location(const unsigned short data)
{
    return data % COUNT_LOCATION;
}

unsigned short get_time(const unsigned short data)
{
    return data / COUNT_LOCATION;
}

struct rr_route
{
    unsigned short departure_data;
    unsigned short arrival_data;
    unsigned char delay_data;
};

void set_last(struct rr_route *r)
{
    if (NULL == r)
        warning("set_last() failed - null argument");

    r->departure_data = USHRT_MAX;
    r->arrival_data = USHRT_MAX;
    r->delay_data = UCHAR_MAX;
}

bool is_last_route(const struct rr_route r)
{
    if (USHRT_MAX == r.departure_data &&
        USHRT_MAX == r.arrival_data &&
        UCHAR_MAX == r.delay_data)
        return true;
    return false;
}

#endif
