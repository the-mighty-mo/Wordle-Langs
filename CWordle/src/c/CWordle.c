#include "console_app/console_app.h"

#include "collections/hashset.h"
#include "collections/string.h"
#include "collections/treeset.h"

#include <stdio.h>
#include <stdlib.h>

static int read_file(string_t *buffer, FILE *file);

int main(int argc, char **argv)
{
    if (argc != 2) {
        /* invalid number of arguments, print a help message */
        printf("Wordle\n");
        printf("Author: Benjamin Hall\n");
        printf("Usage: ./CWordle [dictionary file name]\n");
        return 0;
    }

    char *dict_file_name = argv[1];

    /* make sure none of the inputs are empty */
    if (*dict_file_name == '\0') {
        printf("Error: no dictionary file specified\n");
        return -1;
    }

    FILE *dict_file = fopen(dict_file_name, "r");
    if (dict_file == NULL) {
        printf("Error: could not read dictionary file\n");
        return;
    }

    FILE *usernames_file = fopen(USERNAMES_FILENAME, "a+");
    if (dict_file == NULL) {
        printf("Error: could not read user database\n");
        fclose(dict_file);
        return;
    }

    hashset_t dictionary = hashset_with_capacity(string_type_info(), 1024);
    string_t dict_file_contents = string_with_capacity(1024);
    while (read_file(&dict_file_contents, dict_file) == 0) {
        string_t line = string_clone(&dict_file_contents);
        if (line.len == 5) {
            string_to_uppercase(&line);
            hashset_insert(&dictionary, &line);
        } else {
            string_drop(&line);
        }
    }
    string_drop(&dict_file_contents);
    fclose(dict_file);

    treeset_t usernames = treeset_new(string_type_info());
    string_t usernames_file_contents = string_new();
    while (read_file(&usernames_file_contents, usernames_file) == 0) {
        string_t line = string_clone(&usernames_file_contents);
        treeset_insert(&usernames, &line);
    }
    string_drop(&usernames_file_contents);
    fclose(usernames_file);

    run_console_app(&dictionary, &usernames);

    treeset_drop(&usernames);
    hashset_drop(&dictionary);

    return 0;
}

static int read_file(string_t *buffer, FILE *file)
{
    string_clear(buffer);
    return file_read_line_to_string(buffer, file);
}
