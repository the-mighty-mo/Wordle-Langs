/*
 * Wordle program
 * Author: Benjamin Hall
 */

#pragma once

#include <set>
#include <string>
#include <unordered_set>

namespace console_app {

/**
 * Name of the usernames database file.
 */
static constexpr char *USERNAMES_FILENAME = "users.txt";

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
void RunConsoleApp(std::unordered_set<std::string> const &dictionary, std::set<std::string> &usernames);

}
