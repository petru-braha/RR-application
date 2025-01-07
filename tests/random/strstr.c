#include <stdio.h>
#include <string.h>

// success
int main()
{
    const char buffer[] = "report 0 59596";
    if (strstr(buffer, "report") == buffer)
        printf("success.\n");
    else
        printf("failure.\n");
}
