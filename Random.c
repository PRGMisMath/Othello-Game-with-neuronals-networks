#include "Random.h"

static inline uint64_t rd_rotl(const uint64_t x, int k)
{
	return (x << k) | (x >> (64 - k));
}

void rd_set_seed(uint64_t seed)
{
	s[0] = seed;
	seed += 0x9e3779b97f4a7c15;
	seed = (seed ^ (seed >> 30)) * 0xbf58476d1ce4e5b9;
	seed = (seed ^ (seed >> 27)) * 0x94d049bb133111eb;
	s[1] = seed ^ (seed >> 31);
}

uint64_t rd_next(void)
{
	const uint64_t s0 = s[0];
	uint64_t s1 = s[1];
	const uint64_t result = s0 + s1;

	s1 ^= s0;
	s[0] = rd_rotl(s0, 24) ^ s1 ^ (s1 << 16); // a, b
	s[1] = rd_rotl(s1, 37); // c

	return result;
}

void rd_jump(void) 
{
	static const uint64_t JUMP[] = { 0xdf900294d8f554a5, 0x170865df4b3201fc };

	uint64_t s0 = 0;
	uint64_t s1 = 0;
	for (int i = 0; i < sizeof JUMP / sizeof * JUMP; i++)
		for (int b = 0; b < 64; b++) {
			if (JUMP[i] & UINT64_C(1) << b) {
				s0 ^= s[0];
				s1 ^= s[1];
			}
			rd_next();
		}

	s[0] = s0;
	s[1] = s1;
}

void rd_long_jump(void) 
{
	static const uint64_t LONG_JUMP[] = { 0xd2a98b26625eee7b, 0xdddf9b1090aa7ac1 };

	uint64_t s0 = 0;
	uint64_t s1 = 0;
	for (int i = 0; i < sizeof LONG_JUMP / sizeof * LONG_JUMP; i++)
		for (int b = 0; b < 64; b++) {
			if (LONG_JUMP[i] & UINT64_C(1) << b) {
				s0 ^= s[0];
				s1 ^= s[1];
			}
			rd_next();
		}

	s[0] = s0;
	s[1] = s1;
}
