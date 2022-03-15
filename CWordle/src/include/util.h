/*
 * Author: Benjamin Hall
 */

#pragma once

#include <stdlib.h>

#ifdef _MSC_VER
#undef min
#undef max
#endif

static inline size_t min(size_t a, size_t b) {
    return a < b ? a : b;
}

static inline size_t max(size_t a, size_t b) {
    return a > b ? a : b;
}
