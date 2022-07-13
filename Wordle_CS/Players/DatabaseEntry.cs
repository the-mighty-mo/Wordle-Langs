namespace Wordle_CS.Players;

internal class DatabaseEntry<T>
{
    public readonly string Name;
    public readonly T Value;

    public const string DELIM = ": ";

    public DatabaseEntry(string name, T value)
    {
        Name = name;
        Value = value;
    }

    public delegate T stringToT(string str);

    public static DatabaseEntry<T>? FromLine(string line, stringToT strToT)
    {
        var splitStr = line.Split(DELIM, 2);
        return splitStr.Length != 2 ? null : new(splitStr[0], strToT(splitStr[1]));
    }

    public static DatabaseEntry<C>? FromCollection<C>(string line, stringToT strToT) where C : IEnumerable<T>
    {
        var parsedRow = DatabaseEntry<string>.FromLine(line, s => s);
        if (parsedRow is null)
        {
            return null;
        }

        C? items = (C?)Activator.CreateInstance(typeof(C), new object[] { parsedRow.Value.Split(',').Select(s => strToT(s)) });
        return items is null ? null : new DatabaseEntry<C>(parsedRow.Name, items);
    }
}
