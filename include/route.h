#ifndef _00ITINERARY00_
#define _00ITINERARY00_

#define COUNT_LOCATION 41
#define LOCATION_MIN 0
#define LOCATION_MAX 40

#define TIME_MIN 0
#define TIME_MAX 1440

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

#endif
