#include <limits.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>

#include "../include/route.h"

const char *path = "../include/data/schedule.xml";

// gcc xml_read.c -I/usr/include/libxml2 -lxml2
int main()
{
    
    // xmlKeepBlanksDefault();
    printf("%d\n", sizeof(struct rr_routes));
    xmlDocPtr document = xmlParseFile(path);

    xmlNodePtr entity = NULL;
    entity = xmlDocGetRootElement(document)->xmlChildrenNode;

    for (; entity; entity = entity->next)
    {
        if (0 == xmlStrcmp(entity->name, (const xmlChar *)"text"))
            continue;
        printf("%s\n", entity->name);
    }

    xmlFreeDoc(document);
    return EXIT_SUCCESS;
}
