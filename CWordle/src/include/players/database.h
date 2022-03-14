#pragma once

#include "collections/string.h"
#include "type_info.h"

typedef struct _database_entry_t {
    string_t name;
    void *value;
    type_info_t type_info;
} database_entry_t;

typedef int (*str_to_value)(char *str, void **value);
typedef int (*insert_to_collection)(void *collection, void const *value);

void database_entry_drop(database_entry_t *entry);
int database_entry_from_line(database_entry_t *entry, char *line, str_to_value str_to_v, type_info_t type_info);
int database_entry_from_collection(database_entry_t *entry, char *line, str_to_value str_to_v, type_info_t type_info, void *collection, insert_to_collection insert);
