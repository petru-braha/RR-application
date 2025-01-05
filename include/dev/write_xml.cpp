#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unordered_map>
#include <libxml2/libxml/parser.h>

#include "dev/generator.hpp"
#include "route.h"

using std::unordered_map;
using std::vector;

random_generator g;

constexpr int BB = 0;
constexpr int CJ = 13;
constexpr int IS = 24;

constexpr size_t G_MINIMUM_CITY = 1;
constexpr size_t G_MAXIMUM_CITY = COUNT_LOCATION / 2;

constexpr size_t G_MINIMUM_TIME = 0;
constexpr size_t G_MAXIMUM_TIME = 1440;

constexpr size_t G_MINIMUM_ALTV = 2;
constexpr size_t G_MAXIMUM_ALTV = 6;

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

void city_select(const size_t count_index, size_t *results);

void location_generation(size_t &count_routes, std::vector<rr_route> &routes)
{
	// only location generation
	unordered_map<int, vector<int>> locations(COUNT_LOCATION);
	for (size_t index_city = 0; index_city < COUNT_LOCATION; index_city++)
	{
		const size_t count_arrival_location = g(G_MINIMUM_CITY, G_MAXIMUM_CITY);
		size_t arrival_location[COUNT_LOCATION]{};
		city_select(count_arrival_location, arrival_location);
		for (size_t i = 0; i < count_arrival_location; i++)
			locations[index_city].emplace_back(arrival_location[i]);
	}

	// init data structure
	for (size_t index_city = 0; index_city < COUNT_LOCATION; index_city++)
		count_routes += locations[index_city].size();

	size_t index_route = 0;
	std::vector<rr_route> routes(count_routes);
	for (size_t index_city = 0; index_city < COUNT_LOCATION; index_city++)
	{
		routes[index_route].location_departure = index_city;
		for (size_t i_arrival = 0; i_arrival < locations[index_city].size(); i_arrival++)
		{
			routes[index_route].location_arrival = locations[index_city].at(i_arrival);
			index_route++;
		}
	}
}

int main()
{
	size_t count_routes = 0;
	std::vector<rr_route> routes(COUNT_LOCATION);

	location_generation(count_routes, routes);

	// time, alternative generation
	size_t count_alternatives = g(G_MINIMUM_ALTV, G_MAXIMUM_ALTV);
	count_routes *= count_alternatives;
	for (size_t index_route = 0; index_route < count_routes; index_route++)
	{
		// departure hour
		int time = (int)g(G_MINIMUM_TIME, G_MAXIMUM_TIME);
		routes[index_route].time_departure;

		// arrival hour
		time = (int)g(G_MINIMUM_TIME, G_MAXIMUM_TIME);
		routes[index_route].time_arrival = time;

		// routes[index_route].status = STATUS_NDNA;
		rr_route route = routes.at(index_route);
		routes.insert(routes.begin() + index_route, {route});
	}

	const char path[] = "random schedule.xml";

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

	xmlDocPtr document = xmlParseFile(path);
	if (NULL == document)
	{
		printf("error: wrong path.\n");
		exit(EXIT_FAILURE);
	}

	for (size_t i = 0; i < routes.size(); i++)
	{
		// add <route>
		// xmlNewTextChild
		// add <schedule>

		// add <>

		// add </route>
	}
}

// randomly select count_index different cities in linear time
void city_select(const size_t count_index, size_t *results)
{
	if (count_index >= COUNT_LOCATION)
		return;

	size_t index_array[COUNT_LOCATION];
	for (size_t i = 0; i < COUNT_LOCATION; i++)
		index_array[i] = i;

	size_t it_index_array = 0;
	for (; it_index_array < count_index; it_index_array++)
	{
		const size_t it_index_chosen = g(it_index_array, count_index - 1);
		const size_t answer = index_array[it_index_chosen];

		index_array[it_index_chosen] =
			index_array[it_index_array];
		index_array[it_index_array] = answer;

		results[it_index_array] = answer;
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
