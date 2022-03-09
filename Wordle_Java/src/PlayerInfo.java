/*
 * Wordle program
 * CS 203 Project 3
 * Benjamin Hall
 * 2022.03.06
 */

import java.io.*;
import java.util.*;
import java.util.function.*;
import java.util.stream.*;

/**
 * A class to contain information about a player.
 * 
 * @author Benjamin Hall
 */
public class PlayerInfo {
    private final String _username;
    private final HashSet<String> _wordsPlayed;
    private final int[] _numGuesses;
    private int _maxWinStreak;
    private int _curWinStreak;

    /**
     * Creates an object for a new player.
     * 
     * @param username
     *        The username of the player
     */
    public PlayerInfo(String username) {
        this(username, new HashSet<String>(), new int[6], 0, 0);
    }

    /**
     * Creates an object containing information about an existing player.
     * 
     * @param username
     *        The username of the player
     * @param wordsPlayed
     *        A set of words the player has already played
     * @param numGuesses
     *        An array containing the number of times the
     *        player has guessed a word in (i + 1) attempts,
     *        where i is the index of the array from [0, 6)
     * @param maxWinStreak
     *        The maximum winning streak
     * @param curWinStreak
     *        The current winning streak
     */
    public PlayerInfo(
        String username,
        HashSet<String> wordsPlayed,
        int[] numGuesses,
        int maxWinStreak,
        int curWinStreak
    ) {
        _username = username;
        _wordsPlayed = wordsPlayed;
        _numGuesses = numGuesses;
        _maxWinStreak = maxWinStreak;
        _curWinStreak = curWinStreak;
    }

    /**
     * Returns the username of this player.
     */
    public String getUsername() {
        return _username;
    }

    /**
     * Gets a random, unplayed word for a new Wordle game.
     * 
     * @param dictionary
     *        The dictionary from which to find a word
     * @return
     *        A random word the player has not yet played
     */
    public String getRandomWord(Set<String> dictionary) {
        int unplayedWordsCount = dictionary.size() - _wordsPlayed.size();
        int randomWordIdx = (int)(Math.random() * unplayedWordsCount);
        return dictionary.stream()
            .filter(Predicate.not(_wordsPlayed::contains))
            .skip(randomWordIdx)
            .findFirst().get()
            .toUpperCase();
    }

    /**
     * Adds a word that the player successfully guessed
     * to the database.
     * 
     * @param word
     *        The word the player successfully guessed
     * @param numGuesses
     *        The number of guesses the player made
     */
    public void addWonWord(String word, int numGuesses) {
        _wordsPlayed.add(word);
        ++_numGuesses[numGuesses - 1];
        ++_curWinStreak;
        _maxWinStreak = Math.max(_maxWinStreak, _curWinStreak);
    }

    /**
     * Adds a word that the player failed to guess
     * to the database.
     * 
     * @param word
     *        The word the player failed to guess
     */
    public void addLostWord(String word) {
        _wordsPlayed.add(word);
        _curWinStreak = 0;
    }

    /**
     * Returns a formatted string with the player's
     * current statistics.
     */
    public String getStats() {
        StringBuilder builder = new StringBuilder();
        builder.append("Number of Words Played: ")
            .append(_wordsPlayed.size())
            .append(System.lineSeparator());

        int winRate;
        if (_wordsPlayed.size() == 0) {
            winRate = 0;
        } else {
            int totalNumGuesses = Arrays.stream(_numGuesses).sum();
            winRate = (int)Math.round(100.0 * totalNumGuesses / _wordsPlayed.size());
        }
        builder.append("Win Rate: ")
            .append(winRate).append("%")
            .append(System.lineSeparator());

        builder.append("Current Win Streak: ")
            .append(_curWinStreak)
            .append(System.lineSeparator());

        builder.append("Maximum Win Streak: ")
            .append(_maxWinStreak)
            .append(System.lineSeparator());

        builder.append("Guess Distribution:");
        OptionalInt maxNumGuesses = Arrays.stream(_numGuesses).max();

        /* print the guess distribution with bars, max 12 */
        double barFactor;
        if (maxNumGuesses.isPresent()) {
            barFactor = 12.0 / maxNumGuesses.getAsInt();
        } else {
            barFactor = 0;
        }

        for (int i = 0; i < _numGuesses.length; ++i) {
            int numBars = (int)Math.round(barFactor * _numGuesses[i]);
            String bars = "=".repeat(numBars);

            builder.append(System.lineSeparator())
                .append(i + 1).append(": ")
                .append(bars).append(" ")
                .append(_numGuesses[i]);
        }

        return builder.toString();
    }

    /**
     * Returns a String representation of this player's
     * information for saving to a database.
     */
    @Override
    public String toString() {
        StringBuilder builder = new StringBuilder();
        builder.append("Username: ")
            .append(_username)
            .append(System.lineSeparator());

        builder.append("Words Played: ")
            .append(String.join(",", _wordsPlayed))
            .append(System.lineSeparator());

        String numGuessesStr = Arrays.stream(_numGuesses)
            .mapToObj(Integer::toString)
            .collect(Collectors.joining(","));
        builder.append("Number of Guesses: ")
            .append(numGuessesStr)
            .append(System.lineSeparator());

        builder.append("Maximum Win Streak: ")
            .append(_maxWinStreak)
            .append(System.lineSeparator());

        builder.append("Current Win Streak: ")
            .append(_curWinStreak);

        return builder.toString();
    }

    /**
     * Writes this player's information to a file.
     * 
     * @param fileName
     *        The name of the file
     * 
     * @throws IOException
     *        if the data could not be written to the file
     */
    public void writeToFile(String fileName) throws IOException {
        File file = new File(fileName);
        /* make sure file exists */
        file.createNewFile();

        BufferedWriter writer = new BufferedWriter(new FileWriter(file));
        writer.append(toString());
        writer.close();
    }

    /**
     * Parses a player's information from a file and creates a new
     * object containing the parsed information.
     * 
     * @param fileName
     *        The name of the player file
     * @return
     *        An object containing information about the player
     * 
     * @throws IOException
     *        if the data could not be read from the file
     */
    public static PlayerInfo createFromFile(String fileName) throws IOException, NumberFormatException {
        File file = new File(fileName);
        if (!file.exists()) {
            /* the database does not yet exist */
            return null;
        }

        /* read all the lines in the file */
        ArrayList<String> linesInFile = new ArrayList<String>();
        {
            Scanner reader = new Scanner(file);
            while (reader.hasNextLine()) {
                linesInFile.add(reader.nextLine());
            }
            reader.close();
        }

        if (linesInFile.size() != 5) {
            /* corrupt database file */
            throw new IOException("Error: corrupt player database file: " + fileName);
        }

        /* parse the lines in the file */
        DatabaseEntry<String> username = PlayerDatabaseParser.parseLine(linesInFile.get(0), s -> s);
        DatabaseEntry<HashSet<String>> wordsPlayed = PlayerDatabaseParser.parseSet(linesInFile.get(1), s -> s, HashSet::new);
        DatabaseEntry<ArrayList<Integer>> numGuessesList = PlayerDatabaseParser.parseList(linesInFile.get(2), Integer::parseInt, ArrayList::new);
        DatabaseEntry<Integer> maxWinStreak = PlayerDatabaseParser.parseLine(linesInFile.get(3), Integer::parseInt);
        DatabaseEntry<Integer> curWinStreak = PlayerDatabaseParser.parseLine(linesInFile.get(4), Integer::parseInt);

        /* parse the number of guesses into an array */
        int[] numGuesses = new int[6];
        for (int i = 0; i < numGuesses.length && i < numGuessesList.value.size(); ++i) {
            numGuesses[i] = numGuessesList.value.get(i);
        }

        return new PlayerInfo(username.value, wordsPlayed.value, numGuesses, maxWinStreak.value, curWinStreak.value);
    }
}
