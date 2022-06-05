/*
 * Author: Benjamin Hall
 */

#pragma once

#include "type_info.h"

/**
 * Node color (RED or BLACK).
 */
typedef enum _node_color_t {
    RED,
    BLACK
} node_color_t;

/**
 * Struct to store information about a
 * node in a red-black tree.
 */
typedef struct _treenode_t {
    void *data;
    node_color_t color;
    struct _treenode_t *parent;
    struct _treenode_t *left;
    struct _treenode_t *right;
} treenode_t;

/**
 * A treeset, implemented as a red-black tree.
 * Elements in the set are sorted by the type's comparator.
 */
typedef struct _treeset_t {
    size_t len;
    treenode_t *root;
    type_info_t type_info;
} treeset_t;

/**
 * Creates a new treeset.
 * 
 * @param type_info
 *        Information about the type stored in the set
 */
treeset_t treeset_new(type_info_t type_info);
/**
 * Creates a new treeset from an array of elements.
 * 
 * @param type_info
 *        Information about the type stored in the set
 * @param arr
 *        The array of elements
 * @param count
 *        The number of elements
 */
treeset_t treeset_from(type_info_t type_info, void const *arr, size_t count);
/**
 * Clones a treeset.
 * 
 * @param treeset
 *        The treeset to clone
 */
treeset_t treeset_clone(treeset_t const *treeset);
/**
 * Drops a treeset, freeing its internal resources.
 * 
 * @param treeset
 *        The treeset to drop
 */
void treeset_drop(treeset_t *treeset);

/**
 * Checks if the treeset is empty.
 * 
 * @param treeset
 *        The treeset
 */
int treeset_is_empty(treeset_t const *treeset);
/**
 * Inserts an element into the treeset.
 * 
 * This function takes ownership over the inserted element.
 * 
 * @param treeset
 *        The treeset into which to insert
 * @param elem
 *        A pointer to the element to insert
 */
void treeset_insert(treeset_t *treeset, void const *elem);
/**
 * Clears a treeset, freeing all nodes in the process.
 * 
 * @param treeset
 *        The treeset to clear
 */
void treeset_clear(treeset_t *treeset);

/**
 * Checks whether the treeset contains an element.
 * 
 * @param treeset
 *        The treeset in which to find the element
 * @param elem
 *        A pointer to the element to find
 */
int treeset_contains(treeset_t const *treeset, void const *elem);
/**
 * Gets the next element in the treeset iterator.
 * 
 * @param treeset
 *        The treeset over which to iterate
 * @param elem
 *        A pointer to the last element iterated
 */
void const *treeset_get_next(treeset_t const *treeset, void const *elem);

/**
 * Returns type information about a treeset.
 */
type_info_t treeset_type_info(void);
