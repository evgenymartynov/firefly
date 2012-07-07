#include <firefly/core/random.hpp>
#include <cstdlib>

////////////////////////////////////////////////////////////////////////

namespace ff {

    static unsigned int g_seed = 0;

// supplies a seed value to the rng

    void rng_seed(unsigned int seed)
    {
        g_seed = seed;
        srand(seed);
    }

// returns the current seed being used

    unsigned int rng_get_seed()
    {
        return g_seed;
    }

// basic function for random number generation

    unsigned int rng(unsigned int min, unsigned int max)
    {
        // return ( min + ((rand() % (max - min)) + 1) );
        return (min + int(((max-min) * rand()) / (RAND_MAX + 1.0)));
    }

// helper function for simple random numbers

    unsigned int rng(unsigned int max)
    {
        return rng(1, max);
    }

} // exiting namespace ff
