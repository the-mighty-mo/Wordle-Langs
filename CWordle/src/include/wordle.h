#pragma once

#include <stdint.h>

#include "collections/string.h"

typedef enum _WordleGuess {
    Correct,
    Present,
    Incorrect
} WordleGuess;

char const *wordle_guess_to_str(WordleGuess guess);

typedef struct _wordle_answer_t {
    string_t word;
    uint8_t letter_counts[26];
} wordle_answer_t;

#define WORDLE_ANSWER_SIZE 5

static char const *WORDLE_WIN_MESSAGES[] = {
    "Genius",
    "Magnificent",
    "Impressive",
    "Splendid",
    "Great",
    "Phew",
};

wordle_answer_t wordle_answer_new(string_t word);
int wordle_answer_check_guess(wordle_answer_t const *answer, WordleGuess colors[WORDLE_ANSWER_SIZE], char const *guess);
