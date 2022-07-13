using Wordle_CS.Players;

namespace Wordle_CS.ConsoleApp;

public static class ConsoleApp
{
    public const string USERNAMES_FILE = "users.txt";

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
