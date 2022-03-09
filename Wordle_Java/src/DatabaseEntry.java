/*
 * Wordle program
 * CS 203 Project 3
 * Benjamin Hall
 * 2022.03.06
 */

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
}
