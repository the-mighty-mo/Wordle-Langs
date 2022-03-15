/*
 * Author: Benjamin Hall
 */

#pragma once

#include "type_info.h"

/**
 * A dynamically sized array of elements.
 */
typedef struct _vec_t {
    size_t len;
    size_t buf_sz;
    void *buf;
    type_info_t type_info;
} vec_t;

/**
 * Creates a new vector.
 * 
 * The vector will not allocate the buffer until
 * at least one element is inserted.
 * 
 * @param type_info
 *        Information about the type stored in the vector
 */
vec_t vec_new(type_info_t type_info);
/**
 * Creates a new vector with the given capacity.
 * 
 * @param type_info
 *        Information about the type stored in the vector
 * @param cap
 *        The default capacity, excluding null termination
 */
vec_t vec_with_capacity(type_info_t type_info, size_t cap);
/**
 * Creates a new vector from an array of elements.
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
vec_t vec_from(type_info_t type_info, void const *arr, size_t count);
/**
 * Clones a vector.
 * 
 * @param vec
 *        The vector to clone
 */
vec_t vec_clone(vec_t const *vec);
/**
 * Drops a vector, freeing its internal resources.
 * 
 * @param vec
 *        The vector to drop
 */
void vec_drop(vec_t *vec);
/**
 * Ensures the vector's capacity is at least
 * {@code additional} bytes longer than its length.
 * 
 * @param vec
 *        The vector
 * @param additional
 *        The amount to add to the current length to
 *        get the new minimum buffer size
 */
void vec_reserve(vec_t *vec, size_t additional);

/**
 * Checks if the vector is empty.
 * 
 * @param vec
 *        The vector
 */
int vec_is_empty(vec_t const *vec);
/**
 * Pushes an element to the end of the vector.
 * 
 * This function takes ownership over the inserted element.
 * 
 * @param vec
 *        The vector into which to push
 * @param elem
 *        A pointer to the element to insert
 */
void vec_push_back(vec_t *vec, void const *elem);
/**
 * Pushes an array of elements to the end of the vector.
 * 
 * This function takes ownership over the inserted elements.
 * 
 * @param vec
 *        The vector into which to push
 * @param arr
 *        The array of elements
 * @param count
 *        The number of elements
 */
void vec_push_all(vec_t *vec, void const *arr, size_t count);
/**
 * Clears a vector.
 * 
 * This function does not resize the buffer.
 * 
 * @param vec
 *        The vector to clear
 */
void vec_clear(vec_t *vec);

/**
 * Checks whether the vector contains an element.
 * 
 * @param vec
 *        The vector in which to find the element
 * @param elem
 *        A pointer to the element to find
 */
int vec_contains(vec_t const *vec, void const *elem);

/**
 * Returns type information about a vector.
 */
type_info_t vec_type_info(void);
