#pragma once

#include "collections/string.h"
#include "type_info.h"

/**
 * Stores information about an entry in a database.
 * 
 * Each database entry contains two portions: the name
 * of the field, and the value of the data.
 */
typedef struct _database_entry_t {
    string_t name;
    void *value;
    type_info_t type_info;
} database_entry_t;

/**
 * Converts from a character array to a heap-allocated, owned value.
 */
typedef int (*str_to_value)(char *str, void **value);
/**
 * Inserts an element into a collection.
 */
typedef int (*insert_to_collection)(void *collection, void const *value);

/**
 * Drops a database entry, freeing its internal resources.
 * 
 * @param entry
 *        The entry to drop
 */
void database_entry_drop(database_entry_t *entry);
/**
 * Creates a simple database entry from a line of text.
 *
 * The text will be split between field name and data
 * on the ": " delimiter. If the delimiter is not found,
 * then this function returns an error.
 * 
 * @param entry
 *        A pointer into which to load the entry
 * @param line
 *        The line to parse
 * @param string_to_v
 *        A function to convert a character array to a
 *        heap-allocated, owned value
 * @param type_info
 *        Information about the type stored in the entry
 */
int database_entry_from_line(database_entry_t *entry, char *line, str_to_value str_to_v, type_info_t type_info);
/**
 * Creates a database entry from a line of text where
 * the data field is a collection of elements.
 *
 * The text will be split between field name and data
 * on the ": " delimiter. If the delimiter is not found,
 * then this function returns an error. From there,
 * elements will be separated by the "," delemiter and
 * added to a collection.
 * 
 * This function takes ownership over the collection.
 * 
 * @param entry
 *        A pointer into which to load the entry
 * @param line
 *        The line to parse
 * @param string_to_v
 *        A function to convert a character array to a
 *        heap-allocated, owned value for the collection
 * @param collection
 *        A pointer to a heap-allocated collection
 * @param type_info
 *        Information about the collection type
 * @param insert
 *        A function to insert an element into the collection
 */
int database_entry_from_collection(database_entry_t *entry, char *line, str_to_value str_to_v, void *collection, type_info_t type_info, insert_to_collection insert);
