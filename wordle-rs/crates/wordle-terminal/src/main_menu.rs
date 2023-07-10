//! This module provides methods to manage the main menu of
//! the Wordle program, such as user login and running the game.
//!
//! Author: Benjamin Hall

use std::{
    borrow::Borrow,
    collections::{BTreeSet, HashSet},
    io::{self, stdin, Write},
};

use crate::game;
use wordle::WordleAnswer;
use wordle_db::PlayerInfo;

/// Possible states of the main Wordle program.
#[derive(Default, Debug, Clone, PartialEq, Eq)]
pub enum ProgramState<S> {
    /// Request the user's login information
    #[default]
    LogIn,
    /// Run the main menu
    MainMenu(PlayerInfo<S>),
    /// Delete the current user
    DeleteUser(PlayerInfo<S>),
    /// Exit the program
    Exit,
}

/// Possible user selections in the main menu.
#[derive(Debug, Copy, Clone, PartialEq, Eq)]
enum UserSelection {
    /// Play a game of Wordle
    PlayGame = 1,
    /// View the current player's statistics
    ViewStats,
    /// Log off
    LogOff,
    /// Delete the current user
    DeleteUser,
}

impl TryFrom<isize> for UserSelection {
    type Error = ();

    fn try_from(v: isize) -> Result<Self, Self::Error> {
        match v {
            x if x == Self::PlayGame as isize => Ok(Self::PlayGame),
            x if x == Self::ViewStats as isize => Ok(Self::ViewStats),
            x if x == Self::LogOff as isize => Ok(Self::LogOff),
            x if x == Self::DeleteUser as isize => Ok(Self::DeleteUser),
            _ => Err(()),
        }
    }
}

/// Requests a user to enter their login information.
///
/// The user may choose to quit the program (or forcibly
/// quit using Ctrl-C), in which case this function returns
/// None. Otherwise, this function returns information
/// about the player.
///
/// If the user does not yet exist in the given databse,
/// they will be added to it.
///
/// # Panics
///
/// Panics if writing to [`io::stdout`] fails.
///
/// # Examples
///
/// Basic usage:
/// ```
/// # use std::collections::BTreeSet;
/// # use wordle_terminal::main_menu;
/// # fn read_usernames(filename: &str) -> BTreeSet<String> {
/// # BTreeSet::new()
/// # }
/// let mut usernames: BTreeSet<String> =
///     read_usernames("usernames.txt");
///
/// let player_info = main_menu::request_user_login(&mut usernames);
/// match player_info {
///     Some(player) => { /* run program */ },
///     None => { /* exit program */ }
/// }
/// ```
#[must_use]
pub fn request_user_login(usernames: &mut BTreeSet<String>) -> Option<PlayerInfo<String>> {
    let Some(username) = request_username(usernames) else {
        /* user requested to exit the game */
        return None;
    };

    let player_info = match PlayerInfo::from_file(&(username.clone() + ".txt")) {
        Ok(player_info) => player_info,
        /* error reading the database file */
        Err(msg) => {
            println!("{msg}");
            return None;
        }
    };

    println!("Hello, {username}");

    /* this might be a new user, create a fresh instance of PlayerInfo if so */
    player_info.or_else(|| Some(PlayerInfo::new(username)))
}

/// Requests a user to enter their username.
///
/// The user may choose to quit the program (or forcibly
/// quit using Ctrl-C), in which case this function returns
/// None.
///
/// If the user does not yet exist in the given database,
/// they will be added to it.
///
/// # Panics
///
/// Panics if writing to [`io::stdout`] fails.
///
/// # Examples
///
/// Basic usage:
/// ```ignore
/// # use std::collections::BTreeSet;
/// # use wordle_terminal::main_menu;
/// # fn read_usernames(filename: &str) -> BTreeSet<String> {
/// # BTreeSet::new()
/// # }
/// let mut usernames: BTreeSet<String> =
///     read_usernames("usernames.txt");
///
/// let username = main_menu::request_username(&mut usernames);
/// match username {
///     Some(username) => { /* create player with username */ },
///     None => { /* exit program */ }
/// }
/// ```
#[must_use]
fn request_username(usernames: &mut BTreeSet<String>) -> Option<String> {
    {
        let mut lock = io::stdout().lock();
        if !usernames.is_empty() {
            writeln!(lock, "List of existing users:").unwrap();
            for name in usernames.iter() {
                writeln!(lock, "{name}").unwrap();
            }
            writeln!(lock).unwrap();
        }

        writeln!(lock, "Note: usernames are case-insensitive").unwrap();
        writeln!(lock, "Type \":q\" to exit").unwrap();
        write!(lock, "Username: ").unwrap();
        lock.flush().unwrap();
    }

    let mut username = String::new();
    if stdin().read_line(&mut username).is_err() {
        /* user likely quit the program with Ctrl-C */
        return None;
    }
    let username = username.trim().to_lowercase();

    if username == ":q" {
        /* user wants to exit */
        return None;
    }

    if !usernames.contains(&username) {
        /* new user, add to database */
        usernames.insert(username.clone());
    }

    Some(username)
}

/// Runs the Wordle main menu.
///
/// The main menu gives the player four options:
/// - Play a game of Wordle
/// - View their statistics
/// - Log out
/// - Delete their account
///
/// This function lets the caller know what the next
/// state of the program should be. For example, if
/// the user has logged off, the main program should
/// return to the login screen.
///
/// # Panics
///
/// Panics if writing to [`io::stdout`] fails.
///
/// # Examples
///
/// Basic usage:
/// ```no_run
/// # use std::collections::{BTreeSet, HashSet};
/// # use wordle_terminal::main_menu;
/// # fn read_dictionary(filename: &str) -> HashSet<String> {
/// # HashSet::new()
/// # }
/// # fn read_usernames(filename: &str) -> BTreeSet<String> {
/// # BTreeSet::new()
/// # }
/// let dictionary: HashSet<String> =
///     read_dictionary("dictionary.txt");
/// let mut usernames: BTreeSet<String> =
///     read_usernames("usernames.txt");
///
/// let player_info = main_menu::request_user_login(&mut usernames);
/// if let Some(player_info) = player_info {
///     let next_state = main_menu::run(player_info, &dictionary);
/// }
/// ```
#[must_use]
pub fn run<S>(
    mut player: PlayerInfo<S>,
    dictionary: &HashSet<String, impl std::hash::BuildHasher>,
) -> ProgramState<S>
where
    S: Borrow<str>,
{
    let Some(user_selection) = request_user_selection() else {
        /* user likely quit the program with Ctrl-C */
        return ProgramState::Exit;
    };

    match user_selection {
        UserSelection::PlayGame => {
            /* run a game of Wordle */
            if let Some(answer) = player.get_random_word(dictionary) {
                let answer = WordleAnswer::new(answer);
                game::run(&answer, &mut player, dictionary);
                /* print the player's statistics after the game ends */
                println!("{}", player.get_stats());
                /* save the user's new statistics to their database */
                if player
                    .write_to_file(&(player.get_username().to_owned() + ".txt"))
                    .is_err()
                {
                    /* report that we could not write to the database, but do not exit */
                    println!(
                        "Error: could not write to user database file, progress has not been saved"
                    );
                }
            } else {
                /* couldn't get a word, player has already played every word */
                println!("There are no remaining words in the dictionary.");
            }
            ProgramState::MainMenu(player)
        }
        UserSelection::ViewStats => {
            println!("{}", player.get_stats());
            ProgramState::MainMenu(player)
        }
        /* user is logged off, go back to login screen */
        UserSelection::LogOff => ProgramState::LogIn,
        UserSelection::DeleteUser => {
            print!(
                "Are you sure you would like to delete user: {} [y/N] ",
                player.get_username()
            );
            io::stdout().flush().unwrap();

            let mut user_confirmation = String::new();
            match stdin().read_line(&mut user_confirmation) {
                Ok(_)
                    if {
                        user_confirmation.make_ascii_lowercase();
                        user_confirmation.trim()
                    } == "y" =>
                {
                    println!();
                    ProgramState::DeleteUser(player)
                }
                _ => {
                    println!("Action aborted");
                    ProgramState::MainMenu(player)
                }
            }
        }
    }
}

/// Requests a user to input their selection.
///
/// This function gives the player four options:
/// - Play a game of Wordle
/// - View their statistics
/// - Log out
/// - Delete their account
///
/// The user can terminate the program early using Ctrl-C,
/// in which case this function returns None.
///
/// # Panics
///
/// Panics if writing to [`io::stdout`] fails.
///
/// # Examples
///
/// Basic usage:
/// ```ignore
/// # use wordle_terminal::main_menu;
/// let user_selection = main_menu::request_user_selection();
/// match (user_selection) {
///     Some(user_selection) => { /* run selection */ },
///     None => { /* exit program */ }
/// }
/// ```
#[must_use]
fn request_user_selection() -> Option<UserSelection> {
    {
        let mut lock = io::stdout().lock();
        writeln!(lock).unwrap();
        writeln!(lock, "[1] Play a game of Wordle").unwrap();
        writeln!(lock, "[2] View player statistics").unwrap();
        writeln!(lock, "[3] Log off").unwrap();
        writeln!(lock, "[4] Delete user").unwrap();
    }

    let user_selection = loop {
        print!("Selection: ");
        io::stdout().flush().unwrap();

        let mut selection_str = String::new();
        if stdin().read_line(&mut selection_str).is_err() {
            /* user likely quit the program with Ctrl-C */
            break None;
        }

        if let Ok(selection) = selection_str.trim().parse::<isize>() {
            let user_selection = UserSelection::try_from(selection).ok();
            if user_selection.is_none() {
                /* selection out of range */
                println!("Error: invalid selection");
            } else {
                /* valid selection, stop the read loop */
                break user_selection;
            }
        } else {
            println!("Error: selection must be an integer");
        }
    };
    println!();

    user_selection
}
