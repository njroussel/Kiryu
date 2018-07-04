#pragma once

#include <cstdint>

#ifdef KIRYU_DOUBLE_PRECISION
#define Float double
#define TINYOBJLOADER_USE_DOUBLE true
#else
#define Float float 
#endif

#define KIRYU_PI 3.14159265359f

#define KIRYU_EPSILON 1e-6f
