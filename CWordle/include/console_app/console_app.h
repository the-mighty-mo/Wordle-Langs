/*
 * Wordle program
 * Author: Benjamin Hall
 */

#pragma once

#include "collections/hashset.h"
#include "collections/treeset.h"

/**
 * Name of the usernames database file.
 */
#define USERNAMES_FILENAME "users.txt"

/**
 * Runs the main state machine of the Wordle console program.
 *
 * This function manages the various states of the program
 * after all necessary variables have been initialized,
 * such as the dictionary and the set of existing usernames.
 * 
 * @param dictionary
 *        A dictionary of valid Wordle words
 * @param usernames
 *        A set of existing usernames
 */
void run_console_app(hashset_t const *dictionary, treeset_t *usernames);
