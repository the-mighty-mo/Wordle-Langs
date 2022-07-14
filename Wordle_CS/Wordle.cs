namespace Wordle_CS;

/// <summary>
/// Possible guess results for a letter in a game of Wordle
/// </summary>
public enum WordleGuess
{
    /// <summary>
    /// Green ("G"), the letter is in the word at that position
    /// </summary>
    Correct,
    /// <summary>
    /// Yellow ("Y"), the letter is in the word, but not at that position
    /// </summary>
    Present,
    /// <summary>
    /// Gray ("X"), there are no more instances of the letter in the word
    /// </summary>
    Incorrect,
}

/// <summary>
/// Extension methods for the WordleGuess enum
/// </summary>
public static class WordleGuessExtensions
{
    /// <summary>
    /// Returns the string representation of a Wordle guess.
    /// </summary>
    /// <param name="guess">The Wordle guess</param>
    public static string ToStr(this WordleGuess guess) => guess switch
    {
        WordleGuess.Correct => "G",
        WordleGuess.Present => "Y",
        WordleGuess.Incorrect => "X",
        _ => throw new ArgumentOutOfRangeException($"Value {guess} is not a valid WordleGuess"),
    };
}

/// <summary>
/// Manages information about an answer to a game of Wordle.
/// <para/>
/// A game of Wordle has a target word. The guessing algorithm
/// uses preprocessing so it can run in linear time. This results
/// in an array containing the counts of each letter.
/// </summary>
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
    /// <para/>
    /// This function returns an array containing the correctness
    /// of each letter of the guess.
    /// <para/>
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

    /// <summary>
    /// Contains all the possible messages
    /// for a won game of Wordle.
    /// <para/>
    /// If the user guessed the word in n
    /// guesses(starting at 1), then
    /// WIN_MESSAGES[n - 1] is the message
    /// that should be displayed.
    /// </summary>
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
