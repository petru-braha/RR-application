#include <unistd.h>
#include "../../include/printer.h"

#define RECV_FAIL 0
const char *const path_location =
    "../../include/data/location.txt";

static ssize_t print_data(const unsigned char command,
                          const unsigned short argument0,
                          const struct rr_route *const data)
{
    if (TCP_CODE_R == command)
    {
        if (RECV_FAIL == argument0)
            printf("reporting failed.\n");
        else
            printf("reporting was correctly performed.\n");
        return sizeof(unsigned char);
    }

    if (TCP_CODE_Q == command)
    {
        if (RECV_FAIL == argument0)
            printf("exiting failed to fetch with server.\n");
        else
            printf("exiting was correctly performed.\n");
        return sizeof(unsigned char);
    }

    if (NULL == data && argument0)
    {
        error("print_data() failed - null data");
        return -1;
    }

    // UDP commands
    printf("%d routes received.\n", argument0);
    for (unsigned short i = 0; i < argument0; i++)
        client_print(data[i], path_location);
    return argument0 * sizeof(struct rr_route);
}

void simple_print()
{
    unsigned short d_data = 32 + 41 * 1380;
    struct rr_route r = {d_data, 0, 0};
    client_print(r, "../../include/data/location.txt");
}

const unsigned short number = 3;

void multiple_print()
{
    struct rr_route a = {60 * 2 * 41 + 2, 60 * 3 * 41 + 4, 0},
                    b = {60 * 5 * 41 + 5, 60 * 10 * 41 + 7, 0},
                    c = {65 * 41 + 34, 130 * 41 + 25, 0};

    struct rr_route arr[number];
    arr[0] = a;
    arr[1] = b;
    arr[2] = c;
    print_data(UDP_CODE_A, number, arr);
}

// ran at 18:10
int main()
{
    // not departed case
    {
        // 23:00 -> 23:05
        struct rr_route a = {60 * 23 * 41 + 2, (60 * 23 + 5) * 41 + 4, DELEY_MAX},
                        b = {60 * 23 * 41 + 5, (60 * 23 + 5) * 41 + 7, 10},
                        c = {60 * 23 * 41 + 3, (60 * 23 + 5) * 41 + 0, 0};

        struct rr_route arr[number];
        arr[0] = a;
        arr[1] = b;
        arr[2] = c;
        print_data(UDP_CODE_A, number, arr);
        printf("\n");
    }

    // departed but not arrived case
    {
        // 14:00 -> 23:05
        struct rr_route a = {60 * 14 * 41 + 2, (60 * 23 + 5) * 41 + 4, DELEY_MAX},
                        b = {60 * 14 * 41 + 5, (60 * 23 + 5) * 41 + 7, 10},
                        c = {60 * 14 * 41 + 3, (60 * 23 + 5) * 41 + 0, 0};

        struct rr_route arr[number];
        arr[0] = a;
        arr[1] = b;
        arr[2] = c;
        print_data(UDP_CODE_A, number, arr);
        printf("\n");
    }

    // departed and supposedly arrived case
    {
        // 14:00 -> 17:05
        struct rr_route a = {60 * 14 * 41 + 2, (60 * 17 + 47) * 41 + 4, DELEY_MAX},
                        b = {60 * 14 * 41 + 5, (60 * 17 + 47) * 41 + 7, 10},
                        c = {60 * 14 * 41 + 3, (60 * 17 + 47) * 41 + 0, 0};

        struct rr_route arr[number];
        arr[0] = a;
        arr[1] = b;
        arr[2] = c;
        print_data(UDP_CODE_A, number, arr);
        printf("\n");
    }
}
