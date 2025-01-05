#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml2/libxml/parser.h>

// gcc test_insert.c -I/usr/include/libxml2 -lxml2
int main()
{
    const char path[] = "../include/data/test schedule.xml";
    FILE *xml = fopen(path, "w");
    if (NULL == xml)
    {
        printf("error: wrong path.\n");
        exit(EXIT_FAILURE);
    }

    const char s0[] = "<schedule>\n", s1[] = "</schedule>";
    if (0 == fwrite(s0, sizeof(char), strlen(s0), xml))
    {
        printf("error: could not initialise file.\n");
        exit(EXIT_FAILURE);
    }

    if (0 == fwrite(s1, sizeof(char), strlen(s1), xml))
    {
        printf("error: could not initialise file.\n");
        exit(EXIT_FAILURE);
    }

    if (fclose(xml))
    {
        printf("error: could not close the file.\n");
        exit(EXIT_FAILURE);
    }

    xmlDocPtr document = xmlParseFile(path);
    if (NULL == document)
    {
        printf("error: wrong path.\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
