using Wordle_CS.Players;

namespace Wordle_CS.ConsoleApp;

public enum ProgramState
{
    /// Request the user's login information
    LogIn,
    /// Run the main menu
    MainMenu,
    /// Delete the current user
    DeleteUser,
    /// Exit the program
    Exit,
}

enum UserSelection
{
    /// Play a game of Wordle
    PlayGame = 1,
    /// View the current player's statistics
    ViewStats,
    /// Log off
    LogOff,
    /// Delete the current user
    DeleteUser,
}

public class MainMenu
{
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
            playerInfo = PlayerInfo.FromFile($"{username}.txt");
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
                WordleAnswer answer = new(currentPlayer.GetRandomWord(dictionary));
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
