using System.Text;

namespace Wordle_CS.Players;

public class PlayerInfo
{
    public string Username { get; init; }
    private readonly HashSet<string> wordsPlayed;
    private readonly ulong[] numGuesses;
    private ulong maxWinStreak;
    private ulong curWinStreak;

    private readonly Random rnd = new();

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

    public PlayerInfo(string username) : this(username, new(), new ulong[6], 0, 0)
    { }

    public PlayerInfo(string username, HashSet<string> wordsPlayed, ulong[] numGuesses, ulong maxWinStreak, ulong curWinStreak)
    {
        Username = username;
        this.wordsPlayed = wordsPlayed;
        this.numGuesses = numGuesses;
        this.maxWinStreak = maxWinStreak;
        this.curWinStreak = curWinStreak;
    }

    public string GetRandomWord(HashSet<string> dictionary)
    {
        var unplayedWordsCnt = dictionary.Count - wordsPlayed.Count;
        var randomWordIdx = rnd.Next(0, unplayedWordsCnt);
        return dictionary.Skip(randomWordIdx).First();
    }

    public void AddWonWord(string word, ulong numGuesses)
    {
        wordsPlayed.Add(word);
        ++this.numGuesses[numGuesses - 1];
        ++curWinStreak;
        maxWinStreak = Math.Max(maxWinStreak, curWinStreak);
    }

    public void AddLostWord(string word)
    {
        wordsPlayed.Add(word);
        curWinStreak = 0;
    }

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

    public void WriteToFile(string filename)
    {
        using var file = File.Create(filename);
        using BufferedStream buffer = new(file);
        using StreamWriter writer = new(buffer);
        writer.Write(ToString());
    }

    public static PlayerInfo? FromFile(string filename)
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
