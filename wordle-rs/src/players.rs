//! This module provides methods and structs to manage information
//! about a Wordle player. Methods are also provided to save the
//! player to and load a player from a database file.
//!
//! Author: Benjamin Hall

use core::fmt;
use std::collections::HashSet;

use std::fmt::Write;
use std::fs::File;
use std::io::{self, BufReader, BufWriter, Read};

use crate::database::DatabaseEntry;

/// Contains information about a Wordle player.
///
/// A player has a:
/// - username
/// - list of words played
/// - guess distribution
/// - maximum win streak
/// - current win streak
pub struct PlayerInfo {
    username: String,
    words_played: HashSet<String>,
    num_guesses: [usize; 6],
    max_win_streak: usize,
    cur_win_streak: usize,
}

impl fmt::Display for PlayerInfo {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        writeln!(f, "Username: {}", self.username)?;
        writeln!(
            f,
            "Words Played: {}",
            self.words_played
                .iter()
                .cloned()
                .collect::<Vec<_>>()
                .join(",")
        )?;
        writeln!(
            f,
            "Number of Guesses: {}",
            self.num_guesses.map(|i| i.to_string()).join(",")
        )?;
        writeln!(f, "Maximum Win Streak: {}", self.max_win_streak)?;
        writeln!(f, "Current Win Streak: {}", self.cur_win_streak)
    }
}

impl PlayerInfo {
    /// Initializes data for a new player.
    ///
    /// # Examples
    ///
    /// Basic usage:
    /// ```
    /// let player = PlayerInfo::new("user");
    /// ```
    pub fn new(username: String) -> Self {
        PlayerInfo::load(username, HashSet::new(), [0; 6], 0, 0)
    }

    /// Loads data for an existing player.
    ///
    /// # Examples
    ///
    /// Basic usage:
    /// ```
    /// let words_played = HashSet::new();
    /// words_played.insert(String::from("TRACE"));
    ///
    /// let num_guesses = [0; 6];
    /// num_guesses[4 - 1] += 1;
    ///
    /// let max_win_streak = 1;
    /// let cur_win_streak = 1;
    ///
    /// let player = PlayerInfo::new(
    ///     "user",
    ///     words_played,
    ///     num_guesses,
    ///     max_win_streak,
    ///     cur_win_streak
    /// );
    /// ```
    pub fn load(
        username: String,
        words_played: HashSet<String>,
        num_guesses: [usize; 6],
        max_win_streak: usize,
        cur_win_streak: usize,
    ) -> Self {
        PlayerInfo {
            username,
            words_played,
            num_guesses,
            max_win_streak,
            cur_win_streak,
        }
    }

    /// Gets the username of this player.
    ///
    /// # Examples
    ///
    /// Basic usage:
    /// ```
    /// let player = PlayerInfo::new("user");
    /// // prints "user"
    /// println!("{}", player.get_username());
    /// ```
    pub fn get_username(&self) -> &str {
        self.username.as_str()
    }

    /// Gets a random word this player has not yet played.
    ///
    /// # Examples:
    ///
    /// Basic usage:
    /// ```
    /// let player = PlayerInfo::new("user");
    /// let dictionary: HashSet<String> =
    ///     read_dictionary("dictionary.txt");
    /// let word = player.get_random_word(&dictionary);
    /// ```
    pub fn get_random_word(&self, dictionary: &HashSet<String>) -> String {
        let unplayed_words_cnt = dictionary.len() - self.words_played.len();
        let random_word_idx = fastrand::usize(0..unplayed_words_cnt);
        dictionary.iter().nth(random_word_idx).unwrap().to_owned()
    }

    /// Adds a word the player has successfully guessed to their database.
    ///
    /// This function increments the current win streak, adds
    /// the guess to the list of words played and to the guess
    /// distribution, and updates the max win streak if appropriate.
    ///
    /// # Examples:
    ///
    /// Basic usage:
    /// ```
    /// let player = PlayerInfo::new("user");
    /// // player got TRACE in 3 guesses
    /// player.add_won_word(String::from("TRACE"), 3);
    /// ```
    pub fn add_won_word(&mut self, word: String, num_guesses: usize) {
        self.words_played.insert(word);
        self.num_guesses[num_guesses - 1] += 1;
        self.cur_win_streak += 1;
        self.max_win_streak = std::cmp::max(self.max_win_streak, self.cur_win_streak);
    }

    /// Adds a word the player has failed to guess to their database.
    ///
    /// This function resets the current win streak to 0 and adds
    /// the guess to the list of words played. The number of guesses
    /// is not added to the player's guess distribution.
    ///
    /// # Examples:
    ///
    /// Basic usage:
    /// ```
    /// let player = PlayerInfo::new("user");
    /// // player could not guess BEBOP within 6 guesses
    /// player.add_lost_word(String::from("BEBOP"));
    /// ```
    pub fn add_lost_word(&mut self, word: String) {
        self.words_played.insert(word);
        self.cur_win_streak = 0;
    }

    /// Returns a string with formated player statistics.
    ///
    /// Player statistics consist of:
    /// - Number of words played
    /// - Win rate
    /// - Current and max win streak
    /// - Guess distribution
    ///
    /// # Examples
    ///
    /// Basic usage:
    /// ```
    /// let player = PlayerInfo::new("user");
    /// println!("{}", player.get_stats());
    /// ```
    pub fn get_stats(&self) -> String {
        let mut stats = String::new();
        writeln!(stats, "Number of Words Played: {}", self.words_played.len()).unwrap();

        let win_rate = if self.words_played.is_empty() {
            0
        } else {
            (100.0 * self.num_guesses.iter().sum::<usize>() as f64 / self.words_played.len() as f64)
                .round() as u32
        };
        writeln!(stats, "Win Rate: {win_rate}%").unwrap();

        writeln!(stats, "Current Win Streak: {}", self.cur_win_streak).unwrap();
        writeln!(stats, "Maximum Win Streak: {}", self.max_win_streak).unwrap();

        writeln!(stats, "Guess Distribution:").unwrap();
        let max_num_guesses = self.num_guesses.iter().max();

        /* print the guess distribution with bars, max 12 */
        let bar_factor = match max_num_guesses {
            Some(max_num_guesses) => 12.0 / *max_num_guesses as f64,
            None => 0.0,
        };

        for (i, num_guess) in self.num_guesses.iter().enumerate() {
            let num_bars = (bar_factor * *num_guess as f64).round() as usize;
            let bars = "=".repeat(num_bars);
            writeln!(stats, "{}: {bars} {num_guess}", i + 1).unwrap();
        }

        stats.trim().to_owned()
    }

    /// Writes this player's data to a file.
    ///
    /// # Examples
    ///
    /// Basic usage:
    /// ```
    /// let player = PlayerInfo::new("user");
    /// player.write_to_file("user.txt")?;
    /// ```
    pub fn write_to_file(&self, filename: &str) -> io::Result<()> {
        let file = File::create(filename)?;
        let mut writer = BufWriter::new(file);
        use std::io::Write;
        writer.write_all(self.to_string().as_bytes())
    }

    /// Reads a player's info from a file.
    ///
    /// All errors with reading the file, including any errors
    /// with parsing data, is propogated up to the caller.
    ///
    /// # Examples
    ///
    /// Basic usage:
    /// ```
    /// let player = PlayerInfo::from_file("user.txt")?;
    /// ```
    pub fn from_file(filename: &str) -> io::Result<Option<Self>> {
        let bad_data_err = || {
            io::Error::new(
                io::ErrorKind::InvalidData,
                format!("Error: corrupt player database file: {}", filename),
            )
        };

        let file = File::open(filename);
        let file = match file {
            Ok(file) => file,
            Err(_) => return Ok(None),
        };

        let mut file_contents = String::new();
        let mut file_reader = BufReader::new(file);
        file_reader.read_to_string(&mut file_contents)?;
        /* read all the lines in the file */
        let lines_in_file: Vec<&str> = file_contents.lines().collect();

        if lines_in_file.len() != 5 {
            /* corrupt database file */
            return Err(bad_data_err());
        }

        /* parse the lines in the file */
        let username = DatabaseEntry::from_line(lines_in_file[0], str::to_owned);
        let words_played = DatabaseEntry::from_set(lines_in_file[1], str::to_owned);
        let num_guesses_list = DatabaseEntry::from_list(lines_in_file[2], str::parse::<usize>);
        let max_win_streak = DatabaseEntry::from_line(lines_in_file[3], str::parse::<usize>);
        let cur_win_streak = DatabaseEntry::from_line(lines_in_file[3], str::parse::<usize>);

        /* make sure no data was missing */
        if username.is_none()
            || words_played.is_none()
            || num_guesses_list.is_none()
            || max_win_streak.is_none()
            || cur_win_streak.is_none()
        {
            return Err(bad_data_err());
        }
        let username = username.unwrap();
        let words_played = words_played.unwrap();
        let num_guesses_list = num_guesses_list.unwrap();
        let max_win_streak = max_win_streak.unwrap();
        let cur_win_streak = cur_win_streak.unwrap();

        /* propogate any str::parse() errors to the caller */
        let num_guesses_list = DatabaseEntry {
            name: num_guesses_list.name,
            value: num_guesses_list
                .value
                .into_iter()
                .map(|r| r.map_err(|_| bad_data_err()))
                .collect::<Result<Vec<_>, _>>()?,
        };
        let max_win_streak = DatabaseEntry {
            name: max_win_streak.name,
            value: max_win_streak.value.map_err(|_| bad_data_err())?,
        };
        let cur_win_streak = DatabaseEntry {
            name: cur_win_streak.name,
            value: cur_win_streak.value.map_err(|_| bad_data_err())?,
        };

        /* parse the number of guesses into an array */
        let mut num_guesses = [0; 6];
        num_guesses_list
            .value
            .iter()
            .take(num_guesses.len())
            .enumerate()
            .for_each(|(i, num_guess)| num_guesses[i] = *num_guess);

        let player = PlayerInfo::load(
            username.value,
            words_played.value,
            num_guesses,
            max_win_streak.value,
            cur_win_streak.value,
        );
        Ok(Some(player))
    }
}
