/*
 * Wordle program
 * Author: Benjamin Hall
 */

#include "console_app/console_app.h"
#include "collections/string.h"

#include <stdio.h>

/**
 * Runs the Wordle program.
 * 
 * The user must pass in the name of the dictionary
 * file as a command-line argument to the program.
 */
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
        return -1;
    }

    FILE *usernames_file = fopen(USERNAMES_FILENAME, "a+");
    if (dict_file == NULL) {
        printf("Error: could not read user database\n");
        fclose(dict_file);
        return -1;
    }

    /* read the dictionary file */
    hashset_t dictionary = hashset_with_capacity(string_type_info(), 1024);
    string_t dict_file_contents = string_new();
    while (file_read_line_to_string(&dict_file_contents, dict_file) == 0) {
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

    /* read the usernames file */
    treeset_t usernames = treeset_new(string_type_info());
    string_t usernames_file_contents = string_new();
    while (file_read_line_to_string(&usernames_file_contents, usernames_file) == 0) {
        string_t line = string_clone(&usernames_file_contents);
        treeset_insert(&usernames, &line);
    }
    string_drop(&usernames_file_contents);
    fclose(usernames_file);

    /* run the main program */
    run_console_app(&dictionary, &usernames);

    /* cleanup */
    treeset_drop(&usernames);
    hashset_drop(&dictionary);

    return 0;
}
