#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml2/libxml/parser.h>

// gcc test_insert.c -I/usr/include/libxml2 -lxml2
int main()
{
    // initialise the file
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

    // actual open
    xmlDocPtr document = xmlParseFile(path);
    if (NULL == document)
    {
        printf("error: wrong path.\n");
        exit(EXIT_FAILURE);
    }

    xmlNodePtr root_node;
    root_node = xmlDocGetRootElement(document);
    if (NULL == root_node)
    {
        printf("error: invalid xml document.\n");
        xmlFreeDoc(document);
        exit(EXIT_FAILURE);
    }

    xmlNewTextChild(root_node, NULL, "routes", "0");
    xmlNodePtr node = root_node->xmlChildrenNode->next;
    for (size_t i = 0; i < 10; i++)
    {
        char buffer[5];
        sprintf(buffer, "%zu", i);
        xmlNewTextChild(root_node, NULL, "routes", "");
        xmlNewTextChild(node, NULL, "location_departure", "8");
        xmlNewTextChild(node, NULL, "location_arrival", "8");
        xmlNewTextChild(node, NULL, "time_departure", "8");
        xmlNewTextChild(node, NULL, "time_arrival", "8");
        node = node->next;
    }

    xmlSaveFormatFile(path, document, 1);
    xmlFreeDoc(document);
    return EXIT_SUCCESS;
}
