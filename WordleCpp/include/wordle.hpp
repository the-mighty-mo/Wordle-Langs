/*
 * Wordle program
 * Author: Benjamin Hall
 */

#pragma once

#include <array>
#include <string>
#include <stdint.h>

namespace wordle {

/**
 * Possible guess results for a letter in a game of Wordle.
 */
enum class WordleGuess {
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
};

/**
 * Returns a string representation of a Wordle guess.
 *
 * @param guess
 *        The Wordle guess
 */
constexpr char const *WordleGuessToString(WordleGuess guess)
{
    switch (guess) {
    case WordleGuess::Correct:
        return "G";
    case WordleGuess::Present:
        return "Y";
    case WordleGuess::Incorrect:
    default:
        return "X";
    }
}

/**
 * Manages information about an answer to a game of Wordle.
 * A game of Wordle has a target word. The guessing algorithm
 * uses preprocessing so it can run in linear time. This results
 * in an array containing the counts of each letter.
 */
class WordleAnswer {
private:
    std::string const m_word;
    std::array<uint8_t, 26> const m_letterCounts;

public:
    /**
     * Creates a new Wordle answer.
     *
     * @param word
     *        The answer word; must be uppercase
     */
    WordleAnswer(std::string word);

    /**
     * Returns the target word for this answer.
     */
    constexpr std::string const &GetWord() const
    {
        return m_word;
    }

    /**
     * Calculates the correctness of a guess.
     *
     * This function returns an array containing the correctness
     * of each letter of the guess.
     *
     * It is important to note that the sum of Correct and Present
     * instances for a given letter cannot exceed the total number
     * of instances of the letter in the answer. Additionally,
     * Correct always takes priority over Present.
     *
     * @param guess
     *        The guess
     */
    std::array<WordleGuess, 5> CheckGuess(std::string_view guess) const;
};

/**
 * Contains all the possible messages
 * for a won game of Wordle.
 *
 * If the user guessed the word in n
 * guesses (starting at 1), then
 * WIN_MESSAGES[n - 1] is the message
 * that should be displayed.
 */
static constexpr char const *WORDLE_WIN_MESSAGES[] = {
    "Genius",
    "Magnificent",
    "Impressive",
    "Splendid",
    "Great",
    "Phew",
};

}
