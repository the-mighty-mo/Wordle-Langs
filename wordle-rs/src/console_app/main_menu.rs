//! This module provides methods to manage the main menu of
//! the Wordle program, such as user login and running the game.
//!
//! Author: Benjamin Hall

use std::{
    collections::{BTreeSet, HashSet},
    io::{self, stdin, Write},
};

use crate::{console_app::game, players::PlayerInfo, WordleAnswer};

/// Possible states of the main Wordle program.
#[derive(Debug, Copy, Clone, PartialEq, Eq)]
pub enum ProgramState {
    /// Request the user's login information
    LogIn,
    /// Run the main menu
    MainMenu,
    /// Delete the current user
    DeleteUser,
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
            x if x == UserSelection::PlayGame as isize => Ok(UserSelection::PlayGame),
            x if x == UserSelection::ViewStats as isize => Ok(UserSelection::ViewStats),
            x if x == UserSelection::LogOff as isize => Ok(UserSelection::LogOff),
            x if x == UserSelection::DeleteUser as isize => Ok(UserSelection::DeleteUser),
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
/// # Examples
///
/// Basic usage:
/// ```
/// # use std::collections::BTreeSet;
/// # use wordle_rs::console_app::main_menu;
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
pub fn request_user_login(usernames: &mut BTreeSet<String>) -> Option<PlayerInfo> {
    let username = match request_username(usernames) {
        Some(username) => username,
        /* user requested to exit the game */
        None => return None,
    };

    let player_info = PlayerInfo::from_file(&(username.clone() + ".txt"));
    let player_info = match player_info {
        Ok(player_info) => player_info,
        /* error reading the database file */
        Err(msg) => {
            println!("{}", msg);
            return None;
        }
    };

    println!("Hello, {username}");

    /* this might be a new user, create a fresh instance of PlayerInfo if so */
    let player_info = player_info.unwrap_or_else(|| PlayerInfo::new(username));
    Some(player_info)
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
/// # Examples
///
/// Basic usage:
/// ```ignore
/// # use std::collections::BTreeSet;
/// # use wordle_rs::console_app::main_menu;
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
fn request_username(usernames: &mut BTreeSet<String>) -> Option<String> {
    if !usernames.is_empty() {
        println!("List of existing users:");
        for name in usernames.iter() {
            println!("{name}");
        }
        println!();
    }

    println!("Note: usernames are case-insensitive");
    println!("Type \":q\" to exit");
    print!("Username: ");
    io::stdout().flush().unwrap();

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
/// # Examples
///
/// Basic usage:
/// ```no_run
/// # use std::collections::{BTreeSet, HashSet};
/// # use wordle_rs::console_app::main_menu;
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
/// let mut player_info = main_menu::request_user_login(&mut usernames).unwrap();
/// let next_state = main_menu::run(&mut player_info, &dictionary);
/// ```
#[must_use]
pub fn run<H: std::hash::BuildHasher>(
    current_player: &mut PlayerInfo,
    dictionary: &HashSet<String, H>,
) -> ProgramState {
    let mut next_state = ProgramState::MainMenu;

    let user_selection = request_user_selection();
    let user_selection = match user_selection {
        Some(user_selection) => user_selection,
        /* user likely quit the program with Ctrl-C */
        None => return ProgramState::Exit,
    };

    match user_selection {
        UserSelection::PlayGame => {
            /* run a game of Wordle */
            let answer = WordleAnswer::new(current_player.get_random_word(dictionary));
            game::run(&answer, current_player, dictionary);
            /* print the player's statistics after the game ends */
            println!("{}", current_player.get_stats());
            /* save the user's new statistics to their database */
            if current_player
                .write_to_file(&(current_player.get_username().to_owned() + ".txt"))
                .is_err()
            {
                /* report that we could not write to the database, but do not exit */
                println!(
                    "Error: could not write to user database file, progress has not been saved"
                );
            }
        }
        UserSelection::ViewStats => {
            println!("{}", current_player.get_stats());
        }
        UserSelection::LogOff => {
            /* user is logged off, go back to login screen */
            next_state = ProgramState::LogIn;
        }
        UserSelection::DeleteUser => {
            print!(
                "Are you sure you would like to delete user: {} [y/N] ",
                current_player.get_username()
            );
            io::stdout().flush().unwrap();

            let mut user_confirmation = String::new();
            match stdin().read_line(&mut user_confirmation) {
                Ok(_) if user_confirmation.trim().to_lowercase() == "y" => {
                    next_state = ProgramState::DeleteUser;
                }
                _ => println!("Action aborted"),
            }
        }
    }

    next_state
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
/// # Examples
///
/// Basic usage:
/// ```ignore
/// # use wordle_rs::console_app::main_menu;
/// let user_selection = main_menu::request_user_selection();
/// match (user_selection) {
///     Some(user_selection) => { /* run selection */ },
///     None => { /* exit program */ }
/// }
/// ```
fn request_user_selection() -> Option<UserSelection> {
    let mut user_selection = None;

    println!();
    println!("[1] Play a game of Wordle");
    println!("[2] View player statistics");
    println!("[3] Log off");
    println!("[4] Delete user");

    let mut read = true;
    while read {
        print!("Selection: ");
        io::stdout().flush().unwrap();

        let mut selection_str = String::new();
        if stdin().read_line(&mut selection_str).is_err() {
            /* user likely quit the program with Ctrl-C */
            return None;
        }

        let selection = selection_str.trim().parse::<isize>();

        match selection {
            Ok(selection) => {
                user_selection = UserSelection::try_from(selection).ok();
                match user_selection {
                    /* valid selection, stop the read loop */
                    Some(_) => read = false,
                    /* selection out of range */
                    None => println!("Error: invalid selection"),
                }
            }
            Err(_) => {
                println!("Error: selection must be an integer");
            }
        }
    }
    println!();

    user_selection
}
