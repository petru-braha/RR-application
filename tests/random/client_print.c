#include "../../include/printer.h"

void computation()
{
    // 41 * 41 * 1440 * 1440 iterations
    for (unsigned char l_d = 0; l_d < COUNT_LOCATION; l_d++)
        for (unsigned char l_a = 0; l_a < COUNT_LOCATION; l_a++)
            for (unsigned short t_d = 0; t_d < TIME_MAX; t_d++)
                for (unsigned short t_a = 0; t_a < TIME_MAX; t_a++)
                {
                    struct rr_route r = {l_d, l_a, t_d, t_a};
                    client_print(r, "../../include/data/location.txt");
                }
}

// success
int main()
{
    struct rr_route r = {0, 0, 0, 0};
    client_print(r, "../../include/data/location.txt");
}
