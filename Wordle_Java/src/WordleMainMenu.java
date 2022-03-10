/*
 * Wordle program
 * Author: Benjamin Hall
 * Created: 2022.03.07
 */

import java.io.*;
import java.util.*;

/**
 * A class to manage the main menu
 * of the Wordle program.
 * 
 * @author Benjamin Hall
 */
public class WordleMainMenu {
    private final Scanner _reader;

    /**
     * Creates a new Wordle main menu.
     * 
     * @param reader
     *        The Scanner for user input
     */
    public WordleMainMenu(Scanner reader) {
        _reader = reader;
    }

    /**
     * An enum representing all the possible
     * selections the user can make.
     */
    private static enum UserSelection {
        PlayGame,
        ViewStats,
        LogOff,
        DeleteUser;

        /* cache the result of values() so fromInt() is constant time */
        private static final UserSelection[] _values = values();

        /**
         * Returns the user selection associated with the
         * given integer value.
         * 
         * @param value
         *        The integer value
         * @return
         *        The associated user selection, or
         *        null if the value is out of range
         */
        public static UserSelection fromInt(int value) {
            if (value >= 0 && value < _values.length) {
                return _values[value];
            } else {
                return null;
            }
        }
    }

    /**
     * Requests the user to login and loads the player's data.
     * 
     * @param usernames
     *        A set of existing usernames
     * @return
     *        An object containing information about the player,
     *        or null if the program should exit (either due to
     *        a database error or user request)
     */
    public PlayerInfo requestUserLogin(TreeSet<String> usernames) {
        String username = requestUsername(usernames);
        if (username == null) {
            /* user requested to exit the game */
            return null;
        }

        PlayerInfo playerInfo;
        try {
            playerInfo = PlayerInfo.createFromFile(username + ".txt");
        } catch (IOException e) {
            /* either we couldn't read the file, or the database is corrupt */
            System.err.println(e.getMessage());
            return null;
        } catch (NumberFormatException n) {
            /* couldn't parse an integer entry, database is likely corrupt */
            System.err.println("Error: corrupt player database file: " + username + ".txt");
            return null;
        }

        if (playerInfo == null) {
            /* this is a new user, create a fresh instance of PlayerInfo */
            playerInfo = new PlayerInfo(username);
        }

        System.out.println("Hello, " + username);
        return playerInfo;
    }

    /**
     * Requests that the player inputs their username.
     * 
     * @param usernames
     *        A set of existing usernames
     * @return
     *        The player's username, or null
     *        if the user wishes to exit
     */
    private String requestUsername(TreeSet<String> usernames) {
        if (usernames.size() > 0) {
            System.out.println("List of existing users:");
            usernames.stream().forEach(System.out::println);
            System.out.println();
        }

        System.out.println("Note: usernames are case-insensitive");
        System.out.println("Type \":q\" to exit");
        System.out.print("Username: ");
        if (!_reader.hasNextLine()) {
            /* user likely quit the program with Ctrl-C */
            return null;
        }
        String username = _reader.nextLine().toLowerCase();

        if (username.equals(":q")) {
            /* user wants to exit */
            return null;
        }

        if (!usernames.contains(username)) {
            /* new user, add to database */
            usernames.add(username);
        }

        return username;
    }

    /**
     * Runs the Wordle main menu's state machine.
     * 
     * @param currentPlayer
     *        The logged in player
     * @param dictionary
     *        The dictionary of five-letter words
     * @return
     *        The next state of the program
     */
    public ProgramState run(PlayerInfo currentPlayer, Set<String> dictionary) {
        ProgramState nextState = ProgramState.MainMenu;

        UserSelection userSelection = requestUserSelection();
        if (userSelection == null) {
            /* user likely quit the program with Ctrl-C */
            return ProgramState.Exit;
        }

        switch (userSelection) {
        case PlayGame:
            /* run a game of Wordle */
            Wordle wordle = new Wordle(currentPlayer.getRandomWord(dictionary));
            wordle.run(_reader, currentPlayer, dictionary);
            /* print the player's statistics after the game ends */
            System.out.println(currentPlayer.getStats());
            /* save the user's new statistics to their database */
            try {
                currentPlayer.writeToFile(currentPlayer.getUsername() + ".txt");
            } catch (IOException e) {
                /* report that we could not write to the database, but do not exit */
                System.err.println("Error: could not write to user database file," +
                    "progress has not been saved");
            }
            break;

        case ViewStats:
            System.out.println(currentPlayer.getStats());
            break;

        case LogOff:
            /* user is logged off, go back to login screen */
            nextState = ProgramState.LogIn;
            break;

        case DeleteUser:
            System.out.print("Are you sure you would like to delete user: " + currentPlayer.getUsername() + " [y/N] ");
            String userConfirmation = null;
            if (_reader.hasNextLine()) {
                userConfirmation = _reader.nextLine().toLowerCase();
            }

            if (userConfirmation != null && userConfirmation.equals("y")) {
                /* tell the caller to delete the current user */
                nextState = ProgramState.DeleteUser;
            } else {
                System.out.println("Action aborted");
            }
            break;
        }

        return nextState;
    }

    /**
     * Requests a selection from the user.
     * 
     * @return
     *        The user's selection
     */
    private UserSelection requestUserSelection() {
        UserSelection userSelection = null;

        System.out.println();
        System.out.println("[1] Play a game of Wordle");
        System.out.println("[2] View player statistics");
        System.out.println("[3] Log off");
        System.out.println("[4] Delete user");

        boolean read = true;
        while (read) {
            System.out.print("Selection: ");

            if (!_reader.hasNextLine()) {
                /* user likely quit the program with Ctrl-C */
                return null;
            }

            try {
                /* UserSelection starts at 0, so subtract 1 from user input */
                userSelection = UserSelection.fromInt(_reader.nextInt() - 1);
                if (userSelection != null) {
                    /* valid selection, stop the read loop */
                    read = false;
                } else {
                    System.out.println("Error: invalid selection");
                }
            } catch (InputMismatchException e) {
                System.out.println("Error: selection must be an integer");
            }

            /* handle remaining newline after calling reader.nextInt() */
            _reader.nextLine();
        }
        System.out.println();

        return userSelection;
    }
}
