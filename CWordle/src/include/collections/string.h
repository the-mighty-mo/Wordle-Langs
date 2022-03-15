/*
 * Author: Benjamin Hall
 */

#pragma once

#include <stdio.h>
#include <string.h>

#include "type_info.h"

/**
 * A dynamically sized string.
 */
typedef struct _string_t {
    size_t len;
    size_t buf_sz;
    char *buf;
} string_t;

/**
 * Creates a new string.
 * 
 * The string will not allocate the buffer until
 * at least one character is inserted.
 */
string_t string_new(void);
/**
 * Creates a new string with the given capacity.
 * 
 * @param cap
 *        The default capacity, excluding null termination
 */
string_t string_with_capacity(size_t cap);
/**
 * Creates a new string from a character array.
 * 
 * @param str
 *        The null-terminated character array
 */
string_t string_from(char const *str);
/**
 * Clones a string.
 * 
 * @param string
 *        The string to clone
 */
string_t string_clone(string_t const *string);
/**
 * Drops a string, freeing its internal resources.
 * 
 * @param string
 *        The string to drop
 */
void string_drop(string_t *string);
/**
 * Ensures the string's capacity is at least
 * {@code additional} bytes longer than its length.
 * 
 * @param string
 *        The string
 * @param additional
 *        The amount to add to the current length to
 *        get the new minimum buffer size
 */
void string_reserve(string_t *string, size_t additional);

/**
 * Checks if the string is empty.
 * 
 * @param string
 *        The string
 */
int string_is_empty(string_t const *string);
/**
 * Pushes a character array to the end of the string.
 * 
 * @param string
 *        The string into which to concatenate
 * @param str
 *        The null-terminated character array to concatenate
 */
void string_push_str(string_t *string, char const *str);
/**
 * Clears a string.
 * 
 * This function does not resize the buffer.
 * 
 * @param string
 *        The string to clear
 */
void string_clear(string_t *string);

/**
 * Converts all characters of a string to lowercase.
 * 
 * @param string
 *        The string to make lowercase
 */
void string_to_lowercase(string_t *string);
/**
 * Converts all characters of a string to uppercase.
 * 
 * @param string
 *        The string to make uppercase
 */
void string_to_uppercase(string_t *string);
/**
 * Trims whitespace from both ends of a string.
 * 
 * @param string
 *        The string to trim
 */
void string_trim(string_t *string);
/**
 * Trims whitespace at the start of a string.
 * 
 * @param string
 *        The string to trim
 */
void string_trim_leading(string_t *string);
/**
 * Trims whitespace at the end of a string.
 * 
 * @param string
 *        The string to trim
 */
void string_trim_trailing(string_t *string);
/**
 * Trims trailing newlines at the end of a string.
 * 
 * @param string
 *        The string to trim
 */
void string_trim_newline(string_t *string);

/**
 * Reads a file to a string.
 * 
 * @param string
 *        The string into which to read
 * @param file
 *        The file to read
 */
int file_read_to_string(string_t *string, FILE *file);
/**
 * Reads a line of a file to a string.
 * 
 * @param string
 *        The string into which to read
 * @param file
 *        The file to read
 */
int file_read_line_to_string(string_t *file_contents, FILE *file);

/**
 * Returns type information about a string.
 */
type_info_t string_type_info(void);
