#include "wordle.h"

char const *wordle_guess_to_str(WordleGuess guess)
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

wordle_answer_t wordle_answer_new(string_t word)
{
    wordle_answer_t answer = {0};
    answer.word = word;
    for (char const *c = word.buf; *c != '\0'; ++c) {
        ++answer.letter_counts[*c - 'A'];
    }

    return answer;
}

void wordle_answer_drop(wordle_answer_t *answer)
{
    if (answer == NULL) {
        return;
    }
    string_drop(&answer->word);
    memset(answer, 0, sizeof(*answer));
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
        if (answer->word.buf[i] == guess[i]) {
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
