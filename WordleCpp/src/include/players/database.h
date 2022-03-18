#pragma once

#include <functional>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

namespace players {

/**
 * Stores information about an entry in a database.
 * 
 * Each database entry contains two portions: the name
 * of the field, and the value of the data.
 */
template <typename T>
class DatabaseEntry {
public:
    std::string const name;
    T const value;

    static constexpr char *DELIM = ": ";

    /**
     * Creates a new database entry with the given name and value.
     * 
     * @param name
     *        The name of the field
     * @param value
     *        The value stored in the field
     */
    DatabaseEntry(std::string name, T value) : name{name}, value{value} {}

    /**
     * Creates a simple database entry from a line of text.
     *
     * The text will be split between field name and data
     * on the ": " delimiter. If the delimiter is not found,
     * then this function returns an error.
     * 
     * @param line
     *        The line to parse
     * @param stringToT
     *        A function to convert a string ref to the target type
     */
    static std::optional<DatabaseEntry<T>> FromLine(std::string const &line, std::function<T(std::string const &)> const &stringToT)
    {
        size_t delimIdx = line.find(DELIM);
        if (delimIdx == std::string::npos) {
            return std::nullopt;
        } else {
            return std::optional{DatabaseEntry{line.substr(0, delimIdx), stringToT(line.substr(delimIdx + 2, line.length()))}};
        }
    }

    static std::optional<DatabaseEntry<std::vector<T>>> FromVector(std::string const &line, std::function<T(std::string const &)> const &stringToT)
    {
        std::optional<DatabaseEntry<std::string const &>> parsedRow = DatabaseEntry<std::string const &>::FromLine(line, [](auto const &s) { return s; });
        if (!parsedRow.has_value()) {
            return std::nullopt;
        }

        std::vector<T> vec;
        for (int i = 0, j = 0; (j = parsedRow->value.find(",", i)) != std::string::npos; i = j + 1) {
            vec.push_back(stringToT(line.substr(i, j)));
        }

        return std::optional{
                DatabaseEntry<std::vector<T>>{std::move(parsedRow->name), std::move(vec)}
            };
    }

    static std::optional<DatabaseEntry<std::unordered_set<T>>> FromHashset(std::string const &line, std::function<T(std::string const &)> const &stringToT)
    {
        std::optional<DatabaseEntry<std::string const &>> parsedRow = DatabaseEntry<std::string const &>::FromLine(line, [](auto const &s) { return s; });
        if (!parsedRow.has_value()) {
            return std::nullopt;
        }

        std::unordered_set<T> set;
        for (int i = 0, j = 0; (j = parsedRow->value.find(",", i)) != std::string::npos; i = j + 1) {
            set.insert(stringToT(line.substr(i, j)));
        }

        return std::optional{
                DatabaseEntry<std::unordered_set<T>>{std::move(parsedRow->name), std::move(set)}
            };
    }
};

}
