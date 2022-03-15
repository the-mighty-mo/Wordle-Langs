#pragma once

#include "type_info.h"

typedef struct _vec_t {
    size_t len;
    size_t buf_sz;
    void *buf;
    type_info_t type_info;
} vec_t;

vec_t vec_new(type_info_t type_info);
vec_t vec_with_capacity(type_info_t type_info, size_t cap);
vec_t vec_from(type_info_t type_info, void const *arr, size_t count);
vec_t vec_clone(vec_t const *vec);
void vec_drop(vec_t *vec);
void vec_reserve(vec_t *vec, size_t additional);

int vec_is_empty(vec_t const *vec);
void vec_push_back(vec_t *vec, void const *elem);
void vec_push_all(vec_t *vec, void const *arr, size_t count);
void vec_clear(vec_t *vec);

int vec_contains(vec_t const *vec, void const *elem);

type_info_t vec_type_info(void);
