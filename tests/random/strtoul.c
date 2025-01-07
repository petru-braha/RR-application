#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

// success
int main()
{
    char buffer[20];
    char *invalid_address = NULL;

    strcpy(buffer, "10");
    printf("%u ", strtoul(buffer, &invalid_address, 10));
    if ('\0' == *invalid_address)
        printf("success\n");
    else
    {
        printf("error\n");
        invalid_address = NULL;
    }

    strcpy(buffer, "-10");
    printf("%d ", strtoul(buffer, &invalid_address, 10));
    if ('\0' == *invalid_address)
        printf("success\n");
    else
    {
        printf("error\n");
        invalid_address = NULL;
    }

    strcpy(buffer, "+10");
    printf("%d ", strtoul(buffer, &invalid_address, 10));
    if ('\0' == *invalid_address)
        printf("success\n");
    else
    {
        printf("error\n");
        invalid_address = NULL;
    }

    strcpy(buffer, "10.0");
    printf("%u ", strtoul(buffer, &invalid_address, 10));
    if ('\0' == *invalid_address)
        printf("success\n");
    else
    {
        printf("error\n");
        invalid_address = NULL;
    }

    strcpy(buffer, "  6text7");
    printf("%u ", strtoul(buffer, &invalid_address, 10));
    if ('\0' == *invalid_address)
        printf("success\n");
    else
    {
        printf("error\n");
        invalid_address = NULL;
    }

    return 0;
}
