#ifndef RANDOM_HPP
#define RANDOM_HPP


#include <stdint.h>

#define NL_RAND_MAX ULLONG_MAX


namespace rd {


static inline uint64_t rotl(const uint64_t x, int k);

static uint64_t s[2];

void set_seed(uint64_t seed);

uint64_t next(void);

void jump(void);

void long_jump(void);


}

#endif