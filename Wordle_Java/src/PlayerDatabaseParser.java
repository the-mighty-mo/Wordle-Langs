/*
 * Wordle program
 * CS 203 Project 3
 * Benjamin Hall
 * 2022.03.06
 */

import java.util.*;
import java.util.function.*;

/**
 * A class to parse data from a player database.
 * 
 * @author Benjamin Hall
 */
public class PlayerDatabaseParser {
    /* all fields are "Field name: Value(s)" */
    private static final String delimiter = ": ";

    /**
     * Parses a line from the database file.
     * 
     * @param <T>
     *        The type of data
     * @param line
     *        The line to parse
     * @param stringToT
     *        A function to convert the data portion of
     *        the parsed line to the desired data type
     * @return
     *        A pair containing the field parsed and the
     *        data stored in the field
     */
    public static <T> DatabaseEntry<T> parseLine(String line, Function<String, T> stringToT) {
        int delimIdx = line.indexOf(delimiter);
        if (delimIdx < 0) {
            return null;
        }

        String key = line.substring(0, delimIdx).trim();
        String value = line.substring(delimIdx + delimiter.length(), line.length())
            .trim();

        return new DatabaseEntry<T>(key, stringToT.apply(value));
    }

    /**
     * Parses a line representing a list of items from the database file.
     * 
     * @param <T>
     *        The type of data
     * @param <L>
     *        The type of the list
     * @param line
     *        The line to parse
     * @param stringToT
     *        A function to convert the data portion of
     *        the parsed line to the desired data type
     * @param listFactory
     *        A function to create a new instance of the list
     * @return
     *        A pair containing the field parsed and the
     *        list of data stored in the field
     */
    public static <T, L extends List<T>> DatabaseEntry<L> parseList(String line, Function<String, T> stringToT, Supplier<L> listFactory) {
        DatabaseEntry<String> parsedRow = parseLine(line, s -> s);
        if (parsedRow == null) {
            return null;
        }

        L items = listFactory.get();
        if (parsedRow.value.length() != 0) {
            String[] itemStrs = parsedRow.value.split(",");
            for (String itemStr : itemStrs) {
                items.add(stringToT.apply(itemStr));
            }
        }

        return new DatabaseEntry<L>(parsedRow.name, items);
    }

    /**
     * Parses a line representing a set of unique items from the database file.
     * 
     * @param <T>
     *        The type of data
     * @param <S>
     *        The type of the set
     * @param line
     *        The line to parse
     * @param stringToT
     *        A function to convert the data portion of
     *        the parsed line to the desired data type
     * @param setFactory
     *        A function to create a new instance of the set
     * @return
     *        A pair containing the field parsed and the
     *        set of data stored in the field
     */
    public static <T, S extends Set<T>> DatabaseEntry<S> parseSet(String line, Function<String, T> stringToT, Supplier<S> setFactory) {
        DatabaseEntry<String> parsedRow = parseLine(line, s -> s);
        if (parsedRow == null) {
            return null;
        }

        S items = setFactory.get();
        if (parsedRow.value.length() != 0) {
            String[] itemStrs = parsedRow.value.split(",");
            for (String itemStr : itemStrs) {
                items.add(stringToT.apply(itemStr));
            }
        }

        return new DatabaseEntry<S>(parsedRow.name, items);
    }
}
