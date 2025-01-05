#ifndef _00FILES00_
#define _00FILES00_

#include <sys/types.h>
#include <unistd.h>

#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>

#include "../include/route.h"

struct rr_route routes[COUNT_ROUTES_MAX];

int write_xml()
{
    pid_t process = fork();
    if (0 == process)
    {
        execv("write_xml", NULL);
        printf("error: execv failed.\n");
        return EXIT_FAILURE;
    }
    else
    {
        int rt;
        wait(&rt);
    }
}

int read_xml(const char *const path)
{
    // useful function witch doesn't work: xmlKeepBlanksDefault();
    xmlDocPtr document = xmlParseFile(path);
    if (NULL == document)
    {
        printf("error: wrong path.\n");
        exit(EXIT_FAILURE);
    }

    xmlNodePtr route_node;
    route_node = xmlDocGetRootElement(document)->xmlChildrenNode;
    if (NULL == route_node)
    {
        printf("error: invalid xml document.\n");
        xmlFreeDoc(document);
        exit(EXIT_FAILURE);
    }

    for (; route_node; route_node = route_node->next)
    {
        if (0 == xmlStrcmp(route_node->name, (const xmlChar *)"text"))
            continue;

        struct rr_route route;
        xmlNodePtr data;
        xmlChar *pulled;

        // location
        data = route_node->xmlChildrenNode->next;
        pulled = xmlNodeListGetString(document, data->xmlChildrenNode, 1);
        route.location_departure =
            (unsigned char)atoi((char *)pulled);
        xmlFree(pulled);
        if (route.location_departure >= COUNT_LOCATION) // error
            continue;

        data = data->next->next;
        pulled = xmlNodeListGetString(document, data->xmlChildrenNode, 1);
        route.location_arrival =
            (unsigned char)atoi((char *)pulled);
        xmlFree(pulled);
        if (route.location_arrival >= COUNT_LOCATION) // error
            continue;

        // time
        data = data->next->next;
        pulled = xmlNodeListGetString(document, data->xmlChildrenNode, 1);
        route.time_departure =
            (unsigned short)atoi((char *)pulled);
        xmlFree(pulled);
        if (route.time_departure >= TIME_MAX) // error
            continue;

        data = data->next->next;
        pulled = xmlNodeListGetString(document, data->xmlChildrenNode, 1);
        route.time_arrival =
            (unsigned short)atoi((char *)pulled);
        xmlFree(pulled);
        if (route.time_arrival >= TIME_MAX) // error
            continue;
    }

    xmlFreeDoc(document);
    return EXIT_SUCCESS;
}

#endif
