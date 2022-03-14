#include "wordle.h"

#include <string.h>

char const *guess_to_str(WordleGuess guess)
{
    switch (guess) {
    case Correct:
        return "G";
    case Present:
        return "Y";
    case Incorrect:
    default:
        return "X";
    }
}

int wordle_answer_init(wordle_answer_t *answer, char const *word)
{
    if (answer == NULL || word == NULL) {
        return -1;
    }

    answer->word = word;
    for (char const *c = word; *c != '\0'; ++c) {
        ++answer->letter_counts[*c - 'A'];
    }

    return 0;
}

int wordle_answer_check_guess(wordle_answer_t const *answer, WordleGuess colors[WORDLE_ANSWER_SIZE], char const *guess)
{
    if (answer == NULL || guess == NULL) {
        return -1;
    }

    for (int i = 0; i < WORDLE_ANSWER_SIZE; ++i) {
        colors[i] = Incorrect;
    }

    uint8_t letter_counts[26];
    memcpy(letter_counts, answer->letter_counts, 26);

    /* first check for green letters */
    for (int i = 0; i < WORDLE_ANSWER_SIZE; ++i) {
        if (answer->word[i] == guess[i]) {
            --letter_counts[guess[i] - 'A'];
            colors[i] = Correct;
        }
    }

    /* then check for yellow letters */
    for (int i = 0; i < WORDLE_ANSWER_SIZE; ++i) {
        if (colors[i] == Incorrect) {
            /* letter has not yet been checked */
            if (letter_counts[guess[i] - 'A'] > 0) {
                /* letter in word but not this position */
                colors[i] = Present;
                --letter_counts[guess[i] - 'A'];
            }
        }
    }

    return 0;
}
