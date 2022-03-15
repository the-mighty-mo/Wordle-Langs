/*
 * Author: Benjamin Hall
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "type_info.h"

/**
 * A hashset, implemented using open addressing.
 * Elements in the set are hashed by the type's hash function.
 */
typedef struct _hashset_t {
    size_t len;
    size_t buf_sz;
    void *buf;
    uint8_t *tombstones;
    type_info_t type_info;
} hashset_t;

/**
 * Creates a new hashset.
 * 
 * The hashset will not allocate the buffer until
 * at least one element is inserted.
 * 
 * @param type_info
 *        Information about the type stored in the set
 */
hashset_t hashset_new(type_info_t type_info);
/**
 * Creates a new hashset with the given capacity.
 * 
 * @param type_info
 *        Information about the type stored in the set
 * @param cap
 *        The default capacity
 */
hashset_t hashset_with_capacity(type_info_t type_info, size_t cap);
/**
 * Creates a new hashset from an array of elements.
 * 
 * This function takes ownership over the inserted elements.
 * 
 * @param type_info
 *        Information about the type stored in the set
 * @param arr
 *        The array of elements
 * @param count
 *        The number of elements
 */
hashset_t hashset_from(type_info_t type_info, void const *arr, size_t count);
/**
 * Clones a hashset.
 * 
 * @param hashset
 *        The hashset to clone
 */
hashset_t hashset_clone(hashset_t const *hashset);
/**
 * Drops a hashset, freeing its internal resources.
 * 
 * @param hashset
 *        The hashset to drop
 */
void hashset_drop(hashset_t *hashset);
/**
 * Ensures the hashset's capacity is at least
 * {@code additional} bytes longer than its length.
 * 
 * @param hashset
 *        The hashset
 * @param additional
 *        The amount to add to the current length to
 *        get the new minimum buffer size
 */
void hashset_reserve(hashset_t *hashset, size_t additional);

/**
 * Checks if the hashset is empty.
 * 
 * @param hashset
 *        The hashset
 */
int hashset_is_empty(hashset_t const *hashset);
/**
 * Inserts an element into the hashset.
 * 
 * This function takes ownership over the inserted element.
 * 
 * @param hashset
 *        The hashset into which to insert
 * @param elem
 *        A pointer to the element to insert
 */
void hashset_insert(hashset_t *hashset, void const *elem);
/**
 * Clears a hashset.
 * 
 * This function does not resize the buffer.
 * 
 * @param hashset
 *        The hashset to clear
 */
void hashset_clear(hashset_t *hashset);

/**
 * Checks whether the hashset contains an element.
 * 
 * @param hashset
 *        The hashset in which to find the element
 * @param elem
 *        A pointer to the element to find
 */
int hashset_contains(hashset_t const *hashset, void const *elem);
/**
 * Gets the next element in the hashset iterator.
 * 
 * @param hashset
 *        The hashset over which to iterate
 * @param elem
 *        A pointer to the last element iterated
 */
void const *hashset_get_next(hashset_t const *hashset, void const *elem);

/**
 * Returns type information about a hashset.
 */
type_info_t hashset_type_info(void);
