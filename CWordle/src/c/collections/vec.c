#include "collections/vec.h"
#include "util.h"

#include <ctype.h>
#include <string.h>

#define DEFAULT_CAP 16

vec_t vec_new(type_info_t type_info)
{
    return vec_with_capacity(type_info, DEFAULT_CAP);
}

vec_t vec_with_capacity(type_info_t type_info, size_t cap)
{
    vec_t vec = {
        0,
        cap,
        calloc(cap, type_info.type_sz),
        type_info
    };
    return vec;
}

vec_t vec_from(type_info_t type_info, void const *arr, size_t count)
{
    size_t buf_len = max(count, DEFAULT_CAP);
    void *buf = calloc(buf_len, type_info.type_sz);
    memcpy(buf, arr, count * type_info.type_sz);

    vec_t vec = {
        count,
        buf_len,
        buf,
        type_info
    };
    return vec;
}

vec_t vec_clone(vec_t const *vec)
{
    if (vec == NULL) {
        type_info_t type_info = {0, NULL, NULL, NULL};
        return vec_with_capacity(type_info, 1);
    }

    vec_t vector = {
        vec->len,
        vec->buf_sz,
        malloc(vec->buf_sz * vec->type_info.type_sz),
        vec->type_info.type_sz
    };
    memcpy(vector.buf, vec->buf, vector.buf_sz * vector.type_info.type_sz);
    return vector;
}

void vec_drop(vec_t *vec)
{
    if (vec == NULL) {
        return;
    }

    if (vec->type_info.drop) {
        for (int i = 0; i < vec->len; ++i) {
            vec->type_info.drop(vec->buf + i * vec->type_info.type_sz);
        }
    }
    free(vec->buf);
    memset(vec, 0, sizeof(*vec));
}

void vec_reserve(vec_t *vec, size_t additional)
{
    if (vec == NULL) {
        return;
    }

    if (vec->len + additional > vec->buf_sz) {
        vec->buf_sz = max(vec->len + additional, vec->buf_sz << 1);
        vec->buf = realloc(vec->buf, vec->buf_sz * vec->type_info.type_sz);
    }
}

int vec_is_empty(vec_t const *vec)
{
    return vec->len == 0;
}

void vec_push_back(vec_t *vec, void const *elem)
{
    if (vec == NULL || elem == NULL) {
        return;
    }

    vec_reserve(vec, 1);

    memcpy(vec->buf + vec->len * vec->type_info.type_sz, elem, vec->type_info.type_sz);
    ++vec->len;
}

void vec_push_all(vec_t *vec, void const *arr, size_t count)
{
    if (vec == NULL || arr == NULL) {
        return;
    }

    vec_reserve(vec, count);

    memcpy(vec->buf + vec->len * vec->type_info.type_sz, arr, count * vec->type_info.type_sz);
    vec->len += count;
}

void vec_clear(vec_t *vec)
{
    if (vec->type_info.drop) {
        for (int i = 0; i < vec->len; ++i) {
            vec->type_info.drop(vec->buf + i * vec->type_info.type_sz);
        }
    }
    vec->len = 0;
}

int vec_contains(vec_t const *vec, void const *elem)
{
    if (vec == NULL || elem == NULL) {
        return 0;
    }
    
    for (int i = 0; i < vec->len; ++i) {
        void const *vec_elem = vec->buf + i * vec->type_info.type_sz;
        if (vec->type_info.compare(vec_elem, elem) == 0) {
            return 1;
        }
    }
    return 0;
}

static type_info_t const type_info = {
    sizeof(vec_t),
    vec_drop,
    NULL,
    NULL
};

type_info_t vec_type_info(void)
{
    return type_info;
}