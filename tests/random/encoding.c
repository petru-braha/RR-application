#include "../../include/printer.h"

// success
int main(int argc, char *argv[])
{
    if (2 != argc)
        return 1;

    unsigned short code = 0;
    stringtocode(argv[1], &code, "../../include/data/location.txt");
    printf("%d\n", code);
    return 0;
}
