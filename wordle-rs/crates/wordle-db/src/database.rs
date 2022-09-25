//! This module provides methods and structs to parse
//! and manage databases used by the wordle-rs crate.
//!
//! Author: Benjamin Hall

use std::{convert::identity, marker::PhantomData};

/// Stores information about an entry in a database.
///
/// Each database entry contains two portions: the key
/// of the field, and the value of the data.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct Entry<S, T, V> {
    pub key: S,
    pub value: T,
    inner: PhantomData<V>,
}

/// Delimiter between the field name and data for database entries
const DELIM: &str = ": ";

impl<S, T, V> Entry<S, T, V> {
    /// Creates a new database entry with the
    /// given key and value.
    ///
    /// # Examples
    ///
    /// Basic usage:
    /// ```ignore
    /// # use wordle::players::database::Entry;
    /// let entry = Entry::<_, _, ()>::new("Key", "value");
    /// assert_eq!(entry.key, "Key");
    /// assert_eq!(entry.value, "value");
    /// ```
    #[inline]
    #[must_use]
    const fn new(key: S, value: T) -> Self {
        Self {
            key,
            value,
            inner: PhantomData,
        }
    }
}

impl<'a, S, T> Entry<S, T, ()>
where
    S: From<&'a str>,
{
    /// Creates a simple database entry from a line of text.
    ///
    /// The text will be split between field name and data
    /// on the ": " delimiter. If the delimiter is not found,
    /// then this function returns None.
    ///
    /// # Examples
    ///
    /// Basic usage:
    /// ```ignore
    /// # use std::convert::identity;
    /// # use wordle::players::database::Entry;
    /// let str_entry = Entry::from_line("String Test: data", identity);
    /// assert_eq!(
    ///     str_entry.unwrap(),
    ///     Entry::new("String Test", "data")
    /// );
    /// ```
    #[must_use]
    pub fn from_line(line: &'a str, string_to_t: impl Fn(&'a str) -> T) -> Option<Self> {
        let split_str = line.split_once(DELIM);
        split_str.map(|(key, value)| Self::new(key.into(), string_to_t(value)))
    }

    /// Creates a simple database entry from a line of text where
    /// parsing the data entry has the potential to fail.
    ///
    /// The text will be split between field name and data
    /// on the ": " delimiter. If the delimiter is not found,
    /// then this function returns None.
    ///
    /// If this function fails to parse the data section, it
    /// will return the error to the caller.
    ///
    /// # Examples
    ///
    /// Basic usage:
    /// ```ignore
    /// # use wordle::players::database::Entry;
    /// # fn main() -> Result<(), std::num::ParseIntError> {
    /// let int_entry = Entry::try_from_line("Int Test: 3", str::parse::<i32>)?;
    /// assert_eq!(
    ///     int_entry.unwrap(),
    ///     Entry::new("Int Test", 3)
    /// );
    /// # Ok(())
    /// # }
    /// ```
    pub fn try_from_line<E>(
        line: &'a str,
        string_to_t: impl Fn(&'a str) -> Result<T, E>,
    ) -> Result<Option<Self>, E> {
        let split_str = line.split_once(DELIM);
        split_str
            .map(|(key, value)| string_to_t(value).map(|value| Self::new(key.into(), value)))
            .map_or(Ok(None), |r| r.map(Some))
    }
}

impl<'a, S, T, V> Entry<S, T, V>
where
    S: From<&'a str>,
    T: FromIterator<V>,
{
    /// Creates a database entry from a line of text where
    /// the data field is a collection of elements.
    ///
    /// The text will be split between field name and data
    /// on the ": " delimiter. If the delimiter is not found,
    /// then this function returns None. From there, elements
    /// will be separated by the "," delimiter and added to
    /// a collection.
    ///
    /// # Examples
    ///
    /// Basic usage:
    /// ```ignore
    /// # use std::{
    /// #     collections::HashSet,
    /// #     convert::identity,
    /// # };
    /// # use wordle::players::database::Entry;
    /// let str_vec_entry =
    ///     Entry::from_collection("String Test: data1,data2", identity);
    /// assert_eq!(
    ///     str_vec_entry.unwrap(),
    ///     Entry::new("String Test", vec!["data1", "data2"])
    /// );
    /// let str_set_entry =
    ///     Entry::from_collection("String Test: data1,data2", identity);
    /// assert_eq!(
    ///     str_set_entry.unwrap(),
    ///     Entry::new("String Test", HashSet::from(["data1", "data2"]))
    /// );
    /// ```
    #[must_use]
    pub fn from_collection(line: &'a str, string_to_v: impl Fn(&'a str) -> V) -> Option<Self> {
        let parsed_row = Entry::<&str, _, _>::from_line(line, identity);
        parsed_row.map(|parsed_row| {
            let items = parsed_row.value.split(',').map(string_to_v).collect();

            Self::new(parsed_row.key.into(), items)
        })
    }

    /// Creates a database entry from a line of text where
    /// the data field is a collection of elements, and
    /// parsing the data entry has the potential to fail.
    ///
    /// The text will be split between field name and data
    /// on the ": " delimiter. If the delimiter is not found,
    /// then this function returns None. From there, elements
    /// will be separated by the "," delimiter and added to
    /// a collection.
    ///
    /// If at any point in time this function fails to parse
    /// an element of the data section, it will return the error
    /// to the caller.
    ///
    /// # Examples
    ///
    /// Basic usage:
    /// ```ignore
    /// # use std::collections::HashSet;
    /// # use wordle::players::database::Entry;
    /// # fn main() -> Result<(), std::num::ParseIntError> {
    /// let int_vec_entry =
    ///     Entry::try_from_collection("Int Test: 4,3,4,5", str::parse::<i32>)?;
    /// assert_eq!(
    ///     int_vec_entry.unwrap(),
    ///     Entry::new("Int Test", vec![4, 3, 4, 5])
    /// );
    /// let int_set_entry =
    ///     Entry::try_from_collection("Int Test: 6,3,4,5", str::parse::<i32>)?;
    /// assert_eq!(
    ///     int_set_entry.unwrap(),
    ///     Entry::new("Int Test", HashSet::from([6, 3, 4, 5]))
    /// );
    /// # Ok(())
    /// # }
    /// ```
    pub fn try_from_collection<E>(
        line: &'a str,
        string_to_v: impl Fn(&'a str) -> Result<V, E>,
    ) -> Result<Option<Self>, E> {
        let parsed_row = Entry::<&str, _, _>::from_line(line, identity);
        parsed_row
            .map(|parsed_row| {
                parsed_row
                    .value
                    .split(',')
                    .map(string_to_v)
                    .collect::<Result<_, _>>()
                    .map(|items| Self::new(parsed_row.key.into(), items))
            })
            .map_or(Ok(None), |r| r.map(Some))
    }
}
