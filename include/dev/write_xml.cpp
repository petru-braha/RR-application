/* comments:
 * write_xml.cpp - a route random generator application
 * author - Braha Petru Bogdan - <petrubraha@gmail.com> (c)
 * compilation command: g++ -std=c++11 include/dev/write_xml.cpp -I/usr/include/libxml2 -lxml2 -o include/dev/write_xml
 */

#include <stdio.h>
#include <string.h>

#include <unordered_map>
#include <libxml2/libxml/parser.h>

#include "generator.hpp"

typedef unsigned short usht;

struct ez_route
{
	usht departure_location = 0;
	usht departure_time = 0;
	usht arrival_location = 0;
	usht arrival_time = 0;
};

void select_restriction(const usht, usht *const);
void location_generation(std::vector<ez_route> &);
void schedule_generation(std::vector<ez_route> &);
void document_generation(const char *const);
int main(int argc, char *argv[]);

constexpr usht COUNT_ROUTES_MAX = 10000;
constexpr usht COUNT_LOCATION = 41;
constexpr usht BB = 24;
constexpr usht CJ = 12;
constexpr usht IS = 23;

constexpr usht G_MINIMUM_CITY = 1;
constexpr usht G_MAXIMUM_CITY =
		COUNT_LOCATION * 2 / 3;

constexpr usht G_MINIMUM_TIME = 0;
constexpr usht G_MAXIMUM_TIME = 1440;

constexpr usht G_MINIMUM_ALTV = 2;
constexpr usht G_MAXIMUM_ALTV = 6;

random_generator g;

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

	xmlNodePtr node = nullptr;
	for (usht i = 0; i < schedule.size(); i++)
	{
		char buffer[10];

		xmlNewTextChild(root_node, NULL,
										(const xmlChar *)"route",
										nullptr);
		if(0 == i)
			node = root_node->xmlChildrenNode;
		if(node->next)
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

	printf("%zu routes were generated.\n", schedule.size());
	xmlSaveFormatFile(path, document, 1);
	xmlFreeDoc(document);
	return EXIT_SUCCESS;
}

// the first element is invalid
void select_restriction(const usht count_index, usht *const results)
{
	// base cases
	if (nullptr == results)
		return;
	if (count_index >= COUNT_LOCATION)
		return;

	// todo change to std::make_index_array
	usht index_array[COUNT_LOCATION];
	for (usht i = 0; i < COUNT_LOCATION; i++)
		index_array[i] = i;

	// be aware of the invalid element
	const usht invalid = results[0];
	index_array[invalid] = 0;
	index_array[0] = invalid;

	usht it_index_array = 1;
	for (; it_index_array <= count_index; it_index_array++)
	{
		const usht it_index_chosen =
				g(it_index_array, COUNT_LOCATION - 1);
		const usht answer = index_array[it_index_chosen];

		index_array[it_index_chosen] =
				index_array[it_index_array];
		index_array[it_index_array] = answer;

		results[it_index_array] = answer;
	}
}

void location_generation(std::vector<ez_route> &schedule)
{
	std::unordered_map<int, std::vector<int>> locations;
	for (usht index_city = 0; index_city < COUNT_LOCATION; index_city++)
	{
		// add extra routes for big cities
		usht G_MIN = G_MINIMUM_CITY,
				 G_MAX = G_MAXIMUM_CITY;
		if (BB == index_city ||
				CJ == index_city ||
				IS == index_city)
		{
			G_MIN += COUNT_LOCATION - G_MAXIMUM_CITY;
			G_MAX += COUNT_LOCATION - G_MAXIMUM_CITY;
		}

		const usht count_arrival_location = g(G_MIN, G_MAX);

		// create index array from 0 to 40
		usht arrival_location[COUNT_LOCATION];
		for (usht i = 0; i < count_arrival_location; i++)
			arrival_location[i] = i;

		// make sure there are no pair of the same number, e. g. 0 0
		arrival_location[index_city] = 0;
		arrival_location[0] = index_city;
		select_restriction(count_arrival_location, arrival_location);

		// include the random results into locations
		locations[index_city].reserve(count_arrival_location);
		for (usht i = 1; i <= count_arrival_location; i++)
			locations.at(index_city).emplace_back(arrival_location[i]);
	}

	size_t total_size = 0;
	for (const auto &pair : locations)
		total_size += pair.second.size();
	if (total_size * G_MAXIMUM_ALTV >= COUNT_ROUTES_MAX)
	{
		printf("write_xml() failed - too many entries.\n");
		exit(EXIT_FAILURE);
	}

	for (usht index_city = 0;
			 index_city < COUNT_LOCATION;
			 index_city++)
		for (usht i_arrival = 0;
				 i_arrival < locations.at(index_city).size() &&
				 schedule.size() < COUNT_ROUTES_MAX;
				 i_arrival++)
		{
			ez_route route;
			route.departure_location = index_city;
			route.arrival_location = locations.at(index_city).at(i_arrival);
			schedule.emplace_back(route);
		}
}

void analysis(std::vector<ez_route> &schedule)
{
	size_t counter[COUNT_LOCATION];
	for (size_t i = 0; i < COUNT_LOCATION; i++)
		counter[i] = 0;
	for (size_t i = 0; i < schedule.size(); i++)
		counter[schedule[i].departure_location]++;
	for (size_t i = 0; i < COUNT_LOCATION; i++)
		printf("%zu:%zu ", i, counter[i]);
	printf("\n");
}

void schedule_generation(std::vector<ez_route> &schedule)
{
	// analysis(schedule);
	for (usht index_route = 0;
			 index_route < schedule.size();
			 index_route++)
	{
		ez_route &ref = schedule[index_route];
		ref.departure_time =
				g(G_MINIMUM_TIME, G_MAXIMUM_TIME);
		ref.arrival_time =
				g(G_MINIMUM_TIME, G_MAXIMUM_TIME);
	}

	usht count_alternatives = g(G_MINIMUM_ALTV, G_MAXIMUM_ALTV);
	while (count_alternatives)
	{
		std::vector<ez_route> data = schedule;
		for (usht index_route = 0;
				 index_route < data.size();
				 index_route++)
		{
			ez_route &ref = data[index_route];
			ref.departure_time =
					g(G_MINIMUM_TIME, G_MAXIMUM_TIME);
			ref.arrival_time =
					g(G_MINIMUM_TIME, G_MAXIMUM_TIME);
		}

		schedule.insert(schedule.end(),
										data.begin(), data.end());
		count_alternatives--;
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
