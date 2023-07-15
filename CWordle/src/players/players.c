/*
 * Wordle program
 * Author: Benjamin Hall
 */

#include "players/players.h"
#include "players/database.h"
#include "collections/vec.h"
#include "util.h"

#include <math.h>
#include <stdio.h>

player_info_t player_info_new(string_t username)
{
    player_info_t player_info = {0};

    player_info.username = username;
    player_info.words_played = hashset_new(string_type_info());

    return player_info;
}

player_info_t player_info_load(string_t username, hashset_t words_played,
                    uint32_t num_guesses[MAX_NUM_GUESSES], uint32_t max_win_streak,
                    uint32_t cur_win_streak)
{
    player_info_t player_info = {0};

    player_info.username = username;
    player_info.words_played = words_played;
    memcpy(player_info.num_guesses, num_guesses, sizeof(player_info.num_guesses));
    player_info.max_win_streak = max_win_streak;
    player_info.cur_win_streak = cur_win_streak;

    return player_info;
}

void player_info_drop(player_info_t *player_info)
{
    if (player_info == NULL) {
        return;
    }
    string_drop(&player_info->username);
    hashset_drop(&player_info->words_played);
}

string_t const *player_info_get_random_word(player_info_t const *player_info, hashset_t const *dictionary)
{
    if (player_info == NULL || dictionary == NULL) {
        return NULL;
    }

    size_t const unplayed_words = dictionary->len - player_info->words_played.len;
    size_t const random_word_idx = rand() % unplayed_words;

    string_t const *random_word = NULL;
    for (size_t i = 0; i == 0 || random_word; ++i) {
        do {
            random_word = hashset_get_next(dictionary, random_word);
        } while (random_word && hashset_contains(&player_info->words_played, random_word));

        if (i == random_word_idx) {
            break;
        }
    }

    return random_word;
}

void player_info_add_won_word(player_info_t *player_info, string_t word, uint32_t num_guesses)
{
    if (player_info == NULL) {
        return;
    }

    hashset_insert(&player_info->words_played, &word);
    ++player_info->num_guesses[num_guesses - 1];
    ++player_info->cur_win_streak;
    player_info->max_win_streak = max(player_info->max_win_streak, player_info->cur_win_streak);
}

void player_info_add_lost_word(player_info_t *player_info, string_t word)
{
    if (player_info == NULL) {
        return;
    }

    hashset_insert(&player_info->words_played, &word);
    player_info->cur_win_streak = 0;
}

string_t player_info_to_string(player_info_t const *player_info)
{
    if (player_info == NULL) {
        return string_with_capacity(0);
    }

    string_t string = string_with_capacity(64);

    string_push_str(&string, "Username: ");
    string_push_str(&string, player_info->username.buf);

    string_push_str(&string, "\nWords Played: ");
    {
        string_t const *str = NULL;
        for (size_t i = 0; i < player_info->words_played.len; ++i) {
            str = hashset_get_next(&player_info->words_played, str);
            string_push_str(&string, str->buf);
            if (i < player_info->words_played.len - 1) {
                string_push_str(&string, ",");
            }
        }
    }

    string_push_str(&string, "\nNumber of Guesses: ");
    for (size_t i = 0; i < MAX_NUM_GUESSES; ++i) {
        char int_str[20] = {0};
        snprintf(int_str, sizeof(int_str) - 1, "%d", player_info->num_guesses[i]);
        if (i < MAX_NUM_GUESSES - 1) {
            int_str[strlen(int_str)] = ',';
        }
        string_push_str(&string, int_str);
    }

    char tmp[20] = {0};

    string_push_str(&string, "\nMaximum Win Streak: ");
    snprintf(tmp, sizeof(tmp) - 1, "%d", player_info->max_win_streak);
    string_push_str(&string, tmp);

    memset(tmp, 0, sizeof(tmp));

    string_push_str(&string, "\nCurrent Win Streak: ");
    snprintf(tmp, sizeof(tmp) - 1, "%d", player_info->cur_win_streak);
    string_push_str(&string, tmp);

    string_push_str(&string, "\n");

    return string;
}

string_t player_info_get_stats(player_info_t const *player_info)
{
    if (player_info == NULL) {
        return string_with_capacity(0);
    }

    char tmp[20] = {0};

    string_t string = string_with_capacity(64);

    string_push_str(&string, "Number of Words Played: ");
    snprintf(tmp, sizeof(tmp) - 1, "%zd", player_info->words_played.len);
    string_push_str(&string, tmp);

    memset(tmp, 0, sizeof(tmp));

    string_push_str(&string, "\nWin Rate: ");
    if (hashset_is_empty(&player_info->words_played)) {
        tmp[0] = 0 + '0';
    } else {
        int win_rate = 0;
        for (size_t i = 0; i < MAX_NUM_GUESSES; ++i) {
            win_rate += player_info->num_guesses[i];
        }
        win_rate = (int)round(100.0 * win_rate / player_info->words_played.len);
        snprintf(tmp, sizeof(tmp) - 1, "%d", win_rate);
    }
    string_push_str(&string, tmp);

    memset(tmp, 0, sizeof(tmp));

    string_push_str(&string, "\nCurrent Win Streak: ");
    snprintf(tmp, sizeof(tmp) - 1, "%d", player_info->cur_win_streak);
    string_push_str(&string, tmp);

    memset(tmp, 0, sizeof(tmp));

    string_push_str(&string, "\nMaximum Win Streak: ");
    snprintf(tmp, sizeof(tmp) - 1, "%d", player_info->max_win_streak);
    string_push_str(&string, tmp);

    memset(tmp, 0, sizeof(tmp));

    string_push_str(&string, "\nGuess Distribution:");
    int max_num_guesses = 0;
    for (size_t i = 0; i < MAX_NUM_GUESSES; ++i) {
        max_num_guesses = max(max_num_guesses, player_info->num_guesses[i]);
    }

    /* print the guess distribution with bars, max 12 */
    double bar_factor;
    if (max_num_guesses == 0) {
        bar_factor = 0;
    } else {
        bar_factor = 12.0 / max_num_guesses;
    }

    for (size_t i = 0; i < MAX_NUM_GUESSES; ++i) {
        size_t const num_bars = (size_t)round(bar_factor * player_info->num_guesses[i]);
        string_reserve(&string, num_bars + 6);

        string_push_str(&string, "\n");
        snprintf(tmp, sizeof(tmp) - 1, "%d", i + 1);
        string_push_str(&string, tmp);
        string_push_str(&string, ": ");

        memset(tmp, 0, sizeof(tmp));

        for (size_t j = 0; j < num_bars; ++j) {
            string_push_str(&string, "=");
        }

        string_push_str(&string, " ");
        snprintf(tmp, sizeof(tmp) - 1, "%d", player_info->num_guesses[i]);
        string_push_str(&string, tmp);

        memset(tmp, 0, sizeof(tmp));
    }

    return string;
}

int player_info_write_to_file(player_info_t const *player_info, char const *filename)
{
    if (player_info == NULL || filename == NULL) {
        return -1;
    }

    FILE *file = fopen(filename, "w");
    string_t player_str = player_info_to_string(player_info);
    int retval = fputs(player_str.buf, file);
    string_drop(&player_str);
    fclose(file);

    if (retval < 0) {
        return -1;
    } else {
        return 0;
    }
}

static int str_to_owned(char *str, void **value)
{
    string_t *string = malloc(sizeof(*string));
    *string = string_from(str);
    *value = string;
    return 0;
}

static int str_to_uint(char *str, void **value)
{
    uint32_t *i = malloc(sizeof(uint32_t));
    *i = atoi(str);
    *value = i;
    return 0;
}

static type_info_t const int_type_info = {
    sizeof(uint32_t),
    NULL, NULL, NULL
};

int player_info_from_file(player_info_t *player_info, char const *filename)
{
    if (player_info == NULL || filename == NULL) {
        return -1;
    }

    int retval = 0;

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return 1;
    }
    string_t file_contents = string_with_capacity(64);

    retval = file_read_line_to_string(&file_contents, file);
    if (retval) {
        goto read_err;
    }

    database_entry_t username;
    retval = database_entry_from_line(&username, file_contents.buf, str_to_owned, string_type_info());
    if (retval) {
        goto user_err;
    }

    retval = file_read_line_to_string(&file_contents, file);
    if (retval) {
        goto user_err;
    }

    hashset_t *words_played_set = malloc(sizeof(*words_played_set));
    *words_played_set = hashset_new(string_type_info());
    database_entry_t words_played;
    retval = database_entry_from_collection(&words_played, file_contents.buf, str_to_owned, words_played_set, hashset_type_info(), hashset_insert);
    if (retval) {
        goto words_err;
    }

    retval = file_read_line_to_string(&file_contents, file);
    if (retval) {
        goto words_err;
    }

    vec_t *num_guesses_vec = malloc(sizeof(*num_guesses_vec));
    *num_guesses_vec = vec_with_capacity(int_type_info, MAX_NUM_GUESSES);
    database_entry_t num_guesses_list;
    retval = database_entry_from_collection(&num_guesses_list, file_contents.buf, str_to_uint, num_guesses_vec, vec_type_info(), vec_push_back);
    if (retval) {
        goto numguess_err;
    }

    retval = file_read_line_to_string(&file_contents, file);
    if (retval) {
        goto numguess_err;
    }

    database_entry_t max_win_streak;
    retval = database_entry_from_line(&max_win_streak, file_contents.buf, str_to_uint, int_type_info);
    if (retval) {
        goto maxwin_err;
    }

    retval = file_read_line_to_string(&file_contents, file);
    if (retval) {
        goto maxwin_err;
    }

    database_entry_t cur_win_streak;
    retval = database_entry_from_line(&cur_win_streak, file_contents.buf, str_to_uint, int_type_info);
    if (retval) {
        goto curwin_err;
    }

    /* parse the number of guesses into an array */
    uint32_t num_guesses[MAX_NUM_GUESSES] = {0};
    uint32_t const *num_guess = NULL;
    for (size_t i = 0; i < MAX_NUM_GUESSES && i < num_guesses_vec->len; ++i) {
        num_guess = vec_get_next(num_guesses_vec, num_guess);
        num_guesses[i] = *num_guess;
    }

    *player_info = player_info_load(
            *(string_t *)move(username.value, &username.type_info),
            *(hashset_t *)move(words_played.value, &words_played.type_info),
            num_guesses,
            *(uint32_t *)move(max_win_streak.value, &max_win_streak.type_info),
            *(uint32_t *)move(cur_win_streak.value, &cur_win_streak.type_info));

curwin_err:
    database_entry_drop(&cur_win_streak);
maxwin_err:
    database_entry_drop(&max_win_streak);
numguess_err:
    database_entry_drop(&num_guesses_list);
words_err:
    database_entry_drop(&words_played);
user_err:
    database_entry_drop(&username);
read_err:
    string_drop(&file_contents);
    fclose(file);

    return retval;
}
