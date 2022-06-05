/*
 * Author: Benjamin Hall
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>

typedef void (*drop_fn)(void *value);
typedef int (*compare_fn)(void const *a, void const *b);
typedef int (*hash_fn)(void const *value);

typedef struct _type_info_t {
    size_t type_sz;
    drop_fn drop;
    compare_fn compare;
    hash_fn hash;
} type_info_t;

static inline void const *move(void const *value, type_info_t *type_info)
{
    type_info->drop = NULL;
    return value;
}

#ifdef _MSC_VER
#define CAST_BUF(ptr) ((uint8_t *)ptr)
#else
#define CAST_BUF(ptr) (ptr)
#endif
