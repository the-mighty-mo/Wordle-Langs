using System.Text;

namespace Wordle_CS.Players;

/// <summary>
/// Contains information about a Wordle player.
/// <para/>
/// A player has a:
/// <list type="bullet">
///     <item>username</item>
///     <item>list of words played</item>
///     <item>guess distribution</item>
///     <item>maximum win streak</item>
///     <item>current win streak</item>
/// </list>
/// </summary>
public class PlayerInfo
{
    /// <summary>
    /// This player's username.
    /// </summary>
    public string Username { get; init; }
    private readonly HashSet<string> wordsPlayed;
    private readonly ulong[] numGuesses;
    private ulong maxWinStreak;
    private ulong curWinStreak;

    private readonly Random rnd = new();

    /// <summary>
    /// Initializes data for a new player.
    /// </summary>
    /// <param name="username">The username of the player</param>
    public PlayerInfo(string username) : this(username, new(), new ulong[6], 0, 0)
    { }

    /// <summary>
    /// Loads data for an existing player.
    /// </summary>
    /// <param name="username">The username of the player</param>
    /// <param name="wordsPlayed">A set of words the player has already played</param>
    /// <param name="numGuesses">
    ///     An array containing the number of times the
    ///     player has guessed a word in (i + 1) attempts,
    ///     where i is the index of the array from [0, 6)
    /// </param>
    /// <param name="maxWinStreak">The maximum winning streak</param>
    /// <param name="curWinStreak">The current winning streak</param>
    public PlayerInfo(string username, HashSet<string> wordsPlayed, ulong[] numGuesses, ulong maxWinStreak, ulong curWinStreak)
    {
        Username = username;
        this.wordsPlayed = wordsPlayed;
        this.numGuesses = numGuesses;
        this.maxWinStreak = maxWinStreak;
        this.curWinStreak = curWinStreak;
    }

    /// <summary>
    /// Gets a random word the player has not yet played.
    /// </summary>
    /// <param name="dictionary">The dictionary from which to find a word</param>
    public string GetRandomWord(HashSet<string> dictionary)
    {
        var unplayedWordsCnt = dictionary.Count - wordsPlayed.Count;
        var randomWordIdx = rnd.Next(0, unplayedWordsCnt);
        return dictionary.Where(w => !wordsPlayed.Contains(w)).Skip(randomWordIdx).First();
    }

    /// <summary>
    /// Adds a word the player has successfully guessed to their database.
    /// <para/>
    /// This function increments the current win streak, adds
    /// the guess to the list of words played and to the guess
    /// distribution, and updates the max win streak if appropriate.
    /// </summary>
    /// <param name="word">The word the player successfully guessed</param>
    /// <param name="numGuesses">The number of guesses the player made</param>
    public void AddWonWord(string word, ulong numGuesses)
    {
        wordsPlayed.Add(word);
        ++this.numGuesses[numGuesses - 1];
        ++curWinStreak;
        maxWinStreak = Math.Max(maxWinStreak, curWinStreak);
    }

    /// <summary>
    /// Adds a word the player has failed to guess to their database.
    /// <para/>
    /// This function resets the current win streak to 0 and adds
    /// the guess to the list of words played. The number of guesses
    /// is not added to the player's guess distribution.
    /// </summary>
    /// <param name="word">The word the player failed to guess</param>
    public void AddLostWord(string word)
    {
        wordsPlayed.Add(word);
        curWinStreak = 0;
    }

    /// <summary>
    /// Returns a string representation of this player's information.
    /// </summary>
    /// <returns></returns>
    public override string ToString()
    {
        StringBuilder sb = new();
        sb.AppendLine($"Username: {Username}");
        sb.AppendLine($"Words Played: {string.Join(",", wordsPlayed)}");
        sb.AppendLine($"Number of Guesses: {string.Join(",", numGuesses)}");
        sb.AppendLine($"Maximum Win Streak: {maxWinStreak}");
        sb.AppendLine($"Current Win Streak: {curWinStreak}");
        return sb.ToString();
    }

    /// <summary>
    /// Returns a string with formatted player statistics.
    /// <para/>
    /// Player statistics consist of:
    /// <list type="bullet">
    ///     <item>Number of words played</item>
    ///     <item>Win rate</item>
    ///     <item>Current and max win streak</item>
    ///     <item>Guess distribution</item>
    /// </list>
    /// </summary>
    public string GetStats()
    {
        StringBuilder sb = new();
        sb.AppendLine($"Number of Words Played: {wordsPlayed.Count}");

        var winRate = wordsPlayed.Count == 0 ? 0 : (uint)Math.Round(100.0 * numGuesses.Aggregate((sum, x) => sum + x) / wordsPlayed.Count);
        sb.AppendLine($"Win Rate: {winRate}%");

        sb.AppendLine($"Current Win Streak: {curWinStreak}");
        sb.AppendLine($"Maximum Win Streak: {maxWinStreak}");

        sb.AppendLine("Guess Distribution:");
        var maxNumGuesses = numGuesses.Max();
        var barFactor = maxNumGuesses == 0 ? 0 : 12.0 / maxNumGuesses;

        foreach (var (numGuess, i) in numGuesses.Select((numGuess, i) => (numGuess, i)))
        {
            var numBars = (int)Math.Round(barFactor * numGuess);
            var bars = string.Concat(Enumerable.Repeat("=", numBars));
            sb.AppendLine($"{i + 1}: {bars} {numGuess}");
        }

        return sb.ToString().Trim();
    }

    /// <summary>
    /// Writes a player's data to a file.
    /// </summary>
    /// <param name="filename">The file to which to write</param>
    public void WriteToFile(string filename)
    {
        using var file = File.Create(filename);
        using BufferedStream buffer = new(file);
        using StreamWriter writer = new(buffer);
        writer.Write(ToString());
    }

    /// <summary>
    /// Reads a player's information from a file.
    /// <para/>
    /// The function throws an exception on parse errors.
    /// </summary>
    /// <param name="filename">The file from which to read</param>
    /// <returns>
    ///     A player's information if the operation
    ///     succeeded, or a nullopt
    /// </returns>
    public static PlayerInfo? CreateFromFile(string filename)
    {
        string fileContents;
        try
        {
            using var file = File.OpenRead(filename);
            using BufferedStream buffer = new(file);
            using StreamReader reader = new(buffer);
            fileContents = reader.ReadToEnd();
        }
        catch (Exception)
        {
            return null;
        }

        return FromStr(fileContents, new IOException($"Error: corrupt player database file: {filename}"));
    }

    /// <summary>
    /// Reads a player's info from a string.
    /// <para/>
    /// The function throws an exception on parse errors.
    /// </summary>
    /// <param name="playerData">The string to parse</param>
    /// <param name="badDataErr">The exception to throw on parse errors</param>
    /// <returns>
    ///     A player's information if the operation
    ///     succeeded, or a nullopt
    /// </returns>
    private static PlayerInfo FromStr(string playerData, IOException badDataErr)
    {
        var linesInFile = playerData.Lines().ToList();
        if (linesInFile.Count != 5)
        {
            throw badDataErr;
        }

        var username = DatabaseEntry<string>.FromLine(linesInFile[0], s => s) ?? throw badDataErr;
        var wordsPlayed = DatabaseEntry<string>.FromCollection<HashSet<string>>(linesInFile[1], s => s) ?? throw badDataErr;
        var numGuessesList = DatabaseEntry<ulong>.FromCollection<List<ulong>>(linesInFile[2], ulong.Parse) ?? throw badDataErr;
        var maxWinStreak = DatabaseEntry<ulong>.FromLine(linesInFile[3], ulong.Parse) ?? throw badDataErr;
        var curWinStreak = DatabaseEntry<ulong>.FromLine(linesInFile[4], ulong.Parse) ?? throw badDataErr;

        var numGuesses = new ulong[6];
        foreach (var (numGuess, i) in numGuessesList.Value.Take(numGuesses.Length).Select((n, i) => (n, i)))
        {
            numGuesses[i] = numGuess;
        }

        return new PlayerInfo(username.Value, wordsPlayed.Value, numGuesses, maxWinStreak.Value, curWinStreak.Value);
    }
}
