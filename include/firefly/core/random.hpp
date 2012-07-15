#ifndef FIREFLY_RANDOM_HPP
#define FIREFLY_RANDOM_HPP

////////////////////////////////////////////////////////////////////////

namespace ff {

    extern inline void rng_seed(unsigned int seed = 0);
    extern inline unsigned int rng_get_seed();
    extern inline int rng(unsigned int min, unsigned int max);
    extern inline int rng(unsigned int max);

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////

#endif
