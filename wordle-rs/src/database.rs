//! This module provides methods and structs to parse
//! and manage databases used by the wordle-rs crate.
//!
//! Author: Benjamin Hall

use std::{collections::HashSet, convert::identity, hash::Hash};

/// Stores information about an entry in a database.
///
/// Each database entry contains two portions: the name
/// of the field, and the value of the data.
pub struct DatabaseEntry<T> {
    pub name: String,
    pub value: T,
}

/// Delimiter between the field name and data for database entries
const DELIM: &str = ": ";

impl<T> DatabaseEntry<T> {
    /// Creates a simple database entry from a line of text.
    ///
    /// The text will be split between field name and data
    /// on the ": " delimiter. If the delimiter is not found,
    /// then this function returns None.
    ///
    /// # Examples
    ///
    /// Basic usage:
    /// ```
    /// // Field name: "String Test"
    /// // Data: "data"
    /// let str_entry = DatabaseEntry::from_line("String Test: data", identity);
    /// // Field name: "Int Test"
    /// // Data: 3
    /// let int_entry = DatabaseEntry::from_line("Int Test: 3", str::parse::<i32>);
    /// ```
    pub fn from_line<'a>(line: &'a str, string_to_t: fn(&'a str) -> T) -> Option<DatabaseEntry<T>> {
        let split_str = line.split_once(DELIM);
        split_str.map(|(key, value)| DatabaseEntry {
            name: key.to_owned(),
            value: string_to_t(value),
        })
    }

    /// Creates a database entry from a line of text where
    /// the data field is a list of elements.
    ///
    /// The text will be split between field name and data
    /// on the ": " delimiter. If the delimiter is not found,
    /// then this function returns None. From there, elements
    /// will be separated by the "," delimiter and added to
    /// a vector.
    ///
    /// # Examples
    ///
    /// Basic usage:
    /// ```
    /// // Field name: "String Test"
    /// // Data: vec!["data1", "data2"]
    /// let str_list_entry = DatabaseEntry::from_list("String Test: data1,data2", identity);
    /// // Field name: "Int Test"
    /// // Data: vec![4, 3, 4, 5]
    /// let int_list_entry = DatabaseEntry::from_list("Int Test: 4,3,4,5", str::parse::<i32>);
    /// ```
    pub fn from_list<'a>(
        line: &'a str,
        string_to_t: fn(&'a str) -> T,
    ) -> Option<DatabaseEntry<Vec<T>>> {
        let parsed_row = DatabaseEntry::from_line(line, identity);
        parsed_row.map(|parsed_row| {
            let items = parsed_row.value.split(',').map(string_to_t).collect();

            DatabaseEntry {
                name: parsed_row.name,
                value: items,
            }
        })
    }

    /// Creates a database entry from a line of text where
    /// the data field is a set of unique elements.
    ///
    /// The text will be split between field name and data
    /// on the ": " delimiter. If the delimiter is not found,
    /// then this function returns None. From there, elements
    /// will be separated by the "," delimiter and added to
    /// a HashSet.
    ///
    /// # Examples
    ///
    /// Basic usage:
    /// ```
    /// // Field name: "String Test"
    /// // Data: HashSet["data1", "data2"]
    /// let str_set_entry = DatabaseEntry::from_set("String Test: data1,data2", identity);
    /// // Field name: "Int Test"
    /// // Data: HashSet[6, 3, 4, 5]
    /// let int_set_entry = DatabaseEntry::from_set("Int Test: 6,3,4,5", str::parse::<i32>);
    /// ```
    pub fn from_set<'a>(
        line: &'a str,
        string_to_t: fn(&'a str) -> T,
    ) -> Option<DatabaseEntry<HashSet<T>>>
    where
        T: Eq,
        T: Hash,
    {
        let parsed_row = DatabaseEntry::from_line(line, identity);
        parsed_row.map(|parsed_row| {
            let items = parsed_row.value.split(',').map(string_to_t).collect();

            DatabaseEntry {
                name: parsed_row.name,
                value: items,
            }
        })
    }
}
