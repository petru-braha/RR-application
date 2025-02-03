/* comments:
 * write_xml.cpp - a route random generator application
 * author - Braha Petru Bogdan - <petrubraha@gmail.com> (c)
 * compilation command: g++ -std=c++11 include/dev/write_xml.cpp -I/usr/include/libxml2 -lxml2 -o include/dev/write_xml
 */

#include <stdio.h>
#include <string.h>
#include <libxml2/libxml/parser.h>
#include "write_file.hpp"

void select_restriction(const usht, usht *const);
void location_generation(std::vector<ez_route> &);
void schedule_generation(std::vector<ez_route> &);
void document_annexation(const char *const, std::vector<ez_route> &);
int main(int argc, char *argv[]);

extern random_generator g;

int main(int argc, char *argv[])
{
	if (1 != argc)
	{
		printf("wrong number of arguments");
		return EXIT_FAILURE;
	}

	const char *const path = argv[0];
	usht count_schedule = 0;
	std::vector<ez_route> schedule;
	schedule.reserve(COUNT_LOCATION);

	location_generation(schedule);
	schedule_generation(schedule);
	printf("%zu routes were generated.\n", schedule.size());

	document_annexation(path, schedule);
	return EXIT_SUCCESS;
}

void document_generation(const char *const path)
{
	FILE *xml = fopen(path, "w");
	if (nullptr == xml)
	{
		printf("error: write_xml failed - fopen().\n");
		exit(EXIT_FAILURE);
	}

	const char s0[] = "<schedule>\n", s1[] = "</schedule>";
	if (0 == fwrite(s0, sizeof(char), strlen(s0), xml))
	{
		printf("error: write_xml failed - fwrite().\n");
		exit(EXIT_FAILURE);
	}

	if (0 == fwrite(s1, sizeof(char), strlen(s1), xml))
	{
		printf("error: write_xml failed - fwrite().\n");
		exit(EXIT_FAILURE);
	}

	if (fclose(xml))
	{
		printf("error: write_xml failed - fclose().\n");
		exit(EXIT_FAILURE);
	}
}

void document_annexation(
		const char *const path,
		std::vector<ez_route> &schedule)
{
	document_generation(path);
	xmlDocPtr document = xmlParseFile(path);
	if (NULL == document)
	{
		printf("error: write_xml - wrong path.\n");
		exit(EXIT_FAILURE);
	}

	xmlNodePtr root_node;
	root_node = xmlDocGetRootElement(document);
	if (NULL == root_node)
	{
		printf("error: write_xml - invalid xml document.\n");
		xmlFreeDoc(document);
		exit(EXIT_FAILURE);
	}

	xmlNodePtr node = nullptr;
	for (usht i = 0; i < schedule.size(); i++)
	{
		char buffer[10];

		xmlNewTextChild(root_node, NULL,
										(const xmlChar *)"route",
										nullptr);
		if (0 == i)
			node = root_node->xmlChildrenNode;
		if (node->next)
			node = node->next;

		sprintf(buffer, "%u", schedule[i].departure_location);
		xmlNewTextChild(node, NULL,
										(const xmlChar *)"location_departure",
										(const xmlChar *)buffer);

		sprintf(buffer, "%u", schedule[i].arrival_location);
		xmlNewTextChild(node, NULL,
										(const xmlChar *)"location_arrival",
										(const xmlChar *)buffer);

		sprintf(buffer, "%u", schedule[i].departure_time);
		xmlNewTextChild(node, NULL,
										(const xmlChar *)"time_departure",
										(const xmlChar *)buffer);

		sprintf(buffer, "%u", schedule[i].arrival_time);
		xmlNewTextChild(node, NULL,
										(const xmlChar *)"time_arrival",
										(const xmlChar *)buffer);
	}

	xmlSaveFormatFile(path, document, 1);
	xmlFreeDoc(document);
}
