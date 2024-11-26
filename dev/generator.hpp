#ifndef _0GENERATOR0_
#define _0GENERATOR0_

#include "pservice_base"
#include <thread>
#include <random>

STD_PSERVICE_BEGIN

class random_generator
{
    std::mt19937 generator;

public:
    inline random_generator() {
        size_t seed_of_seed = time(0) + clock() * 1000 + 10000 *
            std::hash<std::thread::id>{}(std::this_thread::get_id());

        std::mt19937 random_seed;
        random_seed.seed((unsigned int)seed_of_seed);
        random_seed.discard(31337);

        generator.seed(random_seed());
    }

    inline unsigned int operator () () { return generator(); }
    
    // [minimum, maximum]
    inline size_t operator () (size_t minimum, size_t maximum) {
        std::uniform_int_distribution<size_t> distribution(minimum, maximum);
        return distribution(generator);
    }
  
    // [minimum, maximum)
    inline double operator () (double minimum, double maximum) {
        std::uniform_real_distribution<> distribution(minimum, maximum);
        return distribution(generator);
    }
};

STD_PSERVICE_END
#endif
