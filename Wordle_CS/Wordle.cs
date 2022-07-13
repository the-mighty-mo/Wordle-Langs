namespace Wordle_CS;

public enum WordleGuess
{
    /// Green ("G"), the letter is in the word at that position
    Correct,
    /// Yellow ("Y"), the letter is in the word, but not at that position
    Present,
    /// Gray ("X"), there are no more instances of the letter in the word
    Incorrect,
}

public static class WordleGuessExtensions
{
    /// <summary>
    /// Returns a string representation of a Wordle guess.
    /// </summary>
    public static string ToStr(this WordleGuess guess) => guess switch
    {
        WordleGuess.Correct => "G",
        WordleGuess.Present => "Y",
        WordleGuess.Incorrect => "X",
        _ => throw new ArgumentOutOfRangeException($"Value {guess} is not a valid WordleGuess"),
    };
}

public class WordleAnswer
{
    public string Word { get; init; }
    private readonly int[] letterCounts = new int[26];

    /// <summary>
    /// Creates a new Wordle answer.
    /// </summary>
    /// <param name="word">The answer word</param>
    public WordleAnswer(string word)
    {
        Word = word;
        foreach (var c in word)
        {
            ++letterCounts[c - 'A'];
        }
    }

    /// <summary>
    /// Calculates the correctness of a guess.
    /// <br/>
    /// This function returns an array containing the correctness
    /// of each letter of the guess.
    /// <br/>
    /// It is important to note that the sum of Correct and Present
    /// instances for a given letter cannot exceed the total number
    /// of instances of the letter in the answer. Additionally,
    /// Correct always takes priority over Present.
    /// </summary>
    /// <param name="guess">The guess</param>
    public WordleGuess[] CheckGuess(string guess)
    {
        var colors = Enumerable.Repeat(WordleGuess.Incorrect, 5).ToArray();
        var letterCounts = (int[])this.letterCounts.Clone();

        /* first check for green letters */
        foreach (var ((a, g), i) in Word.Zip(guess).Select((cs, i) => (cs, i)))
        {
            if (a == g)
            {
                --letterCounts[g - 'A'];
                colors[i] = WordleGuess.Correct;
            }
        }

        /* then check for yellow letters */
        foreach (var (g, i) in guess.Select((c, i) => (c, i)))
        {
            if (colors[i] == WordleGuess.Incorrect)
            {
                /* letter has not yet been checked */
                if (letterCounts[g - 'A'] > 0)
                {
                    /* letter in word but not this position */
                    colors[i] = WordleGuess.Present;
                    --letterCounts[g - 'A'];
                }
            }
        }

        return colors;
    }

    public static readonly string[] WORDLE_WIN_MESSAGES = new string[]
    {
        "Genius",
        "Magnificent",
        "Impressive",
        "Splendid",
        "Great",
        "Phew",
    };
}
