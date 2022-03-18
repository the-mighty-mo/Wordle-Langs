/*
 * Wordle program
 * Author: Benjamin Hall
 */

#include "console_app/main_menu.h"
#include "console_app/game.h"

#include <algorithm>
#include <iostream>
#include <fstream>

namespace console_app {
namespace main_menu {

using wordle::WordleAnswer;

/**
 * Possible user selections in the main menu.
 */
enum class UserSelection {
    /**
     * Play a game of Wordle
     */
    PlayGame = 1,
    /**
     * View the current player's statistics
     */
    ViewStats,
    /**
     * Log off
     */
    LogOff,
    /**
     * Delete the current user
     */
    DeleteUser
};

/**
 * Requests a user to enter their username.
 *
 * The user may choose to quit the program (or forcibly
 * quit using Ctrl-C), in which case this function returns
 * a nullopt.
 *
 * If the user does not yet exist in the given database,
 * they will be added to it.
 * 
 * @param usernames
 *        A set of existing usernames
 */
static std::optional<std::string> RequestUsername(std::set<std::string> &usernames)
{
    if (!usernames.empty()) {
        printf("List of existing users:\n");
        for (auto username : usernames) {
            printf("%s\n", username.c_str());
        }
        printf("\n");
    }

    printf("Note: usernames are case-insensitive\n");
    printf("Type \":q\" to exit\n");
    printf("Username: ");

    std::string username;
    if (!std::getline(std::cin, username)) {
        /* user likely quit the program with Ctrl-C */
        return std::nullopt;
    }
    /* trim string */
    username.erase(username.begin(), std::find_if(username.begin(), username.end(), [](auto c) {
            return !std::isspace(c);
        }));
    username.erase(std::find_if(username.rbegin(), username.rend(), [](auto c) {
            return !std::isspace(c);
        }).base(), username.end());
    /* convert to lowercase */
    std::transform(username.begin(), username.end(), username.begin(), [](auto c) {
            return std::tolower(c);
        });

    if (username.compare(":q") == 0) {
        /* user wants to exit */
        return std::nullopt;
    }

    if (usernames.find(username) == usernames.end()) {
        /* new user, add to database */
        usernames.insert(username);
    }

    return username;
}

std::optional<PlayerInfo> RequestUserLogin(std::set<std::string> &usernames)
{
    auto username = RequestUsername(usernames);
    if (!username.has_value()) {
        /* user requested to exit the game */
        return std::nullopt;
    }

    std::string filename = *username + ".txt";
    std::optional<PlayerInfo> playerInfo;
    try {
        playerInfo = PlayerInfo::CreateFromFile(filename);
    } catch (std::string e) {
        printf("%s\n", e.c_str());
        return std::nullopt;
    }

    printf("Hello, %s\n", username->c_str());

    /* this might be a new user, create a fresh instance of player_info_t if so */
    return playerInfo.value_or(PlayerInfo{*username});
}

/**
 * Requests a user to input their selection.
 *
 * This function gives the player four options:
 * - Play a game of Wordle
 * - View their statistics
 * - Log out
 * - Delete their account
 *
 * The user can terminate the program early using Ctrl-C,
 * in which case this function returns a nullopt.
 */
static std::optional<UserSelection> RequestUserSelection()
{
    std::optional<UserSelection> userSelection;

    printf("\n");
    printf("[1] Play a game of Wordle\n");
    printf("[2] View player statistics\n");
    printf("[3] Log off\n");
    printf("[4] Delete user\n");

    uint8_t read = 1;
    while (read) {
        printf("Selection: ");

        std::string selectionStr;
        if (!std::getline(std::cin, selectionStr)) {
            /* user likely quit the program with Ctrl-C */
            return std::nullopt;
        }

        try {
            uint8_t selection = std::stoi(selectionStr);
            if (selection >= 1 && selection <= 4) {
                /* valid selection, stop the read loop */
                userSelection = std::optional{(UserSelection)selection};
                read = 0;
            } else {
                /* selection out of range */
                printf("Error: invalid selection\n");
            }
        } catch (std::invalid_argument e) {
            printf("Error: selection must be an integer\n");
        }
    }
    printf("\n");

    return userSelection;
}

ProgramState RunMenu(PlayerInfo &currentPlayer, std::unordered_set<std::string> const &dictionary)
{
    ProgramState nextState = ProgramState::MainMenu;

    auto userSelection = RequestUserSelection();
    if (!userSelection.has_value()) {
        /* user likely quit the program with Ctrl-C */
        return ProgramState::Exit;
    }

    switch (*userSelection) {
    case UserSelection::PlayGame:
    {
        /* run a game of Wordle */
        WordleAnswer answer{currentPlayer.GetRandomWord(dictionary)};
        if (game::RunGame(answer, currentPlayer, dictionary) != 0) {
            nextState = ProgramState::Exit;
            break;
        }
        /* print the player's statistics after the game ends */
        printf("%s\n", currentPlayer.GetStats().c_str());
        /* save the user's new statistics to their database */
        if (currentPlayer.WriteToFile(currentPlayer.GetUsername() + ".txt") != 0) {
            /* report that we could not write to the database, but do not exit */
            printf("Error: could not write to user database file, progress has not been saved\n");
        }
    }
    break;
    case UserSelection::ViewStats:
    {
        printf("%s\n", currentPlayer.GetStats().c_str());
    }
    break;
    case UserSelection::LogOff:
        /* user is logged off, go back to login screen */
        nextState = ProgramState::LogIn;
        break;
    case UserSelection::DeleteUser:
    {
        printf("Are you sure you would like to delete user: %s [y/N] ", currentPlayer.GetUsername().c_str());

        std::string userConfirmation;
        std::getline(std::cin, userConfirmation);
        /* trim string */
        userConfirmation.erase(userConfirmation.begin(), std::find_if(userConfirmation.begin(), userConfirmation.end(), [](auto c) {
                return !std::isspace(c);
            }));
        userConfirmation.erase(std::find_if(userConfirmation.rbegin(), userConfirmation.rend(), [](auto c) {
                return !std::isspace(c);
            }).base(), userConfirmation.end());
        /* convert to lowercase */
        std::transform(userConfirmation.begin(), userConfirmation.end(), userConfirmation.begin(), [](auto c) {
                return std::tolower(c);
            });

        if (userConfirmation.compare("y") == 0) {
            nextState = ProgramState::DeleteUser;
        } else {
            printf("Action aborted\n");
        }
    }
    break;
    }

    return nextState;
}

}
}
