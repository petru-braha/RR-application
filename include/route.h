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

// NDNA == not departed not arrived
#define STATUS_NDNA 0
#define STATUS_YDNA 100
#define STATUS_YDYA 200
#define STATUS_OKAY 0
#define STATUS_LATE 99
#define STATUS_FAIL 199

struct rr_route
{
    unsigned char location_departure;
    unsigned char location_arrival;
    unsigned short time_departure;
    unsigned short time_arrival;
};

void set_last(struct rr_route *r)
{
    if (NULL == r)
        warning("set_last() failed - invalid pointer");

    r->location_departure = UCHAR_MAX;
    r->location_arrival = UCHAR_MAX;
    r->time_departure = USHRT_MAX;
    r->time_arrival = USHRT_MAX;
}

bool is_last_route(struct rr_route r)
{
    if (UCHAR_MAX == r.location_departure &&
        UCHAR_MAX == r.location_arrival &&
        USHRT_MAX == r.time_departure &&
        USHRT_MAX == r.time_arrival)
        return true;
    return false;
}

#endif
