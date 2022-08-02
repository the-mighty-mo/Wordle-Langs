/*
 * Wordle program
 * Author: Benjamin Hall
 * Created: 2022.03.02
 */

import java.util.*;

/**
 * A class to manage a single game of Wordle.
 * 
 * @author Benjamin Hall
 */
public class Wordle {
    private final String _word;
    private final int[] _letterCounts = new int[26];

    private static final String[] WIN_MESSAGES = new String[] {
        "Genius",
        "Magnificent",
        "Impressive",
        "Splendid",
        "Great",
        "Phew",
    };

    /**
     * Create a new Wordle game.
     * 
     * @param word
     *        The target word; must be uppercase
     */
    public Wordle(String word) {
        _word = word;
        /* preprocess the number of each letter */
        for (char c : _word.toCharArray()) {
            ++_letterCounts[c - 'A'];
        }
    }

    /**
     * Runs this game of Wordle.
     * 
     * @param reader
     *        The Scanner for user input
     * @param player
     *        Information about the player
     * @param dictionary
     *        The dictionary to use
     */
    public void run(Scanner reader, PlayerInfo player, Set<String> dictionary) {
        System.out.println("Guess the 5-letter word in 6 or fewer guesses.");
        System.out.println("After each guess, each letter will be given a color:");
        System.out.println("G = Green:\tletter is in that position in the word");
        System.out.println("Y = Yellow:\tletter is in the word, but not that position");
        System.out.println("X = Black:\tthere are no more instances of the letter in the word");
        System.out.println();

        boolean wonGame = false;
        int i;
        for (i = 1; i <= 6; ++i) {
            String guess = null;
            boolean read = true;
            while (read) {
                System.out.print("[" + i + "] ");
                if (!reader.hasNextLine()) {
                    /* user likely quit the program with Ctrl-C */
                    return;
                }

                guess = reader.nextLine().toUpperCase();
                if (guess.length() != 5) {
                    System.out.println("Error: guess must be 5 letters");
                } else if (!dictionary.contains(guess)) {
                    System.out.println("Error: guess must be a word in the dictionary");
                } else {
                    /* valid guess, stop the read loop */
                    read = false;
                }
            }

            System.out.print("    ");
            WordleLetterColor[] colors = guess(guess);
            for (WordleLetterColor color : colors) {
                System.out.print(color.toString());
            }
            System.out.println();

            wonGame = Arrays.stream(colors)
                .allMatch(c -> c == WordleLetterColor.Green);
            if (wonGame) {
                /* break without incrementing i */
                break;
            }
        }

        if (wonGame) {
            player.addWonWord(_word, i);
            System.out.print(WIN_MESSAGES[i - 1] + "! ");
        } else {
            player.addLostWord(_word);
            System.out.print("Too bad! ");
        }
        System.out.println("The word was: " + _word);
        System.out.println();
    }

    /**
     * Guess a word for the current Wordle game.
     * 
     * @param guess
     *        The word to guess, in upper case
     * @return
     *        Array containing the color of each letter
     */
    public WordleLetterColor[] guess(String guess) {
        WordleLetterColor[] colors = new WordleLetterColor[5];
        int[] letterCounts = _letterCounts.clone();

        /* first check for green letters */
        for (int i = 0; i < _word.length(); ++i) {
            if (guess.charAt(i) == _word.charAt(i)) {
                colors[i] = WordleLetterColor.Green;
                --letterCounts[guess.charAt(i) - 'A'];
            }
        }

        /* then check for yellow letters */
        for (int i = 0; i < guess.length(); ++i) {
            if (colors[i] == null) {
                /* letter does not yet have a color */
                if (letterCounts[guess.charAt(i) - 'A'] > 0) {
                    /* letter in word but not this position, make it yellow */
                    colors[i] = WordleLetterColor.Yellow;
                    --letterCounts[guess.charAt(i) - 'A'];
                } else {
                    /* no more of this letter in word, make it black */
                    colors[i] = WordleLetterColor.Black;
                }
            }
        }

        return colors;
    }
}
