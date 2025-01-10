#include <stdio.h>
#include <stdbool.h>

bool f0()
{
    printf("yes.\n");
    return true;
}

bool f1()
{
    printf("yes.\n");
    return false;
}

// success
int main()
{
    // it is expected to print only once "yes."
    if (f0() || f1())
        return 0;
    return 1;
}
