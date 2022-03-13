/*
 * Wordle program
 * Author: Benjamin Hall
 * Created: 2022.03.06
 */

import java.util.*;
import java.util.function.*;

/**
 * A class to store information about
 * an entry in a database.
 * 
 * @author Benjamin Hall
 */
public class DatabaseEntry<T> {
    public final String name;
    public final T value;

    /**
     * Creates a new object containing
     * information about a database entry.
     * 
     * @param name
     *        The name of the data field
     * @param value
     *        The data stored in the field
     */
    public DatabaseEntry(String name, T value) {
        this.name = name;
        this.value = value;
    }

    /* all fields are "Field name: Value(s)" */
    private static final String delimiter = ": ";

    /**
     * Creates a simple database entry from a line of text.
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
     * Creates a database entry from a line of text where
     * the data field is a collection of elements.
     * 
     * @param <T>
     *        The type of data
     * @param <C>
     *        The type of the collection
     * @param line
     *        The line to parse
     * @param stringToT
     *        A function to convert the data portion of
     *        the parsed line to the desired data type
     * @param collFactory
     *        A function to create a new instance of the collection
     * @return
     *        A pair containing the field parsed and the
     *        set of data stored in the field
     */
    public static <T, C extends Collection<T>> DatabaseEntry<C> parseCollection(
            String line, Function<String, T> stringToT, Supplier<C> collFactory) {
        DatabaseEntry<String> parsedRow = parseLine(line, s -> s);
        if (parsedRow == null) {
            return null;
        }

        C items = collFactory.get();
        if (parsedRow.value.length() != 0) {
            String[] itemStrs = parsedRow.value.split(",");
            for (String itemStr : itemStrs) {
                items.add(stringToT.apply(itemStr));
            }
        }

        return new DatabaseEntry<C>(parsedRow.name, items);
    }
}
