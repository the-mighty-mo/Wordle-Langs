#pragma once

#include "collections/hashset.h"
#include "collections/string.h"

#define MAX_NUM_GUESSES 6

typedef struct _player_info_t {
    string_t username;
    hashset_t words_played;
    uint32_t num_guesses[MAX_NUM_GUESSES];
    uint32_t max_win_streak;
    uint32_t cur_win_streak;
} player_info_t;

int player_info_new(player_info_t *player_info, string_t username);
int player_info_load(player_info_t *player_info, string_t username, hashset_t words_played,
                    uint32_t num_guesses[MAX_NUM_GUESSES], uint32_t max_win_streak,
                    uint32_t cur_win_streak);
void player_info_drop(player_info_t *player_info);

string_t player_info_get_random_word(player_info_t const *player_info, hashset_t const *dictionary);
void player_info_add_won_word(player_info_t *player_info, string_t word, uint32_t num_guesses);
void player_info_add_lost_word(player_info_t *player_info, string_t word);

string_t player_info_to_string(player_info_t const *player_info);
string_t player_info_get_stats(player_info_t const *player_info);

int player_info_write_to_file(player_info_t const *player_info, char const *filename);
int player_info_from_file(player_info_t *player_info, char const *filename);
