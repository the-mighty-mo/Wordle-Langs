//! This module provides methods to run a game
//! of Wordle in a console application.
//!
//! Author: Benjamin Hall

use std::{
    borrow::Borrow,
    collections::HashSet,
    io::{self, Write},
};

use crate::{players::PlayerInfo, WordleAnswer, WordleGuess, WIN_MESSAGES};

/// Runs a game of Wordle.
///
/// This function manages all user input and output using
/// stdin and stdout, respectively, as well as all six
/// guesses. The function ends after the user has guessed
/// the answer or used all six guesses, whichever is first.
///
/// At the start of the game, a message is printed with
/// instructions for the player.
///
/// # Examples
///
/// Basic usage:
/// ```no_run
/// # use std::{collections::HashSet, io};
/// # use wordle::{
/// #     console_app::game,
/// #     players::PlayerInfo,
/// #     WordleAnswer,
/// # };
/// # fn read_dictionary(filename: &str) -> HashSet<String> {
/// # HashSet::new()
/// # }
/// # fn main() -> io::Result<()> {
/// let dictionary: HashSet<String> =
///     read_dictionary("dictionary.txt");
/// let answer = WordleAnswer::new("TRACE");
/// let player = PlayerInfo::from_file("user.txt")?;
///
/// if let Some(mut player) = player {
///     // runs one game of Wordle where the answer is "TRACE"
///     game::run(&answer, &mut player, &dictionary);
/// }
/// # Ok(())
/// # }
/// ```
pub fn run(
    answer: &WordleAnswer<impl Borrow<str>>,
    player: &mut PlayerInfo<impl Borrow<str>>,
    dictionary: &HashSet<String, impl std::hash::BuildHasher>,
) {
    {
        let mut lock = io::stdout().lock();
        writeln!(lock, "Guess the 5-letter word in 6 or fewer guesses.").unwrap();
        writeln!(lock, "After each guess, each letter will be given a color:").unwrap();
        writeln!(lock, "G = Green:\tletter is in that position in the word").unwrap();
        writeln!(
            lock,
            "Y = Yellow:\tletter is in the word, but not that position"
        )
        .unwrap();
        writeln!(
            lock,
            "X = Black:\tthere are no more instances of the letter in the word"
        )
        .unwrap();
        writeln!(lock).unwrap();
    }

    let won_game = (1..=6).find_map(|i| {
        let mut guess = String::new();
        let guess = loop {
            print!("[{i}] ");
            io::stdout().flush().unwrap();

            guess.clear();
            if io::stdin().read_line(&mut guess).is_err() {
                /* user likely quit the program with Ctrl-C */
                return Some(-1);
            }
            guess.make_ascii_uppercase();
            let guess = guess.trim();
            if guess.len() != 5 {
                println!("Error: guess must be 5 letters");
            } else if !dictionary.contains(guess) {
                println!("Error: guess must be a word in the dictionary");
            } else {
                /* valid guess, stop the read loop */
                break guess;
            }
        };

        let colors = answer.check_guess(guess);
        {
            let mut lock = io::stdout().lock();
            write!(lock, "    ").unwrap();
            for color in colors {
                write!(lock, "{color}").unwrap();
            }
            writeln!(lock).unwrap();
        }

        if colors.into_iter().all(|c| c == WordleGuess::Correct) {
            Some(i)
        } else {
            None
        }
    });

    let mut lock = io::stdout().lock();

    match won_game {
        Some(i) if i > 0 => {
            player.add_won_word(answer.get_word().to_owned(), i as usize);
            write!(lock, "{}! ", WIN_MESSAGES[i as usize - 1]).unwrap();
        }
        None => {
            player.add_lost_word(answer.get_word().to_owned());
            write!(lock, "Too bad! ").unwrap();
        }
        /* user likely quit the program with Ctrl-C */
        Some(_) => return,
    }
    writeln!(lock, "The word was: {}", answer.get_word()).unwrap();
    writeln!(lock).unwrap();
}
