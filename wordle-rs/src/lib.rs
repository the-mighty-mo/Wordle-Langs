//! This crate provides methods and structs to manage
//! a game of Wordle.
//!
//! # Optional features
//!
//! - **`player_db`** - Methods and structs to manage
//!   information about a Wordle player
//! - **`console_app`** *(requires: `player_db`)* - Methods
//!   to run a game of Wordle as a console application
//!
//! Author: Benjamin Hall

#[cfg(feature = "console_app")]
pub mod console_app;
#[cfg(feature = "player_db")]
pub mod players;

use std::{borrow::Borrow, fmt};

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
            Self::Correct => write!(f, "G"),
            Self::Present => write!(f, "Y"),
            Self::Incorrect => write!(f, "X"),
        }
    }
}

/// Stores information about an answer to a game of Wordle.
///
/// A game of Worlde has a target word. The guessing algorithm
/// uses preprocessing so it can run in linear time. This results
/// in an array containing the counts of each letter.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct WordleAnswer<S>
where
    S: Borrow<str>,
{
    word: S,
    letter_counts: [u8; 26],
}

/// Creates an array representing the result
/// of a guess in a game of Wordle.
///
/// - G = Green -> [`WordleGuess::Correct`]
/// - Y = Yellow -> [`WordleGuess::Present`]
/// - X = Gray -> [`WordleGuess::Incorrect`]
///
/// # Examples
///
/// Basic usage:
/// ```
/// # use wordle::{guess_result, WordleGuess};
/// assert_eq!(guess_result![G G G G G], [WordleGuess::Correct; 5]);
/// assert_eq!(guess_result![Y Y Y Y Y], [WordleGuess::Present; 5]);
/// assert_eq!(guess_result![X X X X X], [WordleGuess::Incorrect; 5]);
/// ```
#[macro_export]
macro_rules! guess_result {
    (G) => {$crate::WordleGuess::Correct};
    (Y) => {$crate::WordleGuess::Present};
    (X) => {$crate::WordleGuess::Incorrect};
    ($($c:tt)+) => {[
        $(guess_result!($c)),+
    ]}
}

impl<S> WordleAnswer<S>
where
    S: Borrow<str>,
{
    /// Creates a new Wordle answer.
    ///
    /// # Examples
    ///
    /// Basic usage:
    /// ```
    /// # use wordle::WordleAnswer;
    /// let word = "TRACE";
    /// let answer = WordleAnswer::new(word);
    /// ```
    #[inline]
    #[must_use]
    pub fn new(word: S) -> Self {
        let mut letter_counts = [0; 26];
        word.borrow()
            .chars()
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
    /// # use wordle::WordleAnswer;
    /// let word = "TRACE";
    /// let answer = WordleAnswer::new(word);
    /// assert_eq!(answer.get_word(), word);
    /// ```
    #[inline]
    #[must_use]
    pub fn get_word(&self) -> &str {
        self.word.borrow()
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
    /// # use wordle::{guess_result, WordleAnswer};
    /// let answer = WordleAnswer::new("TRACE");
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
    /// # use wordle::{guess_result, WordleAnswer};
    /// let answer = WordleAnswer::new("AABBB");
    /// let some_yellow = answer.check_guess("CAACC");
    /// assert_eq!(some_yellow, guess_result![X G Y X X]);
    ///
    /// let answer = WordleAnswer::new("AZZAZ");
    /// let some_green = answer.check_guess("AAABB");
    /// assert_eq!(some_green, guess_result![G Y X X X]);
    ///
    /// let answer = WordleAnswer::new("BACCC");
    /// let no_yellow = answer.check_guess("AADDD");
    /// assert_eq!(no_yellow, guess_result![X G X X X]);
    /// ```
    #[must_use]
    pub fn check_guess(&self, guess: &str) -> [WordleGuess; 5] {
        let mut colors = [WordleGuess::Incorrect; 5];
        let mut letter_counts = self.letter_counts;

        /* first check for green letters */
        for (i, (a, g)) in self.word.borrow().chars().zip(guess.chars()).enumerate() {
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
/// `WIN_MESSAGES[n - 1]` is the message
/// that should be displayed.
pub const WIN_MESSAGES: [&str; 6] = [
    "Genius",
    "Magnificent",
    "Impressive",
    "Splendid",
    "Great",
    "Phew",
];

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn all_green() {
        let answer = WordleAnswer::new("ABCDE");
        assert_eq!(answer.check_guess("ABCDE"), guess_result![G G G G G]);
    }

    #[test]
    fn all_yellow() {
        let answer = WordleAnswer::new("EABCD");
        assert_eq!(answer.check_guess("ABCDE"), guess_result![Y Y Y Y Y]);
    }

    #[test]
    fn all_black() {
        let answer = WordleAnswer::new("FGHIJ");
        assert_eq!(answer.check_guess("ABCDE"), guess_result![X X X X X]);
    }

    #[test]
    fn repeat_green() {
        let answer = WordleAnswer::new("AABBB");
        assert_eq!(answer.check_guess("AACCC"), guess_result![G G X X X]);
    }

    #[test]
    fn repeat_yellow() {
        let answer = WordleAnswer::new("AABBB");
        assert_eq!(answer.check_guess("CCAAC"), guess_result![X X Y Y X]);
    }

    #[test]
    fn repeat_some_green() {
        let answer = WordleAnswer::new("AABBB");
        assert_eq!(answer.check_guess("CAACC"), guess_result![X G Y X X]);
    }

    #[test]
    fn repeat_some_yellow() {
        let answer = WordleAnswer::new("AZZAZ");
        assert_eq!(answer.check_guess("AAABB"), guess_result![G Y X X X]);
    }

    #[test]
    fn green_no_yellow() {
        let answer = WordleAnswer::new("BACCC");
        assert_eq!(answer.check_guess("AADDD"), guess_result![X G X X X]);
    }
}
