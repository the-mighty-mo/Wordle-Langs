/*
 * Author: Benjamin Hall
 */

#include "collections/hashset.h"
#include "util.h"

#include <string.h>

#define DEFAULT_NONZERO_CAP 16

hashset_t hashset_new(type_info_t type_info)
{
    return hashset_with_capacity(type_info, 0);
}

hashset_t hashset_with_capacity(type_info_t type_info, size_t cap)
{
    if (cap == 0) {
        hashset_t hashset = {
            0, 0, NULL, NULL, type_info
        };
        return hashset;
    } else {
        hashset_t hashset = {
            0,
            cap,
            calloc(cap, type_info.type_sz),
            calloc(cap, sizeof(*hashset.tombstones)),
            type_info
        };
        return hashset;
    }
}

hashset_t hashset_from(type_info_t type_info, void const *arr, size_t count)
{
    if (count == 0) {
        return hashset_new(type_info);
    }

    size_t buf_len = max(count, DEFAULT_NONZERO_CAP);
    void *buf = calloc(buf_len, type_info.type_sz);
    uint8_t *tombstones = calloc(buf_len, sizeof(*tombstones));

    hashset_t hashset = {
        0,
        buf_len,
        buf,
        tombstones,
        type_info
    };

    for (size_t i = 0; i < count; ++i) {
        hashset_insert(&hashset, CAST_BUF(arr) + i * type_info.type_sz);
    }
    return hashset;
}

hashset_t hashset_clone(hashset_t const *hashset)
{
    if (hashset == NULL) {
        type_info_t info = {0, NULL, NULL, NULL};
        return hashset_new(info);
    } else if (hashset->buf_sz == 0) {
        return hashset_new(hashset->type_info);
    }

    hashset_t set = {
        hashset->len,
        hashset->buf_sz,
        malloc(hashset->buf_sz * hashset->type_info.type_sz),
        malloc(hashset->buf_sz * sizeof(*hashset->tombstones)),
        hashset->type_info
    };
    memcpy(set.buf, hashset->buf, set.buf_sz * set.type_info.type_sz);
    memcpy(set.tombstones, hashset->tombstones, set.buf_sz * sizeof(*set.tombstones));
    return set;
}

void hashset_drop(hashset_t *hashset)
{
    if (hashset == NULL) {
        return;
    }

    if (hashset->type_info.drop) {
        for (size_t i = 0; i < hashset->buf_sz; ++i) {
            if (hashset->tombstones[i] == 1) {
                /* element exists, drop it */
                hashset->type_info.drop(CAST_BUF(hashset->buf) + i * hashset->type_info.type_sz);
            }
        }
    }
    free(hashset->buf);
    free(hashset->tombstones);
    memset(hashset, 0, sizeof(*hashset));
}

static void rehash(hashset_t *hashset, size_t old_buf_sz, void *old_buf, uint8_t *old_tombstones)
{
    for (size_t i = 0; i < old_buf_sz; ++i) {
        if (old_tombstones[i] != 1) {
            /* empty or dead element */
            continue;
        }

        size_t hash_value = hashset->type_info.hash(CAST_BUF(old_buf) + i * hashset->type_info.type_sz) % hashset->buf_sz;
        while (hashset->tombstones[hash_value] != 0) {
            ++hash_value;
            hash_value %= hashset->buf_sz;
        }
        memcpy(CAST_BUF(hashset->buf) + hash_value * hashset->type_info.type_sz, CAST_BUF(old_buf) + i * hashset->type_info.type_sz, hashset->type_info.type_sz);
        hashset->tombstones[hash_value] = 1;
    }
}

void hashset_reserve(hashset_t *hashset, size_t additional)
{
    if (hashset == NULL) {
        return;
    }

    if (hashset->len + additional > hashset->buf_sz) {
        /* cache old buffer and tombstones for rehash */
        size_t buf_sz = hashset->buf_sz;
        void *buf = hashset->buf;
        uint8_t *tombstones = hashset->tombstones;

        hashset->buf_sz = max(hashset->len + additional, hashset->buf_sz << 1);
        hashset->buf_sz = max(hashset->buf_sz, DEFAULT_NONZERO_CAP);
        hashset->buf = calloc(hashset->buf_sz, hashset->type_info.type_sz);
        hashset->tombstones = calloc(hashset->buf_sz, sizeof(*hashset->tombstones));

        rehash(hashset, buf_sz, buf, tombstones);
        /* free the cached buffer and tombstones */
        free(buf);
        free(tombstones);
    }
}

int hashset_is_empty(hashset_t const *hashset)
{
    return hashset->len == 0;
}

void hashset_insert(hashset_t *hashset, void const *elem)
{
    if (hashset == NULL || elem == NULL) {
        return;
    }

    hashset_reserve(hashset, 1);

    size_t hash_value = hashset->type_info.hash(elem) % hashset->buf_sz;
    while (hashset->tombstones[hash_value] != 0) {
        ++hash_value;
        hash_value %= hashset->buf_sz;
    }
    memcpy(CAST_BUF(hashset->buf) + hash_value * hashset->type_info.type_sz, elem, hashset->type_info.type_sz);
    hashset->tombstones[hash_value] = 1;

    ++hashset->len;
}

void hashset_clear(hashset_t *hashset)
{
    if (hashset_is_empty(hashset)) {
        return;
    }

    if (hashset->type_info.drop) {
        for (size_t i = 0; i < hashset->len; ++i) {
            hashset->type_info.drop(CAST_BUF(hashset->buf) + i * hashset->type_info.type_sz);
        }
    }
    memset(hashset->tombstones, 0, hashset->buf_sz * sizeof(*hashset->tombstones));
    hashset->len = 0;
}

int hashset_contains(hashset_t const *hashset, void const *elem)
{
    if (hashset == NULL || elem == NULL || hashset_is_empty(hashset)) {
        return 0;
    }

    size_t hash_value = hashset->type_info.hash(elem) % hashset->buf_sz;
    while (hashset->tombstones[hash_value]) {
        void const *hashset_elem = CAST_BUF(hashset->buf) + hash_value * hashset->type_info.type_sz;
        if (hashset->type_info.compare(hashset_elem, elem) == 0) {
            return 1;
        } else {
            ++hash_value;
        }
    }
    return 0;
}

void const *hashset_get_next(hashset_t const *hashset, void const *elem)
{
    if (hashset == NULL || hashset_is_empty(hashset)) {
        return NULL;
    }

    int i = 0;
    if (elem != NULL) {
        /* get the index of the input element */
        i = (CAST_BUF(elem) - hashset->buf) / hashset->type_info.type_sz + 1;
    }

    for (; i < hashset->buf_sz; ++i) {
        if (hashset->tombstones[i] == 1) {
            /* element exists */
            return CAST_BUF(hashset->buf) + i * hashset->type_info.type_sz;
        }
    }
    return NULL;
}

static type_info_t const type_info = {
    sizeof(hashset_t),
    hashset_drop,
    NULL,
    NULL
};

type_info_t hashset_type_info(void)
{
    return type_info;
}
