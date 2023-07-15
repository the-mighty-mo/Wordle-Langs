/*
 * Wordle program
 * Author: Benjamin Hall
 */

#include "console_app/main_menu.h"
#include "console_app/game.h"

/**
 * Possible user selections in the main menu.
 */
typedef enum _UserSelection {
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
    _DeleteUser
} UserSelection;

/**
 * Requests a user to enter their username.
 *
 * The user may choose to quit the program (or forcibly
 * quit using Ctrl-C), in which case this function returns
 * null.
 *
 * If the user does not yet exist in the given database,
 * they will be added to it.
 *
 * @param usernames
 *        A set of existing usernames
 */
static string_t *request_username(treeset_t *usernames)
{
    if (!treeset_is_empty(usernames)) {
        printf("List of existing users:\n");
        string_t const *elem = NULL;
        for (size_t i = 0; i < usernames->len; ++i) {
            elem = treeset_get_next(usernames, elem);
            printf("%s\n", elem->buf);
        }
        printf("\n");
    }

    printf("Note: usernames are case-insensitive\n");
    printf("Type \":q\" to exit\n");
    printf("Username: ");
    fflush(stdout);

    string_t *username = malloc(sizeof(*username));
    *username = string_new();
    if (file_read_line_to_string(username, stdin) != 0) {
        /* user likely quit the program with Ctrl-C */
        string_drop(username);
        free(username);
        return NULL;
    }
    string_trim(username);
    string_to_lowercase(username);

    if (strcmp(username->buf, ":q") == 0) {
        /* user wants to exit */
        string_drop(username);
        free(username);
        return NULL;
    }

    if (!treeset_contains(usernames, username)) {
        /* new user, add to database */
        string_t elem = string_clone(username);
        treeset_insert(usernames, &elem);
    }

    return username;
}

player_info_t *request_user_login(treeset_t *usernames)
{
    string_t *username = request_username(usernames);
    if (username == NULL) {
        /* user requested to exit the game */
        return NULL;
    }

    string_t filename = string_clone(username);
    string_push_str(&filename, ".txt");
    player_info_t *player_info = malloc(sizeof(*player_info));
    int retval = player_info_from_file(player_info, filename.buf);
    if (retval < 0) {
        /* error reading the database file */
        printf("Error: corrupt player database file: %s\n", filename.buf);

        free(player_info);
        string_drop(&filename);
        string_drop(username);
        free(username);
        return NULL;
    }
    string_drop(&filename);

    printf("Hello, %s\n", username->buf);

    /* this might be a new user, create a fresh instance of player_info_t if so */
    if (retval > 0) {
        *player_info = player_info_new(*username);
    } else {
        string_drop(username);
    }

    free(username);
    return player_info;
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
 * in which case this function returns null.
 */
static UserSelection *request_user_selection(void)
{
    UserSelection *user_selection = NULL;

    printf("\n");
    printf("[1] Play a game of Wordle\n");
    printf("[2] View player statistics\n");
    printf("[3] Log off\n");
    printf("[4] Delete user\n");

    uint8_t read = 1;
    while (read) {
        printf("Selection: ");
        fflush(stdout);

        string_t selection_str = string_new();
        if (file_read_line_to_string(&selection_str, stdin) != 0) {
            /* user likely quit the program with Ctrl-C */
            return NULL;
        }

        uint8_t const selection = atoi(selection_str.buf);
        if (selection >= 1 && selection <= 4) {
            /* valid selection, stop the read loop */
            user_selection = malloc(sizeof(*user_selection));
            *user_selection = (UserSelection)selection;
            read = 0;
        } else {
            /* invalid selection */
            printf("Error: invalid selection\n");
        }

        string_drop(&selection_str);
    }
    printf("\n");

    return user_selection;
}

ProgramState run_menu(player_info_t *current_player, hashset_t const *dictionary)
{
    ProgramState next_state = MainMenu;

    UserSelection *user_selection = request_user_selection();
    if (user_selection == NULL) {
        /* user likely quit the program with Ctrl-C */
        return Exit;
    }

    switch (*user_selection) {
    case PlayGame:
    {
        /* run a game of Wordle */
        string_t const *rand_word = player_info_get_random_word(current_player, dictionary);
        if (rand_word) {
            wordle_answer_t answer = wordle_answer_new(string_clone(rand_word));
            int retval = run_game(&answer, current_player, dictionary);
            wordle_answer_drop(&answer);

            if (retval != 0) {
                next_state = Exit;
                break;
            }

            /* print the player's statistics after the game ends */
            string_t stats = player_info_get_stats(current_player);
            printf("%s\n", stats.buf);
            string_drop(&stats);

            /* save the user's new statistics to their database */
            string_t filename = string_clone(&current_player->username);
            string_push_str(&filename, ".txt");
            FILE *player_file = fopen(filename.buf, "w");
            if (player_file == NULL) {
                /* report that we could not write to the database, but do not exit */
                printf("Error: could not write to user database file, progress has not been saved\n");
            } else {
                string_t player_str = player_info_to_string(current_player);
                fputs(player_str.buf, player_file);
                string_drop(&player_str);
            }
            fclose(player_file);
            string_drop(&filename);
        } else {
            /* couldn't get a word, player has already played every word */
            printf("There are no remaining words in the dictionary.\n");
        }
    }
    break;
    case ViewStats:
    {
        string_t stats = player_info_get_stats(current_player);
        printf("%s\n", stats.buf);
        string_drop(&stats);
    }
    break;
    case LogOff:
        /* user is logged off, go back to login screen */
        next_state = LogIn;
        break;
    case _DeleteUser:
    {
        printf("Are you sure you would like to delete user: %s [y/N] ", current_player->username.buf);
        fflush(stdout);

        string_t user_confirmation = string_new();
        file_read_line_to_string(&user_confirmation, stdin);
        string_trim(&user_confirmation);
        string_to_lowercase(&user_confirmation);

        if (strcmp(user_confirmation.buf, "y") == 0) {
            printf("\n");
            next_state = DeleteUser;
        } else {
            printf("Action aborted\n");
        }

        string_drop(&user_confirmation);
    }
    break;
    }

    free(user_selection);

    return next_state;
}
