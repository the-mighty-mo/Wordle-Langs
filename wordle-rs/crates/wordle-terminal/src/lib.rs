//! This crate provides methods to run a game of Wordle
//! as a terminal application.
//!
//! To run a game of Wordle, the user must first login by
//! either selecting a username from an existing list of
//! usernames, or by creating a new one. From there, the
//! user can play a game of Wordle, view their statistics,
//! log out, or delete their account.
//!
//! User data and a list of usernames are stored to multiple
//! database files. The calling program must load the list
//! of usernames and a dictionary of valid five-letter words.
//! From there, the calling program can start the main state
//! machine of the program.
//!
//! Author: Benjamin Hall

use std::{
    collections::{BTreeSet, HashSet},
    fs::{self, File},
    io::{self, BufWriter},
};

use self::main_menu::ProgramState;

pub mod game;
pub mod main_menu;

/// Name of the usernames database file.
pub const USERNAMES_FILENAME: &str = "users.txt";

/// Runs the main state machine of the Wordle terminal program.
///
/// This function manages the various states of the program
/// after all necessary variables have been initialized,
/// such as the dictionary and the set of existing usernames.
///
/// # Examples
///
/// Basic usage:
/// ```no_run
/// # use std::collections::{BTreeSet, HashSet};
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
/// wordle_terminal::run(&dictionary, &mut usernames);
/// ```
pub fn run(
    dictionary: &HashSet<String, impl std::hash::BuildHasher>,
    usernames: &mut BTreeSet<String>,
) {
    let mut state = ProgramState::default();

    loop {
        state = match state {
            ProgramState::LogIn => {
                if let Some(player) = main_menu::request_user_login(usernames) {
                    if save_usernames(usernames, USERNAMES_FILENAME).is_err() {
                        println!("Error: could not write to the user database");
                        ProgramState::Exit
                    } else {
                        /* user has logged in, continue to the main menu */
                        ProgramState::MainMenu(player)
                    }
                } else {
                    /* user requested to exit, or there was an error */
                    ProgramState::Exit
                }
            }
            ProgramState::MainMenu(player) => main_menu::run(player, dictionary),
            ProgramState::DeleteUser(player) => {
                /* remove the current player from the databse */
                let username = player.get_username();
                usernames.remove(username);
                _ = fs::remove_file(username.to_owned() + ".txt");

                /* save the username database */
                if save_usernames(usernames, USERNAMES_FILENAME).is_err() {
                    println!("Error: could not write to the user database");
                    ProgramState::Exit
                } else {
                    /* user has logged out, go to the login screen */
                    ProgramState::LogIn
                }
            }
            /* end the main loop */
            ProgramState::Exit => break,
        };
    }
}

/// Saves a set of usernames to the usernames database file.
///
/// Any errors, such as failing to open the file or not having
/// read access, will be propagated up to the caller.
///
/// # Example
///
/// Basic usage:
/// ```ignore
/// # use std::{
/// #     collections::BTreeSet,
/// #     io
/// # };
/// # fn main() -> io::Result<()> {
/// let mut usernames = BTreeSet::new();
/// usernames.insert(String::from("user"));
/// wordle_terminal::save_usernames(&usernames, "users.txt")?;
/// # Ok(())
/// # }
/// ```
fn save_usernames(usernames: &BTreeSet<String>, filename: &str) -> io::Result<()> {
    use std::io::Write;
    let file = File::create(filename)?;
    let mut writer = BufWriter::new(file);
    for username in usernames {
        writeln!(writer, "{username}")?;
    }
    writer.flush()
}
