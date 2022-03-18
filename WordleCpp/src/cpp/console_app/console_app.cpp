/*
 * Wordle program
 * Author: Benjamin Hall
 */

#include "console_app/console_app.h"
#include "console_app/main_menu.h"

#include <filesystem>
#include <fstream>

#include <stdlib.h>
#include <time.h>

namespace console_app {

using main_menu::ProgramState;

/**
 * Saves a set of usernames to the usernames database file.
 *
 * Any errors, such as failing to open the file or not having
 * read access, will be propagated up to the caller.
 * 
 * @param usernames
 *        A set of usernames to save
 * @param filename
 *        The file to which the usernames are saved
 */
static int SaveUsernames(std::set<std::string> &usernames, std::string const &filename)
{
    std::ofstream file{filename};
    if (!file.is_open()) {
        return -1;
    }

    for (auto username : usernames) {
        file << username << std::endl;
    }
    
    file.close();
    return 0;
}

void RunConsoleApp(std::unordered_set<std::string> const &dictionary, std::set<std::string> &usernames)
{
    srand(time(NULL));

    ProgramState state = ProgramState::LogIn;
    uint8_t runProgram = 1;

    std::optional<players::PlayerInfo> currentPlayer;

    while (runProgram) {
        switch (state) {
        case ProgramState::LogIn:
        {
            currentPlayer = main_menu::RequestUserLogin(usernames);
            if (!currentPlayer.has_value()) {
                /* user requested to exit, or there was an error */
                state = ProgramState::Exit;
            } else {
                if (SaveUsernames(usernames, USERNAMES_FILENAME) != 0) {
                    printf("Error: could not write to the user database\n");
                    state = ProgramState::Exit;
                } else {
                    /* user has logged in, continue to the main menu */
                    state = ProgramState::MainMenu;
                }
            }
        }
        break;
        case ProgramState::MainMenu:
        {
            state = main_menu::RunMenu(*currentPlayer, dictionary);
        }
        break;
        case ProgramState::DeleteUser:
        {
            /* remove the current player from the databse */
            usernames.erase(currentPlayer->GetUsername());
            std::filesystem::remove(currentPlayer->GetUsername() + ".txt");

            /* save the username database */
            if (SaveUsernames(usernames, USERNAMES_FILENAME) != 0) {
                printf("Error: could not write to the user database\n");
                state = ProgramState::Exit;
            } else {
                /* user has logged out, go to the login screen */
                state = ProgramState::LogIn;
            }
        }
        break;
        case ProgramState::Exit:
        {
            runProgram = 0;
        }
        break;
        }
    }
}

}
