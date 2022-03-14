#include "players/database.h"

#include <stdlib.h>
#include <string.h>

#define DELIM ": "

void database_entry_drop(database_entry_t *entry)
{
    if (entry == NULL) {
        return;
    }
    string_drop(&entry->name);
    if (entry->type_info.drop) {
        entry->type_info.drop(entry->value);
    }
    free(entry->value);
}

int database_entry_from_line(database_entry_t *entry, char *line, str_to_value str_to_v, type_info_t type_info)
{
    if (entry == NULL || line == NULL || str_to_v == NULL) {
        return -1;
    }

    char *value_str = strstr(line, DELIM);
    if (value_str == NULL) {
        return -1;
    }

    *value_str = '\0';
    value_str += 2;

    void *value;
    if (str_to_v(value_str, &value) != 0) {
        return -1;
    }

    entry->name = string_from(line);
    entry->value = value;
    entry->type_info = type_info;

    return 0;
}

static int identity(char *str, void **value)
{
    *value = str;
    return 0;
}

static type_info_t const str_info = {
    sizeof(char),
    NULL,
    NULL,
    NULL
};

int database_entry_from_collection(database_entry_t *entry, char *line, str_to_value str_to_v, type_info_t type_info, void *collection, insert_to_collection insert)
{
    database_entry_t parsed_row;

    if (database_entry_from_line(&parsed_row, line, identity, str_info) != 0) {
        return -1;
    }

    char *next_item = NULL;
    char *token;

#ifdef _MSC_VER
    token = strtok_s(parsed_row.value, ",", &next_item);
#else
    token = strtok_r(parsed_row.value, ",", &next_item);
#endif
    while (token) {
        void *value;
        if (str_to_v(token, &value) != 0) {
            return -1;
        }

        insert(collection, value);
        free(value);
#ifdef _MSC_VER
        token = strtok_s(NULL, ",", &next_item);
#else
        token = strtok_r(NULL, ",", &next_item);
#endif
    }

    entry->name = parsed_row.name;
    entry->value = collection;
    entry->type_info = type_info;

    return 0;
}
