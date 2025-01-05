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
    xmlDocPtr document = xmlParseFile(path);

    xmlNodePtr route = NULL;
    route = xmlDocGetRootElement(document)->xmlChildrenNode;

    for (; route; route = route->next)
    {
        //struct rr_route route;
        if (0 == xmlStrcmp(route->name, (const xmlChar *)"text"))
            continue;
        
        // id_train
        xmlChar* key = NULL;
        xmlNodePtr data = route->xmlChildrenNode->next;
        key = xmlNodeListGetString(document, data->xmlChildrenNode, 1);
        printf("keyword: %s\n", key);
        xmlFree(key);

        // location
        data = route->next;
        key = xmlNodeListGetString(document, data->xmlChildrenNode, 1);
        printf("keyword: %s\n", key);
        xmlFree(key);
        break;

        data = route->next;

        // time


    }

    xmlFreeDoc(document);
    return EXIT_SUCCESS;
}
