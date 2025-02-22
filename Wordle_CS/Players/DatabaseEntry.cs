namespace Wordle_CS.Players;

/// <summary>
/// Stores information about an entry in a database.
/// <para/>
/// Each database entry contains two portions: the name
/// of the field, and the value of the data.
/// </summary>
/// <typeparam name="T">Type of Value</typeparam>
/// <param name="name">The name of the field</param>
/// <param name="value">The value stored in the field</param>
internal class DatabaseEntry<T>(string name, T value)
{
    public readonly string Name = name;
    public readonly T Value = value;

    /// Delimiter between the field name and data for database entries
    private const string DELIM = ": ";

    /// <summary>
    /// A function to convert from a string to a given type.
    /// </summary>
    /// <param name="str">The string to convert</param>
    /// <returns>The converted value</returns>
    public delegate T stringToT(string str);

    /// <summary>
    /// Creates a simple database entry from a line of text.
    /// <para/>
    /// The text will be split between field name and data
    /// on the ": " delimiter. If the delimiter is not found,
    /// then this function returns a nullopt.
    /// </summary>
    /// <param name="line">The line to parse</param>
    /// <param name="strToT">A function to convert a string to the target type</param>
    public static DatabaseEntry<T>? FromLine(string line, stringToT strToT)
    {
        var splitStr = line.Split(DELIM, 2);
        return splitStr.Length != 2 ? null : new(splitStr[0], strToT(splitStr[1]));
    }

    /// <summary>
    /// Creates a database entry from a line of text where
    /// the data field is a collection of elements.
    /// <para/>
    /// The text will be split between field name and data
    /// on the ": " delimiter. If the delimiter is not found,
    /// then this function returns a nullopt. From there,
    /// elements will be separated by the "," delimiter and
    /// added to a collection.
    /// </summary>
    /// <typeparam name="C">The collection type</typeparam>
    /// <param name="line">The line to parse</param>
    /// <param name="strToT">A function to convert a string to the target type</param>
    public static DatabaseEntry<C>? FromCollection<C>(string line, stringToT strToT) where C : IEnumerable<T>
    {
        var parsedRow = DatabaseEntry<string>.FromLine(line, s => s);
        if (parsedRow is null)
        {
            return null;
        }

        C? items = (C?)Activator.CreateInstance(typeof(C), [parsedRow.Value.Split(',').Select(s => strToT(s))]);
        return items is null ? null : new DatabaseEntry<C>(parsedRow.Name, items);
    }
}
