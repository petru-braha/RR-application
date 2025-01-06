#ifndef _00FILES00_
#define _00FILES00_

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>

#include "route.h"

int write_xml()
{
    pid_t process = fork();
    if (0 == process)
    {
        char *arguments[1];
        execv("../include/dev/write_xml", arguments);
        printf("error: execv failed.\n");
        return EXIT_FAILURE;
    }
    else
    {
        int rt;
        wait(&rt);
        return rt;
    }
}

struct rr_route schedule[COUNT_ROUTES_MAX];
int read_xml(const char *const path)
{
    // useful function witch doesn't work: xmlKeepBlanksDefault();
    xmlDocPtr document = xmlParseFile(path);
    if (NULL == document)
    {
        printf("error: read_xml() failed - wrong path.\n");
        exit(EXIT_FAILURE);
    }

    xmlNodePtr route_node;
    route_node = xmlDocGetRootElement(document)->xmlChildrenNode->next;
    if (NULL == route_node)
    {
        printf("error: read_xml() failed - invalid xml document.\n");
        xmlFreeDoc(document);
        exit(EXIT_FAILURE);
    }

    size_t index_route = 0;
    for (; route_node; route_node = route_node->next)
    {
        if (0 == xmlStrcmp(route_node->name, (const xmlChar *)"text"))
            continue;

        struct rr_route new_route;
        xmlNodePtr data;
        xmlChar *pulled;

        // location
        data = route_node->xmlChildrenNode;
        while (data && 0 == xmlStrcmp(data->name, (const xmlChar *)"text"))
            data = data->next;
        if (NULL == data)
            continue;
        pulled = xmlNodeListGetString(document, data->xmlChildrenNode, 1);
        new_route.location_departure =
            (unsigned char)atoi((char *)pulled);
        xmlFree(pulled);
        if (new_route.location_departure >= COUNT_LOCATION) // error
            continue;

        // location
        data = data->next;
        while (data && 0 == xmlStrcmp(data->name, (const xmlChar *)"text"))
            data = data->next;
        if (NULL == data)
            continue;
        pulled = xmlNodeListGetString(document, data->xmlChildrenNode, 1);
        new_route.location_arrival =
            (unsigned char)atoi((char *)pulled);
        xmlFree(pulled);
        if (new_route.location_arrival >= COUNT_LOCATION) // error
            continue;

        // time
        data = data->next;
        while (data && 0 == xmlStrcmp(data->name, (const xmlChar *)"text"))
            data = data->next;
        if (NULL == data)
            continue;
        pulled = xmlNodeListGetString(document, data->xmlChildrenNode, 1);
        new_route.time_departure =
            (unsigned short)atoi((char *)pulled);
        xmlFree(pulled);
        if (new_route.time_departure >= TIME_MAX) // error
            continue;

        // time
        data = data->next;
        while (data && 0 == xmlStrcmp(data->name, (const xmlChar *)"text"))
            data = data->next;
        if (NULL == data)
            continue;
        pulled = xmlNodeListGetString(document, data->xmlChildrenNode, 1);
        new_route.time_arrival =
            (unsigned short)atoi((char *)pulled);
        xmlFree(pulled);
        if (new_route.time_arrival >= TIME_MAX) // error
            continue;

        schedule[index_route++] = new_route;
    }

    set_last_r(&schedule[index_route]);
    xmlFreeDoc(document);
    return EXIT_SUCCESS;
}

#endif
