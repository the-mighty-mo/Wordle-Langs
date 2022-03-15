#include "console_app/game.h"

#include <stdio.h>

void run_game(wordle_answer_t const *answer, player_info_t *player, hashset_t const *dictionary)
{
    printf("Guess the 5-letter word in 6 or fewer guesses.\n");
    printf("After each guess, each letter will be given a color:\n");
    printf("G = Green:\tletter is in that position in the word\n");
    printf("Y = Yellow:\tletter is in the word, but not that position\n");
    printf("X = Black:\tthere are no more instances of the letter in the word\n");
    printf("\n");

    uint8_t won = 0;

    int i;
    for (i = 1; i <= 6; ++i) {
        string_t guess = string_with_capacity(6);
        uint8_t read = 1;
        while (read) {
            printf("[%d] ", i);
            
            if (file_read_line_to_string(&guess, stdin) != 0) {
                /* user likely quit the program with Ctrl-C */
                return;
            }
            string_trim(&guess);
            string_to_uppercase(&guess);
            if (guess.len != 5) {
                printf("Error: guess must be 5 letters\n");
            } else if (!hashset_contains(dictionary, &guess)) {
                printf("Error: guess must be a word in the dictionary");
            } else {
                /* valid guess, stop the read loop */
                read = 0;
            }
        }

        WordleGuess colors[WORDLE_ANSWER_SIZE];
        wordle_answer_check_guess(answer, colors, guess.buf);
        printf("    ");
        for (int i = 0; i < WORDLE_ANSWER_SIZE; ++i) {
            printf("%s", wordle_guess_to_str(colors[i]));
        }
        printf("\n");

        string_drop(&guess);

        won = 1;
        for (int i = 0; i < WORDLE_ANSWER_SIZE; ++i) {
            if (colors[i] != Correct) {
                won = 0;
                break;
            }
        }

        if (won) {
            break;
        }
    }

    if (won) {
        player_info_add_won_word(player, string_clone(&answer->word), i);
        printf("%s! ", WORDLE_WIN_MESSAGES[i - 1]);
    } else {
        player_info_add_lost_word(player, string_clone(&answer->word));
        printf("Too bad! ");
    }
    printf("The word was: %s\n", answer->word.buf);
    printf("\n");
}
