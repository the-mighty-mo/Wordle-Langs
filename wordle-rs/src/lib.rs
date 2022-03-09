use std::{
    collections::{BTreeSet, HashSet},
    fs::{self, File},
    io::{self, BufWriter},
};

use main_menu::ProgramState;

mod database;
mod main_menu;
mod players;
mod wordle;

/// Name of the usernames database file
pub const USERNAMES_FILENAME: &str = "users.txt";

/// Runs the main state machine of the Wordle program.
///
/// This function manages the various states of the program
/// after all necessary variables have been initialized,
/// such as the dictionary and the set of existing usernames.
///
/// # Examples
///
/// Basic usage:
///
/// ```ignore
/// # use std::collections::{BTreeSet, HashSet};
/// # fn read_dictionary(filename: &str) -> HashSet<String> {
/// # HashSet::new()
/// # }
/// # fn read_usernames(filename: &str) -> BTreeSet<String> {
/// # BTreeSet::new()
/// # }
/// let dictionary: HashSet<String> =
///     read_dictionary("dictionary.txt");
/// let usernames: BTreeSet<String> =
///     read_usernames("usernames.txt");
/// wordle_rs::run_state_machine(dictionary, usernames);
/// ```
pub fn run_state_machine(dictionary: HashSet<String>, mut usernames: BTreeSet<String>) {
    let mut state = ProgramState::LogIn;
    let mut run_program = true;

    let mut current_player = None;

    while run_program {
        (|| match state {
            ProgramState::LogIn => {
                current_player = main_menu::request_user_login(&mut usernames);
                match current_player {
                    Some(_) => {
                        if save_usernames(&usernames, USERNAMES_FILENAME).is_err() {
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
                state = main_menu::run(current_player.as_mut().unwrap(), &dictionary)
            }
            ProgramState::DeleteUser => {
                /* remove the current player from the databse */
                let username = current_player.as_ref().unwrap().get_username();
                usernames.remove(username);
                _ = fs::remove_file(username.to_owned() + ".txt").is_err();

                /* save the username database */
                if save_usernames(&usernames, USERNAMES_FILENAME).is_err() {
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
/// read access, will be propogated up to the caller.
///
/// # Example
///
/// Basic usage:
///
/// ```
/// let mut usernames = BTreeSet::new();
/// usernames.insert("user");
/// save_usernames(&usernames, "users.txt")?;
/// ```
fn save_usernames(usernames: &BTreeSet<String>, filename: &str) -> io::Result<()> {
    let file = File::create(filename)?;
    let mut writer = BufWriter::new(file);
    use std::io::Write;
    for username in usernames.iter() {
        writeln!(writer, "{}", username)?;
    }

    Ok(())
}
