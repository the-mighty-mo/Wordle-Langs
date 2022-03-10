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
///
/// - Correct = Green: the letter is in the word at that position
/// - Present = Yellow: the letter is in the word, but not at that position
/// - Incorrect = Gray: there are no more instances of the letter in the word
#[derive(Debug, Copy, Clone, PartialEq, Eq)]
enum WordleGuess {
    Correct,
    Present,
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

/// Stores information about a game of Wordle.
///
/// A game of Worlde has a target word. The guessing algorithm
/// uses preprocessing so it can run in linear time. This results
/// in an array containing the counts of each letter.
pub struct WordleGame {
    word: String,
    letter_counts: [u8; 26],
}

impl WordleGame {
    const WIN_MESSAGES: [&'static str; 6] = [
        "Genius",
        "Magnificent",
        "Impressive",
        "Splendid",
        "Great",
        "Phew",
    ];

    /// Creates a new game of Wordle.
    ///
    /// # Examples
    ///
    /// Basic usage:
    /// ```ignore
    /// # use wordle_rs::wordle::WordleGame;
    /// let word = String::from("TRACE");
    /// let wordle_game = WordleGame::new(word);
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
    /// ```ignore
    /// # use std::collections::HashSet;
    /// # use wordle_rs::{
    /// #     players::PlayerInfo,
    /// #     wordle::WordleGame
    /// # };
    /// # fn read_dictionary(filename: &str) -> HashSet<String> {
    /// # HashSet::new()
    /// # }
    /// let word = String::from("TRACE");
    /// let wordle_game = WordleGame::new(word);
    /// let mut player = PlayerInfo::from_file("user.txt");
    /// let dictionary: HashSet<String> =
    ///     read_dictionary("dictionary.txt");
    ///
    /// // runs one game of Wordle where the answer is "TRACE"
    /// wordle_game.run(&mut player, &dictionary);
    /// ```
    pub fn run(&self, player: &mut PlayerInfo, dictionary: &HashSet<String>) {
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

            let colors = self.guess(&guess);
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
                player.add_won_word(self.word.clone(), i as usize);
                print!("{}! ", Self::WIN_MESSAGES[i as usize - 1]);
            }
            None => {
                player.add_lost_word(self.word.clone());
                print!("Too bad! ");
            }
            Some(_) => return,
        }
        println!("The word was: {}", self.word);
        println!();
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
    /// ```ignore
    /// # use wordle_rs::wordle::WordleGame;
    /// let wordle_game = WordleGame::new(String::from("TRACE"));
    ///
    /// let all_correct = wordle_game.guess("TRACE");
    /// let all_present = wordle_game.guess("ETRAC");
    /// let all_incorrect = wordle_game.guess("BLIND");
    /// ```
    /// For the following examples,
    /// X = Incorrect, Y = Present, G = Correct:
    /// ```ignore
    /// # use wordle_rs::wordle::WordleGame;
    /// let wordle_game = WordleGame::new(String::from("AABBB"));
    /// // Returns: [X G Y X X]
    /// let some_yellow = wordle_game.guess("CAACC");
    ///
    /// let wordle_game = WordleGame::new(String::from("AZZAZ"));
    /// // Returns: [G Y X X X]
    /// let some_green = wordle_game.guess("AAABB");
    ///
    /// let wordle_game = WordleGame::new(String::from("BACCC"));
    /// // Returns: [X G X X X]
    /// let no_yellow = wordle_game.guess("AADDD");
    /// ```
    fn guess(&self, guess: &str) -> [WordleGuess; 5] {
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

#[cfg(test)]
mod test {
    mod wordle {
        use crate::wordle::{WordleGame, WordleGuess};

        macro_rules! mask {
            (G) => {WordleGuess::Correct};
            (Y) => {WordleGuess::Present};
            (X) => {WordleGuess::Incorrect};
            ($($c:tt)+) => {[
                $(mask!($c)),+
            ]}
        }

        #[test]
        fn all_green() {
            let game = WordleGame::new("ABCDE".to_owned());
            assert_eq!(game.guess("ABCDE"), mask![G G G G G]);
        }

        #[test]
        fn all_yellow() {
            let game = WordleGame::new("EABCD".to_owned());
            assert_eq!(game.guess("ABCDE"), mask![Y Y Y Y Y]);
        }

        #[test]
        fn all_black() {
            let game = WordleGame::new("FGHIJ".to_owned());
            assert_eq!(game.guess("ABCDE"), mask![X X X X X]);
        }

        #[test]
        fn repeat_green() {
            let game = WordleGame::new("AABBB".to_owned());
            assert_eq!(game.guess("AACCC"), mask![G G X X X]);
        }

        #[test]
        fn repeat_yellow() {
            let game = WordleGame::new("AABBB".to_owned());
            assert_eq!(game.guess("CCAAC"), mask![X X Y Y X]);
        }

        #[test]
        fn repeat_some_green() {
            let game = WordleGame::new("AABBB".to_owned());
            assert_eq!(game.guess("CAACC"), mask![X G Y X X]);
        }

        #[test]
        fn repeat_some_yellow() {
            let game = WordleGame::new("AZZAZ".to_owned());
            assert_eq!(game.guess("AAABB"), mask![G Y X X X]);
        }

        #[test]
        fn green_no_yellow() {
            let game = WordleGame::new("BACCC".to_owned());
            assert_eq!(game.guess("AADDD"), mask![X G X X X]);
        }
    }
}
