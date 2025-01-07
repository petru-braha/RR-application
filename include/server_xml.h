#ifndef _00FILES00_
#define _00FILES00_

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>

#include "error.h"
#include "route.h"

// takes a path of the binary file
int write_xml(const char *const path)
{
    if (NULL == path)
    {
        error("write_xml() failed - null path");
        return EXIT_FAILURE;
    }

    pid_t process = fork();
    if (0 == process)
    {
        execv(path, NULL);
        error("write_xml() failed - execv()");
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
unsigned short count_routes = 0;

// takes a path of the file to be read
int read_xml(const char *const path)
{
    xmlDocPtr document = xmlParseFile(path);
    if (NULL == document)
    {
        error("read_xml() failed - wrong path.\n");
        exit(EXIT_FAILURE);
    }

    xmlNodePtr route_node;
    route_node = xmlDocGetRootElement(
                     document)
                     ->xmlChildrenNode->next;
    if (NULL == route_node)
    {
        error("read_xml() failed - invalid xml document.\n");
        xmlFreeDoc(document);
        exit(EXIT_FAILURE);
    }

    unsigned short index_route = 0;
    for (; route_node; route_node = route_node->next)
    {

        if (0 == xmlStrcmp(route_node->name, (const xmlChar *)"text"))
            continue;

        unsigned short time = 0;
        struct rr_route new_route = {0, 0, 0};
        xmlNodePtr data;
        xmlChar *pulled;

        // location
        data = route_node->xmlChildrenNode;
        while (data && 0 == xmlStrcmp(data->name, (const xmlChar *)"text"))
            data = data->next;
        if (NULL == data)
            continue;
        pulled = xmlNodeListGetString(document, data->xmlChildrenNode, 1);
        new_route.departure_data =
            (unsigned char)atoi((char *)pulled);
        xmlFree(pulled);
        if (new_route.departure_data >= COUNT_LOCATION) // error
            continue;

        // location
        data = data->next;
        while (data && 0 == xmlStrcmp(data->name, (const xmlChar *)"text"))
            data = data->next;
        if (NULL == data)
            continue;
        pulled = xmlNodeListGetString(document, data->xmlChildrenNode, 1);
        new_route.arrival_data =
            (unsigned char)atoi((char *)pulled);
        xmlFree(pulled);
        if (new_route.arrival_data >= COUNT_LOCATION) // error
            continue;

        // extra check
        if (new_route.departure_data ==
            new_route.arrival_data)
            continue;

        // time
        data = data->next;
        while (data && 0 == xmlStrcmp(data->name, (const xmlChar *)"text"))
            data = data->next;
        if (NULL == data)
            continue;
        pulled = xmlNodeListGetString(document, data->xmlChildrenNode, 1);
        time = (unsigned short)atoi((char *)pulled);
        xmlFree(pulled);
        if (time >= TIME_MAX) // error
            continue;
        new_route.departure_data += COUNT_LOCATION * time;

        // time
        data = data->next;
        while (data && 0 == xmlStrcmp(data->name, (const xmlChar *)"text"))
            data = data->next;
        if (NULL == data)
            continue;
        pulled = xmlNodeListGetString(document, data->xmlChildrenNode, 1);
        time = (unsigned short)atoi((char *)pulled);
        xmlFree(pulled);
        if (time >= TIME_MAX) // error
            continue;
        new_route.arrival_data += COUNT_LOCATION * time;

        // success
        schedule[index_route++] = new_route;
    }

    set_last(&schedule[index_route]);
    count_routes = index_route;
    xmlFreeDoc(document);
    return EXIT_SUCCESS;
}

int test_xml(const char *const path)
{
    xmlDocPtr document = xmlParseFile(path);
    if (NULL == document)
    {
        warning("read_xml() failed - wrong path.\n");
        return EXIT_FAILURE;
    }

    xmlNodePtr route_node;
    route_node = xmlDocGetRootElement(document)->xmlChildrenNode->next;
    if (NULL == route_node)
    {
        warning("read_xml() failed - invalid xml document.\n");
        xmlFreeDoc(document);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

#endif
