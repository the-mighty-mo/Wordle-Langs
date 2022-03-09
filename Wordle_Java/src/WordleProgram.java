/*
 * Wordle program
 * CS 203 Project 3
 * Benjamin Hall
 * 2022.02.23
 */

import java.io.*;
import java.util.*;

/**
 * Main class for running Wordle.
 * 
 * @author Benjamin Hall
 */
public class WordleProgram {
    private static final String usernamesFile = "users.txt";

    /**
     * Runs the Wordle program.
     * 
     * @param args
     *        - The name of the dictionary file
     */
    public static void main(String[] args) {
        if (args.length != 1) {
            /* invalid number of arguments, print a help message */
            System.out.println("Wordle");
            System.out.println("Author: Benjamin Hall");
            System.out.println("Usage: WordleProgram [dictionary file name]");
            return;
        }

        String dictFileName = args[0];

        /* make sure none of the inputs are empty */
        if (dictFileName.length() == 0) {
            System.err.println("Error: no dictionary file specified");
            return;
        }

        File dictFile = new File(dictFileName);
        Set<String> dictionary = new HashSet<String>();
        try {
            readDictionaryFile(dictionary, dictFile);
        } catch (FileNotFoundException e) {
            System.err.println("Error: could not read dictionary file");
            return;
        }

        TreeSet<String> usernames;
        try {
            usernames = loadUsernames(usernamesFile);
        } catch (IOException e) {
            System.err.println("Error: could not read user database");
            return;
        }

        runStateMachine(dictionary, usernames);
    }

    /**
     * Runs the main program state machine.
     * 
     * @param dictionary
     *        The dictionary of five-letter words
     * @param usernames
     *        A set of existing usernames
     */
    private static void runStateMachine(Set<String> dictionary, TreeSet<String> usernames) {
        Scanner reader = new Scanner(System.in);
        WordleMainMenu mainMenu = new WordleMainMenu(reader);

        ProgramState state = ProgramState.LogIn;
        boolean runProgram = true;

        PlayerInfo currentPlayer = null;
        while (runProgram) {
            switch (state) {
            case LogIn:
                currentPlayer = mainMenu.requestUserLogin(usernames);
                if (currentPlayer == null) {
                    /* user requested to exit, or there was an error */
                    state = ProgramState.Exit;
                } else {
                    try {
                        saveUsernames(usernamesFile, usernames);
                        /* user has logged in, continue to the main menu */
                        state = ProgramState.MainMenu;
                    } catch (IOException e) {
                        System.err.println("Error: could not write to the user database");
                        state = ProgramState.Exit;
                    }
                }
                break;

            case MainMenu:
                state = mainMenu.run(currentPlayer, dictionary);
                break;

            case DeleteUser:
                /* remove the current player from the databse */
                usernames.remove(currentPlayer.getUsername());
                try {
                    /* delete the user's data file */
                    new File(currentPlayer.getUsername() + ".txt").delete();
                    /* save the username database */
                    saveUsernames(usernamesFile, usernames);
                    /* user has logged out, go to the login screen */
                    state = ProgramState.LogIn;
                } catch (IOException e) {
                    System.err.println("Error: could not write to the user database");
                    state = ProgramState.Exit;
                }
                break;

            case Exit:
                /* end the main loop */
                runProgram = false;
                break;
            }
        }

        reader.close();
    }

    /**
     * Reads a dictionary file to a set of words. All words
     * stored will be five letter words for use in Wordle.
     * 
     * @param dictionary
     *        The set in which to store the words in the dictionary
     * @param dictFile
     *        The dictionary file
     * 
     * @throws FileNotFoundException
     *        if the dictionary file does not exist
     */
    private static void readDictionaryFile(Set<String> dictionary, File dictFile)
        throws FileNotFoundException
    {
        /* open the dictionary file for reading */
        Scanner reader = new Scanner(dictFile);

        /* add all 5-letter words in the dictionary file to the Set */
        while (reader.hasNextLine()) {
            /* words must be upper case for a game of Wordle */
            String word = reader.nextLine().toUpperCase();
            if (word.length() == 5) {
                dictionary.add(word);
            }
        }
        /* we're done reading from the dictionary file */
        reader.close();
    }

    /**
     * Loads an alphabetically sorted set of usernames from a file.
     * 
     * @param filename
     *        The name of the file
     * @return
     *        A set of usernames, sorted using a red-black tree
     * 
     * @throws IOException
     *        if the file could not be read
     */
    private static TreeSet<String> loadUsernames(String filename) throws IOException {
        File usersDb = new File(filename);
        /* create the file if it does not yet exist */
        usersDb.createNewFile();

        TreeSet<String> users = new TreeSet<String>();
        {
            Scanner usersReader = new Scanner(usersDb);
            while (usersReader.hasNextLine()) {
                users.add(usersReader.nextLine());
            }
            usersReader.close();
        }
        return users;
    }

    /**
     * Saves an alphabetically sorted set of usernames to a file.
     * 
     * @param filename
     *        The name of the file
     * @param usernames
     *        A set of sorted usernames
     * 
     * @throws IOException
     *        if the data could not be written to the file
     */
    private static void saveUsernames(String filename, TreeSet<String> usernames)
        throws IOException
    {
        File usersDb = new File(filename);
        /* create the file if it does not yet exist */
        usersDb.createNewFile();

        FileWriter usersWriter = new FileWriter(usersDb);
        usersWriter.append(String.join("\n", usernames));
        usersWriter.close();
    }
}
