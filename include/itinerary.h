#ifndef _00ITINERARY00_
#define _00ITINERARY00_

struct rr_status
{
    bool status_departure;
    bool status_arrival;
};

struct rr_time
{
    int hours;
    int minutes;
};

struct rr_route
{
    int id_train;
    int location_departure;
    int location_arrival;
    struct rr_time time_departure;
    struct rr_time time_arrival;
    struct rr_status status;
};

#endif
