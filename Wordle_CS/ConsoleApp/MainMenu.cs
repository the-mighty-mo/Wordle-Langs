using Wordle_CS.Players;

namespace Wordle_CS.ConsoleApp;

/// <summary>
/// Possible states of the main Wordle program.
/// </summary>
public enum ProgramState
{
    /// <summary>
    /// Request the user's login information
    /// </summary>
    LogIn,
    /// <summary>
    /// Run the main menu
    /// </summary>
    MainMenu,
    /// <summary>
    /// Delete the current user
    /// </summary>
    DeleteUser,
    /// <summary>
    /// Exit the program
    /// </summary>
    Exit,
}

/// <summary>
/// Possible selections in the main menu.
/// </summary>
enum UserSelection
{
    /// <summary>
    /// Play a game of Wordle
    /// </summary>
    PlayGame = 1,
    /// <summary>
    /// View the current player's statistics
    /// </summary>
    ViewStats,
    /// <summary>
    /// Log off
    /// </summary>
    LogOff,
    /// <summary>
    /// Delete the current user
    /// </summary>
    DeleteUser,
}

/// <summary>
/// Provides methods to manage the main menu of the Wordle
/// program, such as user login and running the game.
/// </summary>
public class MainMenu
{
    /// <summary>
    /// Requests a user to enter their login information.
    /// <para/>
    /// The user may choose to quit the program (or forcibly
    /// quit using Ctrl-C), in which case this function returns
    /// None. Otherwise, this function returns information
    /// about the player.
    /// <para/>
    /// If the user does not yet exist in the given databse,
    /// they will be added to it.
    /// </summary>
    /// <param name="usernames">A set of existing usernames</param>
    /// <returns>
    /// Information about the player, or <see langword="null"/> if the program
    /// should exit (either by user request or a database error)
    /// </returns>
    public static PlayerInfo? RequestUserLogin(SortedSet<string> usernames)
    {
        var username = RequestUsername(usernames);
        if (username is null)
        {
            return null;
        }

        PlayerInfo? playerInfo;
        try
        {
            playerInfo = PlayerInfo.CreateFromFile($"{username}.txt");
        }
        catch (IOException e)
        {
            Console.WriteLine(e.Message);
            return null;
        }

        Console.WriteLine($"Hello, {username}");

        var player = playerInfo ?? new PlayerInfo(username);
        return player;
    }

    /// <summary>
    /// Requests a user to enter their username.
    /// <para/>
    /// The user may choose to quit the program (or forcibly
    /// quit using Ctrl-C), in which case this function returns
    /// a nullopt.
    /// <para/>
    /// If the user does not yet exist in the given database,
    /// they will be added to it.
    /// </summary>
    /// <param name="usernames">A set of existing usernames</param>
    /// <returns>
    /// The player's username, or <see langword="null"/> if the program should
    /// exit (either by user request or a database error)
    /// </returns>
    private static string? RequestUsername(SortedSet<string> usernames)
    {
        if (usernames.Count != 0)
        {
            Console.WriteLine("List of existing users:");
            foreach (var name in usernames)
            {
                Console.WriteLine(name);
            }
            Console.WriteLine();
        }

        Console.WriteLine("Note: usernames are case-insensitive");
        Console.WriteLine("Type \":q\" to exit");
        Console.Write("Username: ");

        string? username = Console.ReadLine();
        if (username is null or ":q")
        {
            return null;
        }

        usernames.Add(username);

        return username;
    }

    /// <summary>
    /// Runs the Wordle main menu.
    /// <para/>
    /// The main menu gives the player four options:
    /// <list type="bullet">
    ///     <item>Play a game of Wordle</item>
    ///     <item>View their statistics</item>
    ///     <item>Log out</item>
    ///     <item>Delete their account</item>
    /// </list>
    /// <para/>
    /// This function lets the caller know what the next
    /// state of the program should be. For example, if
    /// the user has logged off, the main program should
    /// return to the login screen.
    /// </summary>
    /// <param name="currentPlayer">The logged-in player</param>
    /// <param name="dictionary">A dictionary of valid Wordle words</param>
    /// <returns></returns>
    public static ProgramState Run(PlayerInfo currentPlayer, HashSet<string> dictionary)
    {
        var userSelection = RequestUserSelection();
        if (userSelection is null)
        {
            return ProgramState.Exit;
        }

        switch (userSelection)
        {
            case UserSelection.PlayGame:
                var randWord = currentPlayer.GetRandomWord(dictionary);
                if (randWord is not null)
                {
                    WordleAnswer answer = new(randWord);
                    Game.Run(answer, currentPlayer, dictionary);
                    Console.WriteLine(currentPlayer.GetStats());
                    try
                    {
                        currentPlayer.WriteToFile($"{currentPlayer.Username}.txt");
                    }
                    catch (Exception)
                    {
                        Console.WriteLine("Error: could not write to user database file, progress has not been saved");
                    }
                }
                else
                {
                    /* couldn't get a word, player has already played every word */
                    Console.WriteLine("There are no remaining words in the dictionary.");
                }

                return ProgramState.MainMenu;

            case UserSelection.ViewStats:
                Console.WriteLine(currentPlayer.GetStats());
                return ProgramState.MainMenu;

            case UserSelection.LogOff:
                return ProgramState.LogIn;

            case UserSelection.DeleteUser:
                Console.Write($"Are you sure you would like to delete user: {currentPlayer.Username} [y/N] ");

                var userConfirmation = Console.ReadLine();
                if (userConfirmation?.ToLower()?.Trim() == "y")
                {
                    Console.WriteLine();
                    return ProgramState.DeleteUser;
                }
                else
                {
                    Console.WriteLine("Action aborted");
                    return ProgramState.MainMenu;
                }
        }

        return ProgramState.MainMenu;
    }

    /// <summary>
    /// Requests a user to input their selection.
    /// <para/>
    /// This function gives the player four options:
    /// <list type="bullet">
    ///     <item>Play a game of Wordle</item>
    ///     <item>View their statistics</item>
    ///     <item>Log out</item>
    ///     <item>Delete their account</item>
    /// </list>
    /// <para/>
    /// The user can terminate the program early using Ctrl-C,
    /// in which case this function returns a nullopt.
    /// </summary>
    /// <returns>The user selection, or <see langword="null"/> if none was given</returns>
    private static UserSelection? RequestUserSelection()
    {
        Console.WriteLine();
        Console.WriteLine("[1] Play a game of Wordle");
        Console.WriteLine("[2] View player statistics");
        Console.WriteLine("[3] Log off");
        Console.WriteLine("[4] Delete user");

        UserSelection? userSelection;
        while (true)
        {
            Console.Write("Selection: ");
            var selectionStr = Console.ReadLine();
            if (selectionStr is null)
            {
                userSelection = null;
                break;
            }

            if (int.TryParse(selectionStr, out int selection))
            {
                userSelection = (UserSelection)selection;
                break;
            }
            else
            {
                Console.WriteLine("Error: selection must be an integer");
            }
        }
        Console.WriteLine();

        return userSelection;
    }
}
