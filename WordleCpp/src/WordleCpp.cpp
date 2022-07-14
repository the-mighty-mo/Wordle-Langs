/*
 * Wordle program
 * Author: Benjamin Hall
 */

#include "console_app/console_app.hpp"

#include <algorithm>
#include <fstream>
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

    char const *dictFileName = argv[1];

    /* make sure none of the inputs are empty */
    if (*dictFileName == '\0') {
        printf("Error: no dictionary file specified\n");
        return -1;
    }

    std::ifstream dictFile{dictFileName};
    if (!dictFile.is_open()) {
        printf("Error: could not read dictionary file\n");
        return -1;
    }

    std::fstream usernamesFile{console_app::USERNAMES_FILENAME, std::fstream::in | std::fstream::out | std::fstream::app};
    if (!usernamesFile.is_open()) {
        printf("Error: could not read user database\n");
        dictFile.close();
        return -1;
    }

    /* read the dictionary file */
    std::unordered_set<std::string> dictionary;
    {
        std::string dictFileContents;
        while (std::getline(dictFile, dictFileContents)) {
            if (dictFileContents.length() == 5) {
                /* convert to uppercase */
                std::transform(dictFileContents.begin(), dictFileContents.end(), dictFileContents.begin(), [](auto c) {
                        return std::toupper(c);
                    });
                /* add to dictionary */
                dictionary.insert(dictFileContents);
            }
        }
    }
    dictFile.close();

    /* read the usernames file */
    std::set<std::string> usernames;
    {
        std::string usernamesFileContents;
        while (std::getline(usernamesFile, usernamesFileContents)) {
            usernames.insert(usernamesFileContents);
        }
    }
    usernamesFile.close();

    /* run the main program */
    console_app::Run(dictionary, usernames);

    return 0;
}
