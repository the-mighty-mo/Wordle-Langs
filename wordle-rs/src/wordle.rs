//! This module provides methods and structs to manage
//! a game of Wordle, such as the guessing algorithm.
//!
//! Author: Benjamin Hall

use std::fmt;

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
#[derive(Debug, Clone, PartialEq, Eq)]
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
    ///
    /// let all_present = answer.check_guess("ETRAC");
    /// assert_eq!(all_present, guess_result![Y Y Y Y Y]);
    ///
    /// let all_incorrect = answer.check_guess("BLIND");
    /// assert_eq!(all_incorrect, guess_result![X X X X X]);
    /// ```
    /// Examples with duplicate letters:
    /// ```
    /// # use wordle_rs::wordle::{guess_result, WordleAnswer};
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
/// for a won game of Wordle.
///
/// If the user guessed the word in n
/// guesses (starting at 1), then
/// WIN_MESSAGES[n - 1] is the message
/// that should be displayed.
pub const WIN_MESSAGES: [&'static str; 6] = [
    "Genius",
    "Magnificent",
    "Impressive",
    "Splendid",
    "Great",
    "Phew",
];

#[cfg(test)]
mod test {
    mod wordle {
        use crate::wordle::{guess_result, WordleAnswer};

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
