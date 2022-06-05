#pragma once

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace players {

/**
 * Stores information about an entry in a database.
 * 
 * Each database entry contains two portions: the name
 * of the field, and the value of the data.
 */
template <typename T>
class BaseDatabaseEntry {
private:
    static constexpr char const *DELIM = ": ";

public:
    std::string const name;
    T const value;

    /**
     * Creates a new database entry with the given name and value.
     * 
     * @param name
     *        The name of the field
     * @param value
     *        The value stored in the field
     */
    BaseDatabaseEntry(std::string name, T value) :
        name{std::move(name)},
        value{std::move(value)}
    {}

    /**
     * Creates a simple database entry from a line of text.
     *
     * The text will be split between field name and data
     * on the ": " delimiter. If the delimiter is not found,
     * then this function returns a nullopt.
     * 
     * @param line
     *        The line to parse
     * @param stringToT
     *        A function to convert a string ref to the target type
     */
    static std::optional<BaseDatabaseEntry<T>> FromLine(std::string_view line, std::function<T(std::string_view)> const &stringToT)
    {
        size_t delimIdx = line.find(DELIM);
        if (delimIdx == std::string_view::npos) {
            return std::nullopt;
        } else {
            return std::optional{BaseDatabaseEntry<T>{std::string{line.substr(0, delimIdx)}, stringToT(line.substr(delimIdx + 2, line.length()))}};
        }
    }
};

template <typename T>
class DatabaseEntry : public BaseDatabaseEntry<T> {};

template class DatabaseEntry<std::string>;

template <typename T>
class DatabaseEntry<std::vector<T>> : public BaseDatabaseEntry<std::vector<T>> {
public:
    /**
     * Creates a database entry from a line of text where
     * the data field is a vector of elements.
     *
     * The text will be split between field name and data
     * on the ": " delimiter. If the delimiter is not found,
     * then this function returns a nullopt. From there,
     * elements will be separated by the "," delimiter and
     * added to a vector.
     * 
     * @param line
     *        The line to parse
     * @param stringToT
     *        A function to convert a string ref to the target type
     */
    static std::optional<BaseDatabaseEntry<std::vector<T>>> FromVector(std::string_view line, std::function<T(std::string_view)> const &stringToT)
    {
        auto parsedRow = DatabaseEntry<std::string_view>::FromLine(line, [](auto s) { return s; });
        if (!parsedRow.has_value()) {
            return std::nullopt;
        }

        std::vector<T> vec;
        size_t start, end;
        for (start = 0, end = 0; (end = parsedRow->value.find(",", start)) != std::string_view::npos; start = end + 1) {
            vec.push_back(stringToT(parsedRow->value.substr(start, end)));
        }
        vec.push_back(stringToT(parsedRow->value.substr(start, parsedRow->value.length())));

        return std::optional{
                BaseDatabaseEntry<std::vector<T>>{std::move(parsedRow->name), std::move(vec)}
            };
    }
};

template <template <typename...> class S, typename T, typename... Us>
class DatabaseEntry<S<T, Us...>> : public BaseDatabaseEntry<S<T, Us...>> {
public:
    /**
     * Creates a database entry from a line of text where
     * the data field is a set of elements.
     *
     * The text will be split between field name and data
     * on the ": " delimiter. If the delimiter is not found,
     * then this function returns a nullopt. From there,
     * elements will be separated by the "," delimiter and
     * added to a set.
     * 
     * @param line
     *        The line to parse
     * @param stringToT
     *        A function to convert a string ref to the target type
     */
    static std::optional<BaseDatabaseEntry<S<T, Us...>>> FromSet(std::string_view line, std::function<T(std::string_view)> const &stringToT)
    {
        auto parsedRow = DatabaseEntry<std::string_view>::FromLine(line, [](auto s) { return s; });
        if (!parsedRow.has_value()) {
            return std::nullopt;
        }

        S<T> set;
        size_t start, end;
        for (start = 0, end = 0; (end = parsedRow->value.find(",", start)) != std::string_view::npos; start = end + 1) {
            set.insert(stringToT(parsedRow->value.substr(start, end)));
        }
        set.insert(stringToT(parsedRow->value.substr(start, parsedRow->value.length())));

        return std::optional{
                BaseDatabaseEntry<S<T, Us...>>{std::move(parsedRow->name), std::move(set)}
            };
    }
};

}
