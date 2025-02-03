/* comments:
 * write_txt.cpp - a route random generator application
 * author - Braha Petru Bogdan - <petrubraha@gmail.com> (c)
 * compilation command: g++ -std=c++11 include/dev/write_txt.cpp -o include/dev/write_txt
 */

#include <stdio.h>
#include <string.h>
#include "write_file.hpp"

void select_restriction(const usht, usht *const);
void location_generation(std::vector<ez_route> &);
void schedule_generation(std::vector<ez_route> &);
void document_text(const char *const, std::vector<ez_route> &);
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

	document_text(path, schedule);
	return EXIT_SUCCESS;
}

void document_text(
		const char *const path,
		std::vector<ez_route> &schedule)
{
	FILE *txt = fopen(path, "w");
	if (nullptr == txt)
	{
		printf("error: write_txt failed - fopen().\n");
		exit(EXIT_FAILURE);
	}

	for (size_t i = 0; i < schedule.size(); i++)
	{
		std::string buffer;
		ez_route &route = schedule.at(i);
		buffer += std::to_string(route.departure_location);
		buffer += ' ';
		buffer += std::to_string(route.arrival_location);
		buffer += ' ';
		buffer += std::to_string(route.departure_time);
		buffer += ' ';
		buffer += std::to_string(route.arrival_time);
		buffer += '\n';

		if (0 == fwrite(buffer.c_str(), sizeof(char), buffer.size(), txt))
		{
			printf("error: write_txt failed - fwrite().\n");
			exit(EXIT_FAILURE);
		}
	}

	if (fclose(txt))
	{
		printf("error: write_txt failed - fclose().\n");
		exit(EXIT_FAILURE);
	}
}
