#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "type_info.h"

typedef struct _treenode_t {
    void *data;
    struct _treenode_t *parent;
    struct _treenode_t *left;
    struct _treenode_t *right;
} treenode_t;

typedef struct _treeset_t {
    size_t len;
    size_t buf_sz;
    treenode_t *root;
    type_info_t type_info;
} treeset_t;

treeset_t treeset_new(type_info_t type_info);
treeset_t treeset_with_capacity(type_info_t type_info, size_t cap);
treeset_t treeset_from(type_info_t type_info, void const *arr, size_t count);
treeset_t treeset_clone(treeset_t const *treeset);
void treeset_drop(treeset_t *treeset);
void treeset_reserve(treeset_t *treeset, size_t additional);

int treeset_is_empty(treeset_t const *treeset);
void treeset_insert(treeset_t *treeset, void const *elem);
void treeset_clear(treeset_t *treeset);

int treeset_contains(treeset_t const *treeset, void const *elem);

type_info_t treeset_type_info(void);
