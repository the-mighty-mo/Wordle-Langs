using Wordle_CS;
using Wordle_CS.ConsoleApp;

if (args.Length != 1)
{
    Console.WriteLine("Wordle");
    Console.WriteLine("Author: Benjamin Hall");
    Console.WriteLine("Usage: ./Wordle_CS [dictionary file name]");
    return;
}

string dictFileName = args[0];

/* make sure none of the inputs are empty */
if (dictFileName.Length == 0)
{
    Console.WriteLine("Error: no dictionary file specified");
    return;
}

string dictFileContents;
using (var dictFile = File.OpenRead(dictFileName))
{
    try
    {
        dictFileContents = ReadFile(dictFile);
    }
    catch (Exception)
    {
        Console.WriteLine("Error: could not read dictionary file");
        return;
    }
}

string usernamesFileContents;
using (var usernamesFile = File.Open("users.txt", FileMode.OpenOrCreate))
{
    try
    {
        usernamesFileContents = ReadFile(usernamesFile);
    }
    catch (Exception)
    {
        Console.WriteLine("Error: could not read user database");
        return;
    }
}

HashSet<string> dictionary = [.. dictFileContents
    .Lines()
    .Where(s => s.Length == 5)
    .Select(s => s.ToUpper())
];

SortedSet<string> usernames = [.. usernamesFileContents.Lines()];

ConsoleApp.Run(dictionary, usernames);

/// <summary>
/// Loads data from a file into a string.
/// </summary>
/// <param name="file">The file to read</param>
static string ReadFile(FileStream file)
{
    using BufferedStream buffer = new(file);
    using StreamReader reader = new(buffer);
    return reader.ReadToEnd();
}
