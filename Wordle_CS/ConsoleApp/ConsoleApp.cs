using Wordle_CS.Players;

namespace Wordle_CS.ConsoleApp;

/// <summary>
/// Provides methods to run a game of Wordle as a console application.
/// <para/>
/// To run a game of Wordle, the user must first login by
/// either selecting a username from an existing list of
/// usernames, or by creating a new one. From there, the
/// user can play a game of Wordle, view their statistics,
/// log out, or delete their account.
/// <para/>
/// User data and a list of usernames are stored to multiple
/// database files. The calling program must load the list
/// of usernames and a dictionary of valid five-letter words.
/// From there, the calling program can start the main state
/// machine of the program.
/// </summary>
public static class ConsoleApp
{
    /// <summary>
    /// Name of the usernames database file
    /// </summary>
    public const string USERNAMES_FILE = "users.txt";

    /// <summary>
    /// Runs the main state machine of the Wordle console program.
    /// <para/>
    /// This function manages the various states of the program
    /// after all necessary variables have been initialized,
    /// such as the dictionary and the set of existing usernames.
    /// </summary>
    /// <param name="dictionary">A dictionary of valid Wordle words</param>
    /// <param name="usernames">A set of existing usernames</param>
    public static void Run(HashSet<string> dictionary, SortedSet<string> usernames)
    {
        var state = ProgramState.LogIn;
        PlayerInfo? currentPlayer = null;

        while (true)
        {
            switch (state)
            {
                case ProgramState.LogIn:
                    currentPlayer = MainMenu.RequestUserLogin(usernames);
                    if (currentPlayer is null)
                    {
                        state = ProgramState.Exit;
                    }
                    else
                    {
                        try
                        {
                            SaveUsernames(usernames, USERNAMES_FILE);
                            state = ProgramState.MainMenu;
                        }
                        catch (Exception)
                        {
                            Console.WriteLine("Error: could not write to the user database");
                            state = ProgramState.Exit;
                        }
                    }
                    break;

                case ProgramState.MainMenu:
                    state = MainMenu.Run(currentPlayer!, dictionary);
                    break;

                case ProgramState.DeleteUser:
                    var username = currentPlayer!.Username;
                    usernames.Remove(username);
                    try
                    {
                        File.Delete($"{username}.txt");
                    }
                    catch (Exception) { }

                    try
                    {
                        SaveUsernames(usernames, USERNAMES_FILE);
                        state = ProgramState.LogIn;
                    }
                    catch (Exception)
                    {
                        Console.WriteLine("Error: could not write to the user database");
                        state = ProgramState.Exit;
                    }
                    break;

                case ProgramState.Exit:
                    return;
            }
        }
    }

    /// <summary>
    /// Saves a set of usernames to the usernames database file.
    /// <para/>
    /// Any errors, such as failing to open the file or not having
    /// read access, will be propagated up to the caller.
    /// </summary>
    /// <param name="usernames">A set of usernames to save</param>
    /// <param name="filename">The file to which the usernames are saved</param>
    private static void SaveUsernames(SortedSet<string> usernames, string filename)
    {
        using var file = File.Create(filename);
        using BufferedStream buffer = new(file);
        using StreamWriter writer = new(buffer);

        foreach (var username in usernames)
        {
            writer.WriteLine(username);
        }
    }
}
