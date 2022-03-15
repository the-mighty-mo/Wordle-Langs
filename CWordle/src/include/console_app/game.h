#pragma once

#include "collections/hashset.h"
#include "players/players.h"
#include "wordle.h"

void run_game(wordle_answer_t const *answer, player_info_t *player, hashset_t const *dictionary);