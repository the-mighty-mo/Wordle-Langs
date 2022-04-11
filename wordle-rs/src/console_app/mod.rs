//! This module provides methods to run a game of Wordle
//! as a console application.
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

/// Runs the main state machine of the Wordle console program.
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
/// # use wordle::console_app;
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
/// console_app::run(&dictionary, &mut usernames);
/// ```
pub fn run<H: std::hash::BuildHasher>(
    dictionary: &HashSet<String, H>,
    usernames: &mut BTreeSet<String>,
) {
    let mut state = ProgramState::LogIn;
    let mut run_program = true;

    let mut current_player = None;

    while run_program {
        (|| match state {
            ProgramState::LogIn => {
                current_player = main_menu::request_user_login(usernames);
                match current_player {
                    Some(_) => {
                        if save_usernames(usernames, USERNAMES_FILENAME).is_err() {
                            println!("Error: could not write to the user database");
                            state = ProgramState::Exit;
                            return;
                        }
                        /* user has logged in, continue to the main menu */
                        state = ProgramState::MainMenu;
                    }
                    /* user requested to exit, or there was an error */
                    None => state = ProgramState::Exit,
                }
            }
            ProgramState::MainMenu => {
                state = main_menu::run(current_player.as_mut().unwrap(), dictionary);
            }
            ProgramState::DeleteUser => {
                /* remove the current player from the databse */
                let username = current_player.as_ref().unwrap().get_username();
                usernames.remove(username);
                _ = fs::remove_file(username.to_owned() + ".txt").is_err();

                /* save the username database */
                if save_usernames(usernames, USERNAMES_FILENAME).is_err() {
                    println!("Error: could not write to the user database");
                    state = ProgramState::Exit;
                    return;
                }
                /* user has logged out, go to the login screen */
                state = ProgramState::LogIn;
            }
            /* end the main loop */
            ProgramState::Exit => run_program = false,
        })();
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
/// # use wordle::console_app;
/// # fn main() -> io::Result<()> {
/// let mut usernames = BTreeSet::new();
/// usernames.insert(String::from("user"));
/// console_app::save_usernames(&usernames, "users.txt")?;
/// # Ok(())
/// # }
/// ```
fn save_usernames(usernames: &BTreeSet<String>, filename: &str) -> io::Result<()> {
    use std::io::Write;
    let file = File::create(filename)?;
    let mut writer = BufWriter::new(file);
    for username in usernames.iter() {
        writeln!(writer, "{}", username)?;
    }

    Ok(())
}
