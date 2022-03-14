#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "type_info.h"

typedef struct _hashset_t {
    size_t len;
    size_t buf_sz;
    void *buf;
    uint8_t *tombstones;
    type_info_t type_info;
} hashset_t;

hashset_t hashset_new(type_info_t type_info);
hashset_t hashset_with_capacity(type_info_t type_info, size_t cap);
hashset_t hashset_from(type_info_t type_info, void const *arr, size_t count);
hashset_t hashset_clone(hashset_t const *hashset);
void hashset_drop(hashset_t *hashset);
void hashset_reserve(hashset_t *hashset, size_t additional);

int hashset_is_empty(hashset_t const *hashset);
void hashset_insert(hashset_t *hashset, void const *elem);
void hashset_clear(hashset_t *hashset);

int hashset_contains(hashset_t const *hashset, void const *elem);
void const *hashset_get_next(hashset_t const *hashset, void const *elem);

type_info_t hashset_type_info(void);
