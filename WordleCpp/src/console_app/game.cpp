/*
 * Wordle program
 * Author: Benjamin Hall
 */

#include "console_app/game.hpp"

#include <algorithm>
#include <iostream>

namespace console_app {
namespace game {

using wordle::WordleGuess;
using wordle::WORDLE_WIN_MESSAGES;

int Run(WordleAnswer const &answer, PlayerInfo &player, std::unordered_set<std::string> const &dictionary)
{
    printf("Guess the 5-letter word in 6 or fewer guesses.\n");
    printf("After each guess, each letter will be given a color:\n");
    printf("G = Green:\tletter is in that position in the word\n");
    printf("Y = Yellow:\tletter is in the word, but not that position\n");
    printf("X = Black:\tthere are no more instances of the letter in the word\n");
    printf("\n");

    std::optional<int> const wonGame = [&]() -> std::optional<int> {
        for (int i = 1; i <= 6; ++i) {
            std::string guess;
            uint8_t read = 1;
            while (read) {
                printf("[%d] ", i);
                fflush(stdout);

                if (!std::getline(std::cin, guess)) {
                    /* user likely quit the program with Ctrl-C */
                    return std::optional{-1};
                }
                /* trim string */
                guess.erase(guess.begin(), std::find_if(guess.begin(), guess.end(), [](auto c) {
                        return !std::isspace(c);
                    }));
                guess.erase(std::find_if(guess.rbegin(), guess.rend(), [](auto c) {
                        return !std::isspace(c);
                    }).base(), guess.end());
                /* convert to uppercase */
                std::transform(guess.begin(), guess.end(), guess.begin(), [](auto c) {
                        return std::toupper(c);
                    });

                if (guess.length() != 5) {
                    printf("Error: guess must be 5 letters\n");
                } else if (dictionary.find(guess) == dictionary.end()) {
                    printf("Error: guess must be a word in the dictionary\n");
                } else {
                    /* valid guess, stop the read loop */
                    read = 0;
                }
            }

            auto const colors = answer.CheckGuess(guess);
            printf("    ");
            for (auto color : colors) {
                printf("%s", WordleGuessToString(color));
            }
            printf("\n");

            bool const won = std::all_of(colors.begin(), colors.end(), [](auto color) {
                    return color == WordleGuess::Correct;
                });

            if (won) {
                return std::optional{i};
            }
        }
        return std::nullopt;
    }();

    if (wonGame.has_value() && *wonGame == -1) {
        /* user likely quit the program with Ctrl-C */
        return -1;
    } else if (wonGame.has_value()) {
        player.AddWonWord(answer.GetWord(), *wonGame);
        printf("%s! ", WORDLE_WIN_MESSAGES[*wonGame - 1]);
    } else {
        player.AddLostWord(answer.GetWord());
        printf("Too bad! ");
    }
    printf("The word was: %s\n", answer.GetWord().c_str());
    printf("\n");

    return 0;
}

}
}
