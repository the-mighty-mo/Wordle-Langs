/*
 * Wordle program
 * Author: Benjamin Hall
 */

#pragma once

#include "collections/hashset.h"
#include "players/players.h"
#include "wordle.h"

/**
 * Runs a game of Wordle.
 *
 * This function manages all user input and output using
 * stdin and stdout, respectively, as well as all six
 * guesses. The function ends after the user has guessed
 * the answer or used all six guesses, whichever is first.
 *
 * At the start of the game, a message is printed with
 * instructions for the player.
 * 
 * @param answer
 *        The answer to the game of Wordle
 * @param player
 *        The player playing the game
 * @param dictionary
 *        A dictionary of valid Wordle guesses
 */
void run_game(wordle_answer_t const *answer, player_info_t *player, hashset_t const *dictionary);
