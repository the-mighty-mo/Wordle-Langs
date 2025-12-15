/*
 * Author: Benjamin Hall
 */

#pragma once

#include <stdlib.h>

static inline size_t min(size_t a, size_t b) {
    return a < b ? a : b;
}

static inline size_t max(size_t a, size_t b) {
    return a > b ? a : b;
}
