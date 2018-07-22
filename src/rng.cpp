#include <kiryu/rng.h>

RNG::RNG(const uint32_t seed) : m_pcg(seed) { }
