#include <stdio.h>
#include "generator.hpp"

static const int count_cities = 42;
static const int count_routes_max = 41 * 42 / 2;

int main()
{
    // 0. generate at random the number of routes
    
    std::mt19937 generator;

    size_t seed_of_seed = time(0) + clock() * 1000 + 10000; //std::hash<std::thread::id>{}(std::this_thread::get_id());
    std::thread::id iddd = std::this_thread::get_id();
    std::thread::id iddd1(10);
    std::hash<std::thread::id>{}(iddd);
    //std::hash<std::thread::id>{}(std::this_thread::get_id());
        //std::mt19937 random_seed;
        //random_seed.seed((unsigned int)seed_of_seed);
        //random_seed.discard(31337);

        //generator.seed(random_seed());//int count_routes = g((size_t)10, (size_t)count_routes_max);
    //printf("%d", count_routes);
    
    // 0. generate at random the number of routes
    // 1. generate at random index_city
    // 2. for each generate times
    // 3. paste to xml 
    

    

    return EXIT_SUCCESS;
}


// randomly select count_index different cities in linear time
void city_select(size_t count_index)
{
    size_t index_array[count_cities];
    for(size_t i = 0; i < count_cities; i++)
        index_array[i] = i;

    size_t it_index_array = 0;
    for(; it_index_array < count_cities; it_index_array++)
    {
        size_t it_index_chosen = 0;
        size_t answer = index_array[it_index_chosen];
        
        index_array[it_index_chosen] = 
            index_array[it_index_array];
        index_array[it_index_array] = answer;

        if(it_index_array == count_index)
            break;
    }
}
