//! This program runs a game of Wordle using a dictionary
//! file provided through the command-line arguments.
//!
//! Author: Benjamin Hall

use std::{
    collections::{BTreeSet, HashSet},
    env,
    fs::{File, OpenOptions},
    io::{self, BufReader, Read},
};

/// Runs the Wordle program.
///
/// The user must pass in the name of the dictionary
/// file as a command-line argument to the program.
fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 2 {
        /* invalid number of arguments, print a help message */
        println!("Wordle");
        println!("Author: Benjamin Hall");
        println!("Usage: ./wordle-rs [dictionary file name]");
        return;
    }

    let dict_file_name = args[1].as_str();

    /* make sure none of the inputs are empty */
    if dict_file_name.is_empty() {
        println!("Error: no dictionary file specified");
        return;
    }

    let dict_file = File::open(dict_file_name);
    let dict_file_contents = match read_file(dict_file) {
        Ok(dict_file_contents) => dict_file_contents,
        Err(_) => {
            println!("Error: could not read dictionary file");
            return;
        }
    };

    let usernames_file = OpenOptions::new()
        .read(true)
        .write(true)
        .create(true)
        .open(wordle_rs::USERNAMES_FILENAME);
    let usernames_file_contents = match read_file(usernames_file) {
        Ok(usernames_file_contents) => usernames_file_contents,
        Err(_) => {
            println!("Error: could not read user database");
            return;
        }
    };

    let dictionary: HashSet<String> = dict_file_contents
        .lines()
        .filter(|s| s.len() == 5)
        .map(str::to_uppercase)
        .collect();

    let usernames: BTreeSet<String> = usernames_file_contents.lines().map(str::to_owned).collect();

    wordle_rs::run_state_machine(dictionary, usernames);
}

/// Loads data from a file into a String.
///
/// Any errors, such as the file not existing or not having
/// read access, will be propogated up to the caller.
///
/// # Example
///
/// Basic usage:
/// ```
/// # use std::{fs::File, io};
/// # fn main() -> io::Result<()> {
/// let file = File::open("file.txt");
/// let file_contents = read_file(file)?;
/// # Ok(())
/// # }
/// ```
fn read_file(file: io::Result<File>) -> io::Result<String> {
    let file = file?;

    let mut buffer = String::new();
    let mut reader = BufReader::new(file);
    reader.read_to_string(&mut buffer)?;
    Ok(buffer)
}
