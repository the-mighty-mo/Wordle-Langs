#pragma once

#include <stdint.h>

typedef enum _WordleGuess {
    Correct,
    Present,
    Incorrect
} WordleGuess;

char const *guess_to_str(WordleGuess guess);

typedef struct _wordle_answer_t {
    char const *word;
    uint8_t letter_counts[26];
} wordle_answer_t;

#define WORDLE_ANSWER_SIZE 5

char const *WORDLE_WIN_MESSAGES[] = {
    "Genius",
    "Magnificent",
    "Impressive",
    "Splendid",
    "Great",
    "Phew",
};

int wordle_answer_init(wordle_answer_t *answer, char const *word);
int wordle_answer_check_guess(wordle_answer_t const *answer, WordleGuess colors[WORDLE_ANSWER_SIZE], char const *guess);
