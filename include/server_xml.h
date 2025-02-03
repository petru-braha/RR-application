#ifndef _00GENERATE00_
#ifndef _00FILES00_
#define _00FILES00_

#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include <libxml2/libxml/xmlversion.h>
#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>

#include "error.h"
#include "route.h"

// take path of the binary file
int write_file(
    const char *const path,
    const char *const path_location);
int test_xml(const char *const path);
int read_xml(const char *const path);

struct rr_route schedule[COUNT_ROUTES_MAX];
unsigned short count_routes = 0;

int write_file(
    const char *const path,
    const char *const path_location)
{
  if (NULL == path || NULL == path_location)
  {
    error("write_file() failed - null path");
    return EXIT_FAILURE;
  }

  pid_t process = fork();
  if (0 == process)
  {
    execl(path, path_location, NULL);
    error("write_file() failed - execl()");
    return EXIT_FAILURE;
  }
  else
  {
    int rt;
    wait(&rt);
    return rt;
  }
}

int test_xml(const char *const path)
{
  xmlDocPtr document = xmlParseFile(path);
  if (NULL == document)
    return EXIT_FAILURE;

  xmlNodePtr route_node;
  route_node = xmlDocGetRootElement(document)->xmlChildrenNode->next;
  xmlFreeDoc(document);

  if (NULL == route_node)
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

int read_xml(const char *const path)
{
  xmlDocPtr document = xmlParseFile(path);
  if (NULL == document)
  {
    error("read_xml() failed - wrong path");
    exit(EXIT_FAILURE);
  }

  xmlNodePtr route_node;
  route_node = xmlDocGetRootElement(
                   document)
                   ->xmlChildrenNode->next;
  if (NULL == route_node)
  {
    error("read_xml() failed - invalid xml document");
    xmlFreeDoc(document);
    exit(EXIT_FAILURE);
  }

  unsigned short index_route = 0;
  for (; route_node; route_node = route_node->next)
  {

    if (0 == xmlStrcmp(route_node->name, (const xmlChar *)"text"))
      continue;

    unsigned short time = 0;
    struct rr_route new_route = {0, 0, 0, 0};
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
    new_route.id_train = index_route;
    schedule[index_route++] = new_route;
  }

  set_last(&schedule[index_route]);
  count_routes = index_route;
  xmlFreeDoc(document);
  return EXIT_SUCCESS;
}

#endif
#endif
