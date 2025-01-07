#include "../../include/printer.h"

// success
int main()
{
    unsigned short d_data = 32 + 41 * 1380;
    struct rr_route r = {d_data, 0, 0};
    client_print(r, "../../include/data/location.txt");
}
