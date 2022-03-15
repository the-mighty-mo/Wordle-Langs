#pragma once

#include "collections/hashset.h"
#include "collections/treeset.h"
#include "players/players.h"

typedef enum _ProgramState {
    LogIn,
    MainMenu,
    DeleteUser,
    Exit,
} ProgramState;

player_info_t *request_user_login(treeset_t *usernames);
ProgramState run_menu(player_info_t *current_player, hashset_t *const dictionary);
