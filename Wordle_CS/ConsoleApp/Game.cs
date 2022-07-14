namespace Wordle_CS.ConsoleApp;

/// <summary>
/// Provides methods to run a game of Wordle in a console application.
/// </summary>
public static class Game
{
    /// <summary>
    /// Runs a game of Wordle.
    /// <para/>
    /// This function manages all user input and output using
    /// stdin and stdout, respectively, as well as all six
    /// guesses.The function ends after the user has guessed
    /// the answer or used all six guesses, whichever is first.
    /// <para/>
    /// At the start of the game, a message is printed with
    /// instructions for the player.
    /// </summary>
    /// <param name="answer">The answer to the game of Wordle</param>
    /// <param name="player">The player playing the game</param>
    /// <param name="dictionary">A dictionary of valid Wordle guesses</param>
    public static void Run(WordleAnswer answer, Players.PlayerInfo player, HashSet<string> dictionary)
    {
        Console.WriteLine("Guess the 5-letter word in 6 or fewer guesses.");
        Console.WriteLine("After each guess, each letter will be given a color:");
        Console.WriteLine("G = Green:\tletter is in that position in the word");
        Console.WriteLine("Y = Yellow:\tletter is in the word, but not that position");
        Console.WriteLine("X = Black:\tthere are no more instances of the letter in the word");

        var wonGame = new Func<long?>(() =>
        {
            for (int i = 1; i <= 6; ++i)
            {
                string guess;
                while (true)
                {
                    Console.Write($"[{i}] ");

                    string? guessStr = Console.ReadLine();
                    if (guessStr is null)
                    {
                        return -1;
                    }
                    else
                    {
                        guess = guessStr;
                    }

                    guess = guess.Trim().ToUpper();
                    if (guess.Length != 5)
                    {
                        Console.WriteLine("Error: guess must be 5 letters");
                    }
                    else if (!dictionary.Contains(guess))
                    {
                        Console.WriteLine("Error: guess must be a word in the dictionary");
                    }
                    else
                    {
                        break;
                    }
                }

                var colors = answer.CheckGuess(guess);
                Console.Write("    ");
                foreach (var color in colors)
                {
                    Console.Write(color.ToStr());
                }
                Console.WriteLine();

                bool won = colors.All(c => c == WordleGuess.Correct);
                if (won)
                {
                    return i;
                } 
            }

            return null;
        })();

        if (wonGame is not null and -1)
        {
            return;
        }
        else if (wonGame is not null)
        {
            player.AddWonWord(answer.Word, (ulong)wonGame);
            Console.Write($"{WordleAnswer.WORDLE_WIN_MESSAGES[(ulong)wonGame - 1]} ");
        }
        else
        {
            player.AddLostWord(answer.Word);
            Console.Write("Too bad! ");
        }
        Console.WriteLine($"The word was: {answer.Word}");
        Console.WriteLine();
    }
}
