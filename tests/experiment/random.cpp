#include "../../include/dev/generator.hpp"

random_generator g;

double measure(const size_t number, const size_t count_trial)
{
    double frequency[number]{};
    for (size_t i = 0; i < count_trial; i++)
        frequency[g(0, number)]++;



    // reinitialise array
    for (size_t i = 0; i < number; i++)
        frequency[i] = 0;

    for (size_t i = 0; i < count_trial; i++)
        frequency[rand() % number]++;
}

void measure_all()
{
    for (size_t number = 10; number < 1000; number *= 10)
    {
        measure(number);
    }
}

int main()
{
    for (size_t i = 0; i < 30; i++)
    {
    }
}
