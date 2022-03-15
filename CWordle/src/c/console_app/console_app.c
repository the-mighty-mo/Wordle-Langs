/*
 * Wordle program
 * Author: Benjamin Hall
 */

#include "console_app/console_app.h"
#include "console_app/main_menu.h"
#include "collections/string.h"

#include <time.h>
#include <stdio.h>

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
static int save_usernames(treeset_t *usernames, char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        return -1;
    }

    string_t const *username = NULL;
    for (int i = 0; i < usernames->len; ++i) {
        username = treeset_get_next(usernames, username);
        fputs(username->buf, file);
        fputs("\n", file);
    }

    fclose(file);
    return 0;
}

void run_console_app(hashset_t const *dictionary, treeset_t *usernames)
{
    srand(time(NULL));

    ProgramState state = LogIn;
    uint8_t run_program = 1;

    player_info_t *current_player = NULL;

    while (run_program) {
        switch (state) {
        case LogIn:
        {
            player_info_drop(current_player);
            free(current_player);

            current_player = request_user_login(usernames);
            if (current_player == NULL) {
                /* user requested to exit, or there was an error */
                state = Exit;
            } else {
                if (save_usernames(usernames, USERNAMES_FILENAME) != 0) {
                    printf("Error: could not write to the user database\n");
                    state = Exit;
                } else {
                    /* user has logged in, continue to the main menu */
                    state = MainMenu;
                }
            }
        }
        break;
        case MainMenu:
        {
            state = run_menu(current_player, dictionary);
        }
        break;
        case DeleteUser:
        {
            treeset_t tmp_usernames = treeset_new(usernames->type_info);
            string_t const *elem = NULL;
            for (int i = 0; i < usernames->len; ++i) {
                elem = treeset_get_next(usernames, elem);
                if (strcmp(elem->buf, current_player->username.buf) != 0) {
                    treeset_insert(&tmp_usernames, move(elem, &usernames->type_info));
                }
            }
            treeset_drop(usernames);
            *usernames = tmp_usernames;

            string_t filename = string_clone(&current_player->username);
            string_push_str(&filename, ".txt");
            remove(filename.buf);
            string_drop(&filename);

            if (save_usernames(usernames, USERNAMES_FILENAME) != 0) {
                printf("Error: could not write to the user database\n");
                state = Exit;
            } else {
                /* user has logged out, go to the login screen */
                state = LogIn;
            }
        }
        break;
        case Exit:
        {
            run_program = 0;
        }
        break;
        }
    }

    player_info_drop(current_player);
    free(current_player);
}
