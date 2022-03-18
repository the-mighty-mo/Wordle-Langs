/*
 * Wordle program
 * Author: Benjamin Hall
 */

#include "wordle.h"
#include <algorithm>

namespace wordle {

char const *WordleGuessToString(WordleGuess guess)
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

WordleAnswer::WordleAnswer(std::string word) : m_word{std::move(word)}
{
    for (char c : word) {
        ++m_letterCounts[c - 'A'];
    }
}

std::string const &WordleAnswer::GetWord() const
{
    return m_word;
}

std::array<WordleGuess, 5> WordleAnswer::CheckGuess(std::string const &guess) const
{
    std::array<WordleGuess, 5> colors;
    colors.fill(WordleGuess::Incorrect);

    std::array<uint8_t, 26> letterCounts = m_letterCounts;

    /* first check for green letters */
    for (int i = 0; i < m_word.length(); ++i) {
        if (m_word[i] == guess[i]) {
            --letterCounts[guess[i] - 'A'];
            colors[i] = WordleGuess::Correct;
        }
    }

    /* then check for yellow letters */
    for (int i = 0; i < m_word.length(); ++i) {
        if (colors[i] == WordleGuess::Incorrect) {
            /* letter has not yet been checked */
            if (letterCounts[guess[i] - 'A'] > 0) {
                /* letter in word but not this position */
                colors[i] = WordleGuess::Present;
                --letterCounts[guess[i] - 'A'];
            }
        }
    }

    return colors;
}

}
