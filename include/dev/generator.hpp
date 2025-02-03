#ifndef _0GENERATOR0_
#define _0GENERATOR0_

#include <thread>
#include <random>

class random_generator
{
  std::mt19937 generator;

public:
  inline random_generator()
  {
    size_t seed_of_seed =
        time(0) + clock() * 1000 +
        10000 * std::hash<std::thread::id>{}(std::this_thread::get_id());

    std::mt19937 random_seed;
    random_seed.seed((unsigned int)seed_of_seed);
    random_seed.discard(31337);

    generator.seed(random_seed());
  }

  inline unsigned int operator()() { return generator(); }

  // [minimum, maximum]
  inline size_t operator()(size_t minimum, size_t maximum)
  {
    std::uniform_int_distribution<size_t> distribution(minimum, maximum);
    return distribution(generator);
  }

  // [minimum, maximum]
  inline unsigned short operator()(unsigned short minimum, unsigned short maximum)
  {
    std::uniform_int_distribution<unsigned short> distribution(minimum, maximum);
    return distribution(generator);
  }

  // [minimum, maximum)
  inline double operator()(double minimum, double maximum)
  {
    std::uniform_real_distribution<> distribution(minimum, maximum);
    return distribution(generator);
  }
};

#define SIZE 100

/* standard function
 * randomly select count_index cities in linear time without repetition
 */
void select(const size_t count_index, size_t *const results)
{
  random_generator g;
  if (nullptr == results)
    return;

  size_t index_array[SIZE];
  for (size_t i = 0; i < SIZE; i++)
    index_array[i] = i;

  size_t it_index_array = 0;
  for (; it_index_array < count_index; it_index_array++)
  {
    const size_t it_index_chosen =
        g(it_index_array, SIZE - 1);
    const size_t answer = index_array[it_index_chosen];

    index_array[it_index_chosen] =
        index_array[it_index_array];
    index_array[it_index_array] = answer;

    results[it_index_array] = answer;
  }
}

#endif
