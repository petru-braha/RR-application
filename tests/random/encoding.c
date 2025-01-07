#include <stdio.h>
#include <string.h>

#define COUNT_LOCATION 41
#define BYTES_COMMAND_MAX 40

void repair(unsigned char *const buffer)
{
    if (NULL == buffer)
    {
        // error("repair() failed - null buffer");
        return;
    }

    size_t index = strlen(buffer);
    if (index)
        buffer[index - 1] = '\0';
}

unsigned short convert_location(const unsigned char *const buffer)
{
    FILE *file = fopen("../../include/data/location.txt", "r");
    if (NULL == file)
    {
        // error("convert_location() failed - fopen()");
        return COUNT_LOCATION;
    }

    char location[BYTES_COMMAND_MAX];
    for (unsigned short index_location = 0;
         index_location < COUNT_LOCATION;
         index_location++)
    {
        if (location != fgets(location, BYTES_COMMAND_MAX, file))
        {
            // error("convert_location() failed - fgets()");
            fclose(file);
            return COUNT_LOCATION;
        }

        repair(location);
        if (0 == strcmp(buffer, location))
        {
            fclose(file);
            return index_location;
        }
    }

    fclose(file);
    return COUNT_LOCATION;
}

// success
int main(int argc, char *argv[])
{
    if (2 != argc)
        return 1;

    printf("%d\n", convert_location(argv[1]));
    return 0;
}
