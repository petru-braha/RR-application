#include <stdio.h>
#include <string.h>

#include "../../include/printer.h"

// success
int main()
{
    for (unsigned char i = 0; i < COUNT_LOCATION; i++)
    {
        char buffer[BYTES_COMMAND_MAX];
        codetostring(buffer, i, "../../include/data/location.txt");
        printf("%s\n", buffer);
    }
}
