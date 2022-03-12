//! This module provides methods and structs to manage
//! a game of Wordle, such as the guessing algorithm.
//!
//! Author: Benjamin Hall

use std::{
    collections::HashSet,
    fmt,
    io::{self, Write},
};

use crate::players::PlayerInfo;

/// Possible guess results for a letter in a game of Wordle.
#[derive(Debug, Copy, Clone, PartialEq, Eq)]
pub enum WordleGuess {
    /// Green ("G"), the letter is in the word at that position
    Correct,
    /// Yellow ("Y"), the letter is in the word, but not at that position
    Present,
    /// Gray ("X"), there are no more instances of the letter in the word
    Incorrect,
}

impl fmt::Display for WordleGuess {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            WordleGuess::Correct => write!(f, "G"),
            WordleGuess::Present => write!(f, "Y"),
            WordleGuess::Incorrect => write!(f, "X"),
        }
    }
}

/// Stores information about an answer to a game of Wordle.
///
/// A game of Worlde has a target word. The guessing algorithm
/// uses preprocessing so it can run in linear time. This results
/// in an array containing the counts of each letter.
#[derive(Debug, Clone)]
pub struct WordleAnswer {
    word: String,
    letter_counts: [u8; 26],
}

/// Creates an array representing the result
/// of a guess in a game of Wordle.
///
/// - G = Green -> [WordleGuess::Correct]
/// - Y = Yellow -> [WordleGuess::Present]
/// - X = Gray -> [WordleGuess::Incorrect]
///
/// # Examples
///
/// Basic usage:
/// ```
/// # use wordle_rs::wordle::{guess_result, WordleGuess};
/// assert_eq!(guess_result![G G G G G], [WordleGuess::Correct; 5]);
/// ```
#[macro_export]
macro_rules! guess_result {
    (G) => {$crate::wordle::WordleGuess::Correct};
    (Y) => {$crate::wordle::WordleGuess::Present};
    (X) => {$crate::wordle::WordleGuess::Incorrect};
    ($($c:tt)+) => {[
        $(guess_result!($c)),+
    ]}
}
pub use guess_result;

impl WordleAnswer {
    /// Creates a new Wordle answer.
    ///
    /// # Examples
    ///
    /// Basic usage:
    /// ```
    /// # use wordle_rs::wordle::WordleAnswer;
    /// let word = String::from("TRACE");
    /// let answer = WordleAnswer::new(word);
    /// ```
    pub fn new(word: String) -> Self {
        let mut letter_counts = [0; 26];
        word.chars()
            .for_each(|c| letter_counts[c as usize - 'A' as usize] += 1);
        Self {
            word,
            letter_counts,
        }
    }

    /// Gets the word stored in this answer.
    ///
    /// # Examples
    ///
    /// Basic usage:
    /// ```
    /// # use wordle_rs::wordle::WordleAnswer;
    /// let word = "TRACE";
    /// let answer = WordleAnswer::new(word.to_owned());
    /// assert_eq!(answer.get_word(), word);
    /// ```
    pub fn get_word(&self) -> &str {
        &self.word
    }

    /// Calculates the correctness of a guess.
    ///
    /// This function returns an array containing the correctness
    /// of each letter of the guess.
    ///
    /// It is important to note that the sum of Correct and Present
    /// instances for a given letter cannot exceed the total number
    /// of instances of the letter in the answer. Additionally,
    /// Correct always takes priority over Present.
    ///
    /// # Examples
    ///
    /// Basic usage:
    /// ```
    /// # use wordle_rs::wordle::{guess_result, WordleAnswer};
    /// let answer = WordleAnswer::new(String::from("TRACE"));
    ///
    /// let all_correct = answer.check_guess("TRACE");
    /// assert_eq!(all_correct, guess_result![G G G G G]);
    /// let all_present = answer.check_guess("ETRAC");
    /// assert_eq!(all_present, guess_result![Y Y Y Y Y]);
    /// let all_incorrect = answer.check_guess("BLIND");
    /// assert_eq!(all_incorrect, guess_result![X X X X X]);
    ///
    /// let answer = WordleAnswer::new(String::from("AABBB"));
    /// let some_yellow = answer.check_guess("CAACC");
    /// assert_eq!(some_yellow, guess_result![X G Y X X]);
    ///
    /// let answer = WordleAnswer::new(String::from("AZZAZ"));
    /// let some_green = answer.check_guess("AAABB");
    /// assert_eq!(some_green, guess_result![G Y X X X]);
    ///
    /// let answer = WordleAnswer::new(String::from("BACCC"));
    /// let no_yellow = answer.check_guess("AADDD");
    /// assert_eq!(no_yellow, guess_result![X G X X X]);
    /// ```
    pub fn check_guess(&self, guess: &str) -> [WordleGuess; 5] {
        let mut colors = [WordleGuess::Incorrect; 5];
        let mut letter_counts = self.letter_counts;

        /* first check for green letters */
        for (i, (a, g)) in self.word.chars().zip(guess.chars()).enumerate() {
            if a == g {
                letter_counts[g as usize - 'A' as usize] -= 1;
                colors[i] = WordleGuess::Correct;
            }
        }

        /* then check for yellow letters */
        for (i, g) in guess.chars().enumerate() {
            if colors[i] == WordleGuess::Incorrect {
                /* letter has not yet been checked */
                if letter_counts[g as usize - 'A' as usize] > 0 {
                    /* letter in word but not this position */
                    colors[i] = WordleGuess::Present;
                    letter_counts[g as usize - 'A' as usize] -= 1;
                }
            }
        }

        colors
    }
}

/// Contains all the possible messages
/// for a won game of Wordle
const WIN_MESSAGES: [&'static str; 6] = [
    "Genius",
    "Magnificent",
    "Impressive",
    "Splendid",
    "Great",
    "Phew",
];

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
/// ```
/// # use std::{collections::HashSet, io};
/// # use wordle_rs::{
/// #     players::PlayerInfo,
/// #     wordle::{self, WordleAnswer},
/// # };
/// # fn read_dictionary(filename: &str) -> HashSet<String> {
/// # HashSet::new()
/// # }
/// # fn main() -> io::Result<()> {
/// let dictionary: HashSet<String> =
///     read_dictionary("dictionary.txt");
/// let word = String::from("TRACE");
/// let answer = WordleAnswer::new(word);
/// let player = PlayerInfo::from_file("user.txt")?;
///
/// if let Some(mut player) = player {
///     // runs one game of Wordle where the answer is "TRACE"
///     wordle::run_game(&answer, &mut player, &dictionary);
/// }
/// # Ok(())
/// # }
/// ```
pub fn run_game(answer: &WordleAnswer, player: &mut PlayerInfo, dictionary: &HashSet<String>) {
    println!("Guess the 5-letter word in 6 or fewer guesses.");
    println!("After each guess, each letter will be given a color:");
    println!("G = Green:\tletter is in that position in the word");
    println!("Y = Yellow:\tletter is in the word, but not that position");
    println!("X = Black:\tthere are no more instances of the letter in the word");
    println!();

    let won_game = (1..=6).find_map(|i| {
        let mut guess = String::new();
        let mut read = true;
        while read {
            print!("[{i}] ");
            io::stdout().flush().unwrap();

            guess.clear();
            match io::stdin().read_line(&mut guess) {
                Ok(_) => {}
                Err(_) => return Some(-1),
            }
            guess = guess.trim().to_uppercase();
            if guess.len() != 5 {
                println!("Error: guess must be 5 letters");
            } else if !dictionary.contains(&guess) {
                println!("Error: guess must be a word in the dictionary");
            } else {
                /* valid guess, stop the read loop */
                read = false;
            }
        }

        let colors = answer.check_guess(&guess);
        print!("    ");
        colors.iter().for_each(|c| print!("{c}"));
        println!();

        if colors.iter().all(|c| *c == WordleGuess::Correct) {
            Some(i)
        } else {
            None
        }
    });

    match won_game {
        Some(i) if i > 0 => {
            player.add_won_word(answer.get_word().to_owned(), i as usize);
            print!("{}! ", WIN_MESSAGES[i as usize - 1]);
        }
        None => {
            player.add_lost_word(answer.get_word().to_owned());
            print!("Too bad! ");
        }
        Some(_) => return,
    }
    println!("The word was: {}", answer.get_word());
    println!();
}

#[cfg(test)]
mod test {
    mod wordle {
        use crate::wordle::{guess_result, WordleAnswer, WordleGuess};

        #[test]
        fn all_green() {
            let answer = WordleAnswer::new("ABCDE".to_owned());
            assert_eq!(answer.check_guess("ABCDE"), guess_result![G G G G G]);
        }

        #[test]
        fn all_yellow() {
            let answer = WordleAnswer::new("EABCD".to_owned());
            assert_eq!(answer.check_guess("ABCDE"), guess_result![Y Y Y Y Y]);
        }

        #[test]
        fn all_black() {
            let answer = WordleAnswer::new("FGHIJ".to_owned());
            assert_eq!(answer.check_guess("ABCDE"), guess_result![X X X X X]);
        }

        #[test]
        fn repeat_green() {
            let answer = WordleAnswer::new("AABBB".to_owned());
            assert_eq!(answer.check_guess("AACCC"), guess_result![G G X X X]);
        }

        #[test]
        fn repeat_yellow() {
            let answer = WordleAnswer::new("AABBB".to_owned());
            assert_eq!(answer.check_guess("CCAAC"), guess_result![X X Y Y X]);
        }

        #[test]
        fn repeat_some_green() {
            let answer = WordleAnswer::new("AABBB".to_owned());
            assert_eq!(answer.check_guess("CAACC"), guess_result![X G Y X X]);
        }

        #[test]
        fn repeat_some_yellow() {
            let answer = WordleAnswer::new("AZZAZ".to_owned());
            assert_eq!(answer.check_guess("AAABB"), guess_result![G Y X X X]);
        }

        #[test]
        fn green_no_yellow() {
            let answer = WordleAnswer::new("BACCC".to_owned());
            assert_eq!(answer.check_guess("AADDD"), guess_result![X G X X X]);
        }
    }
}
