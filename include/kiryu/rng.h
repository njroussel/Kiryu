#pragma once

#include <pcg_random.hpp>

#include <kiryu/common.h>

class RNG {
    public:
        RNG(const uint32_t seed = 0xDEADBEEF);

        inline float nextFloat() {
            // Set sign and exponent to 0 and 127 respectively and use the
            // random bits for the mantissa. This creates a values in [1, 2)
            union {
                uint32_t val;
                float f;
            } u;
            u.val = (m_pcg() >> 9) | 0x3f800000ul;

            return  u.f - 1.0f;
        }

    private:
        //TODO: check if pcg32_fast has any advantages
        pcg32 m_pcg;
};
