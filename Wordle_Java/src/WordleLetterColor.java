/*
 * Wordle program
 * CS 203 Project 3
 * Benjamin Hall
 * 2022.03.02
 */

/**
 * Possible colors for letters in a game of Wordle.
 *
 * @author Benjamin Hall
 */
public enum WordleLetterColor {
    /** The letter is in the correct position. */
    Green("G"),
    /** The letter is in the word, but in the wrong position. */
    Yellow("Y"),
    /** There are no more instances of the letter in the word. */
    Black("X");

    private final String _name;

    /**
     * Creates a WordleLetterColor with the given name.
     * 
     * @param name
     *        The name to print when calling toString()
     */
    private WordleLetterColor(String name) {
        _name = name;
    }

    @Override
    public String toString() {
        return _name;
    }
}
