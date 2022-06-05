/*
 * Wordle program
 * Author: Benjamin Hall
 */

#pragma once

#include "collections/hashset.h"
#include "collections/treeset.h"
#include "players/players.h"

/**
 * Possible states of the main Wordle program.
 */
typedef enum _ProgramState {
    /**
     * Request the user's login information
     */
    LogIn,
    /**
     * Run the main menu
     */
    MainMenu,
    /**
     * Delete the current user
     */
    DeleteUser,
    /**
     * Exit the program
     */
    Exit,
} ProgramState;

/**
 * Requests a user to enter their login information.
 *
 * The user may choose to quit the program (or forcibly
 * quit using Ctrl-C), in which case this function returns
 * None. Otherwise, this function returns information
 * about the player.
 *
 * If the user does not yet exist in the given databse,
 * they will be added to it.
 * 
 * @param usernames
 *        A set of existing usernames
 * @return
 *        Information about the player, or null if the program
 *        should exit (either by user request or a database error)
 */
player_info_t *request_user_login(treeset_t *usernames);
/**
 * Runs the Wordle main menu.
 *
 * The main menu gives the player four options:
 * - Play a game of Wordle
 * - View their statistics
 * - Log out
 * - Delete their account
 *
 * This function lets the caller know what the next
 * state of the program should be. For example, if
 * the user has logged off, the main program should
 * return to the login screen.
 * 
 * @param current_player
 *        The logged-in player
 * @param dictionary
 *        A dictionary of valid Wordle words
 */
ProgramState run_menu(player_info_t *current_player, hashset_t const *dictionary);
