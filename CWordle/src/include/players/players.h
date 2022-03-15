#pragma once

#include "collections/hashset.h"
#include "collections/string.h"

#define MAX_NUM_GUESSES 6

/**
 * Contains information about a Wordle player.
 * 
 * A player has a:
 * - username
 * - list of words played
 * - guess distribution
 * - maximum win streak
 * - current win streak
 */
typedef struct _player_info_t {
    string_t username;
    hashset_t words_played;
    uint32_t num_guesses[MAX_NUM_GUESSES];
    uint32_t max_win_streak;
    uint32_t cur_win_streak;
} player_info_t;

/**
 * Initializes data for a new player.
 * 
 * @param username
 *        The username of the player
 */
player_info_t player_info_new(string_t username);
/**
 * Loads data for an existing player.
 * 
 * @param username
 *        The username of the player
 * @param words_played
 *        A set of words the player has already played
 * @param num_guesses
 *        An array containing the number of times the
 *        player has guessed a word in (i + 1) attempts,
 *        where i is the index of the array from [0, 6)
 * @param max_win_streak
 *        The maximum winning streak
 * @param cur_win_streak
 *        The current winning streak
 */
player_info_t player_info_load(string_t username, hashset_t words_played,
                    uint32_t num_guesses[MAX_NUM_GUESSES], uint32_t max_win_streak,
                    uint32_t cur_win_streak);
/**
 * Drops a player's information, freeing its internal resources.
 * 
 * @param player_info
 *        The player info to drop
 */
void player_info_drop(player_info_t *player_info);

/**
 * Gets a random word the player has not yet played.
 * 
 * @param player_info
 *        The player
 * @param dictionary
 *        The dictionary from which to find a word
 */
string_t player_info_get_random_word(player_info_t const *player_info, hashset_t const *dictionary);
/**
 * Adds a word the player has successfully guessed to their database.
 * 
 * This function increments the current win streak, adds
 * the guess to the list of words played and to the guess
 * distribution, and updates the max win streak if appropriate.
 * 
 * @param player_info
 *        The player
 * @param word
 *        The word the player successfully guessed
 * @param num_guesses
 *        The number of guesses the player made
 */
void player_info_add_won_word(player_info_t *player_info, string_t word, uint32_t num_guesses);
/**
 * Adds a word the player has failed to guess to their database.
 * 
 * This function resets the current win streak to 0 and adds
 * the guess to the list of words played. The number of guesses
 * is not added to the player's guess distribution.
 * 
 * @param player_info
 *        The player
 * @param word
 *        The word the player failed to guess
 */
void player_info_add_lost_word(player_info_t *player_info, string_t word);

/**
 * Returns a string representation of a player's information.
 * 
 * @param player_info
 *        The player
 */
string_t player_info_to_string(player_info_t const *player_info);
/**
 * Returns a string with formatted player statistics.
 * 
 * Player statistics consist of:
 * - Number of words played
 * - Win rate
 * - Current and max win streak
 * - Guess distribution
 * 
 * @param player_info
 *        The player from which to generate the statistics
 */
string_t player_info_get_stats(player_info_t const *player_info);

/**
 * Writes a player's data to a file.
 * 
 * @param player_info
 *        The player to write
 * @param filename
 *        The file to which to write
 */
int player_info_write_to_file(player_info_t const *player_info, char const *filename);
/**
 * Reads a player's information from a file.
 * 
 * The function returns on the first error it encounters.
 * 
 * @param plaayer_info
 *        A pointer into which the player info is loaded
 * @param filename
 *        The file from which to read
 * @return
 *        0 on success, 1 if the file does not exist,
 *        -1 on any other error
 */
int player_info_from_file(player_info_t *player_info, char const *filename);
