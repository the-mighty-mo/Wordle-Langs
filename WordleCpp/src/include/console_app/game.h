/*
 * Wordle program
 * Author: Benjamin Hall
 */

#pragma once

#include "wordle.h"
#include "players/players.h"

namespace console_app {
namespace game {

using players::PlayerInfo;
using wordle::WordleAnswer;

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
 * @return
 *        -1 if there was an error
 */
int RunGame(WordleAnswer const &answer, PlayerInfo &player, std::unordered_set<std::string> const &dictionary);

}
}
