#include <stdio.h>
#include <string.h>

#include <unordered_map>
#include <libxml2/libxml/parser.h>

#include "generator.hpp"
#include "../route.h"

using std::unordered_map;
using std::vector;

random_generator g;

constexpr int BB = 0;
constexpr int CJ = 13;
constexpr int IS = 24;

constexpr size_t G_MINIMUM_CITY = 1;
constexpr size_t G_MAXIMUM_CITY =
	COUNT_LOCATION * 2 / 3;

constexpr size_t G_MINIMUM_TIME = 0;
constexpr size_t G_MAXIMUM_TIME = 1440;

constexpr size_t G_MINIMUM_ALTV = 2;
constexpr size_t G_MAXIMUM_ALTV = 6;

int rate(const size_t);
void select(const size_t, size_t *const);
void location_generation(std::vector<rr_route> &);
void schedule_generation(std::vector<rr_route> &);
void document_generation(const char *const);

// g++ write_xml.cpp -I/usr/include/libxml2 -lxml2 -o write_xml
int main()
{
	size_t count_schedule = 0;
	std::vector<rr_route> schedule(COUNT_LOCATION);
	const char path[] =
		"../include/data/random schedule.xml";

	location_generation(schedule);
	schedule_generation(schedule);
	document_generation(path);

	// actually add to document
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

	xmlNewTextChild(root_node, NULL,
					(const xmlChar *)"schedule",
					(const xmlChar *)"0");
	xmlNodePtr node =
		root_node->xmlChildrenNode->next;

	for (size_t i = 0; i < schedule.size(); i++)
	{
		char buffer[10];

		xmlNewTextChild(root_node, NULL,
						(const xmlChar *)"route",
						nullptr);

		sprintf(buffer, "%zu",
				(size_t)schedule[i].location_departure);
		xmlNewTextChild(node, NULL,
						(const xmlChar *)"location_departure",
						(const xmlChar *)buffer);

		sprintf(buffer, "%zu",
				(size_t)schedule[i].location_arrival);
		xmlNewTextChild(node, NULL,
						(const xmlChar *)"location_arrival",
						(const xmlChar *)buffer);

		sprintf(buffer, "%zu",
				(size_t)schedule[i].time_departure);
		xmlNewTextChild(node, NULL,
						(const xmlChar *)"time_departure",
						(const xmlChar *)buffer);

		sprintf(buffer, "%zu",
				(size_t)schedule[i].time_arrival);
		xmlNewTextChild(node, NULL,
						(const xmlChar *)"time_arrival",
						(const xmlChar *)buffer);

		node = node->next;
	}

	printf("%zu routes were generated.\n", schedule.size());
	xmlSaveFormatFile(path, document, 1);
	xmlFreeDoc(document);
	return EXIT_SUCCESS;
}

int rate(const size_t city)
{
	size_t minimum = 0, maximum = COUNT_LOCATION / 2;
	if (city == BB || city == IS || city == CJ)
	{
		minimum += 3;
		maximum += 3;
	}

	return g(minimum, maximum);
}

// randomly select count_index cities in linear time without repetition
void select(const size_t count_index, size_t *const results)
{
	if (nullptr == results)
	{
		printf("error: write_xml failed - select().\n");
		exit(EXIT_FAILURE);
	}

	if (count_index >= COUNT_LOCATION)
		return;

	size_t index_array[COUNT_LOCATION];
	for (size_t i = 0; i < COUNT_LOCATION; i++)
		index_array[i] = i;

	size_t it_index_array = 0;
	for (; it_index_array < count_index; it_index_array++)
	{
		const size_t it_index_chosen =
			g(it_index_array, count_index - 1);
		const size_t answer = index_array[it_index_chosen];

		index_array[it_index_chosen] =
			index_array[it_index_array];
		index_array[it_index_array] = answer;

		results[it_index_array] = answer;
	}
}

void location_generation(std::vector<rr_route> &schedule)
{
	unordered_map<int, vector<int>> locations;
	for (size_t index_city = 0; index_city < COUNT_LOCATION; index_city++)
	{
		const size_t count_arrival_location = g(G_MINIMUM_CITY, G_MAXIMUM_CITY);
		size_t arrival_location[COUNT_LOCATION]{};
		for (size_t i = 0; i < count_arrival_location; i++)
			arrival_location[i] = i;
		select(count_arrival_location, arrival_location);

		locations[index_city] = vector<int>();
		for (size_t i = 0; i < count_arrival_location; i++)
			locations.at(index_city).emplace_back(arrival_location[i]);
	}

	schedule = std::vector<rr_route>();
	for (size_t index_city = 0;
		 index_city < COUNT_LOCATION &&
		 schedule.size() < COUNT_ROUTES_MAX;
		 index_city++)
		for (size_t i_arrival = 0;
			 i_arrival < locations.at(index_city).size() &&
			 schedule.size() < COUNT_ROUTES_MAX;
			 i_arrival++)
		{
			rr_route route{0, 0, 0, 0};
			route.location_departure = index_city;
			route.location_arrival = locations.at(index_city).at(i_arrival);
			schedule.emplace_back(route);
		}
}

void schedule_generation(std::vector<rr_route> &schedule)
{
	size_t count_schedule = schedule.size();
	size_t count_alternatives = g(G_MINIMUM_ALTV, G_MAXIMUM_ALTV);
	size_t index_schedule_global = 0;
	for (size_t index_route = 0;
		 index_route < count_schedule &&
		 schedule.size() < COUNT_ROUTES_MAX;
		 index_route++)
	{
		schedule[index_route].time_departure =
			(unsigned short)g(G_MINIMUM_TIME, G_MAXIMUM_TIME);

		// arrival hour
		schedule[index_route].time_arrival =
			(unsigned short)g(G_MINIMUM_TIME, G_MAXIMUM_TIME);

		for (size_t i = 0; i < count_alternatives; i++)
		{
			rr_route route = schedule.at(index_route);
			route.time_departure =
				(unsigned short)g(G_MINIMUM_TIME, G_MAXIMUM_TIME);
			route.time_arrival =
				(unsigned short)g(G_MINIMUM_TIME, G_MAXIMUM_TIME);
			schedule.insert(schedule.begin() + index_schedule_global, route);
			index_schedule_global++;
		}

		index_schedule_global++;
	}
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

/*
- task:
	- generate count of departure city
	- generate count arrival
	- generate hour

- constraints:
	- not compulsory to be each hour generated
	- not from a location to the same one
	- geographycal location??
	- iasi cluj bucharest - higher probability

	- alternatives - each route to have ++ hours generated [2, 6]
	- diversity - in one day arrive from anywhere to anywhere

	- location
		- departure attribute [1 + a, 10 + b]
		- arrival atribute [1 + a, 10 + b]
*/
