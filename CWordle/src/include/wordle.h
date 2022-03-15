/*
 * Wordle program
 * Author: Benjamin Hall
 */

#pragma once

#include "collections/string.h"

/**
 * Possible guess results for a letter in a game of Wordle.
 */
typedef enum _WordleGuess {
    /**
     * Green ("G"), the letter is in the word at that position
     */
    Correct,
    /**
     * Yellow ("Y"), the letter is in the word, but not at that position
     */
    Present,
    /**
     * Gray ("X"), there are no more instances of the letter in the word
     */
    Incorrect
} WordleGuess;

/**
 * Returns a string representation of a Wordle guess.
 * 
 * @param guess
 *        The Wordle guess
 */
char const *wordle_guess_to_str(WordleGuess guess);

/**
 * Stores information about an answer to a game of Wordle.
 * A game of Worlde has a target word. The guessing algorithm
 * uses preprocessing so it can run in linear time. This results
 * in an array containing the counts of each letter.
 */
typedef struct _wordle_answer_t {
    string_t word;
    uint8_t letter_counts[26];
} wordle_answer_t;

#define WORDLE_ANSWER_SIZE 5

/**
 * Contains all the possible messages
 * for a won game of Wordle.
 *
 * If the user guessed the word in n
 * guesses (starting at 1), then
 * WIN_MESSAGES[n - 1] is the message
 * that should be displayed.
 */
static char const *WORDLE_WIN_MESSAGES[] = {
    "Genius",
    "Magnificent",
    "Impressive",
    "Splendid",
    "Great",
    "Phew",
};

/**
 * Creates a new Wordle answer.
 * 
 * @param word
 *        The answer word
 */
wordle_answer_t wordle_answer_new(string_t word);
/**
 * Drops a Wordle answer, freeing its internal resources.
 * 
 * @param answer
 *        The answer to drop
 */
void wordle_answer_drop(wordle_answer_t *answer);

/**
 * Calculates the correctness of a guess.
 * 
 * This function fills an array containing the correctness
 * of each letter of the guess.
 * 
 * It is important to note that the sum of Correct and Present
 * instances for a given letter cannot exceed the total number
 * of instances of the letter in the answer. Additionally,
 * Correct always takes priority over Present.
 * 
 * @param answer
 *        The answer against which to check
 * @param colors
 *        The array of colors to fill
 * @param guess
 *        The guess
 */
void wordle_answer_check_guess(wordle_answer_t const *answer, WordleGuess colors[WORDLE_ANSWER_SIZE], char const *guess);
