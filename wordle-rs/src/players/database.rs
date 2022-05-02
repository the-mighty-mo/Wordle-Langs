//! This module provides methods and structs to parse
//! and manage databases used by the wordle-rs crate.
//!
//! Author: Benjamin Hall

use std::{borrow::Borrow, convert::identity, marker::PhantomData};

/// Stores information about an entry in a database.
///
/// Each database entry contains two portions: the name
/// of the field, and the value of the data.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct DatabaseEntry<T, V, S>
where
    S: Borrow<str>,
{
    pub name: S,
    pub value: T,
    inner: PhantomData<V>,
}

/// Delimiter between the field name and data for database entries
const DELIM: &str = ": ";

impl<T, V, S> DatabaseEntry<T, V, S>
where
    S: Borrow<str>,
{
    /// Creates a new database entry with the
    /// given name and value.
    ///
    /// # Examples
    ///
    /// Basic usage:
    /// ```ignore
    /// # use wordle::players::database::DatabaseEntry;
    /// let entry = DatabaseEntry::<_, (), _>::new("Name", "value");
    /// assert_eq!(entry.name, "Name");
    /// assert_eq!(entry.value, "value");
    /// ```
    #[inline]
    #[must_use]
    fn new(name: S, value: T) -> Self {
        Self {
            name,
            value,
            inner: PhantomData,
        }
    }
}

impl<'a, T, S> DatabaseEntry<T, (), S>
where
    S: Borrow<str> + From<&'a str>,
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
    /// # use std::{convert::identity, marker::PhantomData};
    /// # use wordle::players::database::DatabaseEntry;
    /// let str_entry = DatabaseEntry::from_line("String Test: data", identity);
    /// assert_eq!(
    ///     str_entry.unwrap(),
    ///     DatabaseEntry::new("String Test", "data")
    /// );
    /// ```
    #[must_use]
    pub fn from_line<F>(line: &'a str, string_to_t: F) -> Option<Self>
    where
        F: Fn(&'a str) -> T,
    {
        let split_str = line.split_once(DELIM);
        split_str.map(|(key, value)| DatabaseEntry::new(key.into(), string_to_t(value)))
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
    /// # use std::marker::PhantomData;
    /// # use wordle::players::database::DatabaseEntry;
    /// # fn main() -> Result<(), std::num::ParseIntError> {
    /// let int_entry = DatabaseEntry::try_from_line("Int Test: 3", str::parse::<i32>)?;
    /// assert_eq!(
    ///     int_entry.unwrap(),
    ///     DatabaseEntry::new("Int Test", 3)
    /// );
    /// # Ok(())
    /// # }
    /// ```
    pub fn try_from_line<F, E>(line: &'a str, string_to_t: F) -> Result<Option<Self>, E>
    where
        F: Fn(&'a str) -> Result<T, E>,
    {
        let split_str = line.split_once(DELIM);
        split_str
            .map(|(key, value)| {
                let value = string_to_t(value)?;
                Ok(DatabaseEntry::new(key.into(), value))
            })
            .map_or(Ok(None), |r| r.map(Some))
    }
}

impl<'a, T, V, S> DatabaseEntry<T, V, S>
where
    T: FromIterator<V>,
    S: Borrow<str> + From<&'a str>,
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
    /// #     marker::PhantomData,
    /// # };
    /// # use wordle::players::database::DatabaseEntry;
    /// let str_vec_entry =
    ///     DatabaseEntry::from_collection("String Test: data1,data2", identity);
    /// assert_eq!(
    ///     str_vec_entry.unwrap(),
    ///     DatabaseEntry::new("String Test", vec!["data1", "data2"])
    /// );
    /// let str_set_entry =
    ///     DatabaseEntry::from_collection("String Test: data1,data2", identity);
    /// assert_eq!(
    ///     str_set_entry.unwrap(),
    ///     DatabaseEntry::new("String Test", HashSet::from(["data1", "data2"]))
    /// );
    /// ```
    #[must_use]
    pub fn from_collection<F>(line: &'a str, string_to_v: F) -> Option<Self>
    where
        F: Fn(&'a str) -> V,
    {
        let parsed_row = DatabaseEntry::<_, _, &str>::from_line(line, identity);
        parsed_row.map(|parsed_row| {
            let items = parsed_row.value.split(',').map(string_to_v).collect();

            DatabaseEntry::new(parsed_row.name.into(), items)
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
    /// # use std::{collections::HashSet, marker::PhantomData};
    /// # use wordle::players::database::DatabaseEntry;
    /// # fn main() -> Result<(), std::num::ParseIntError> {
    /// let int_vec_entry =
    ///     DatabaseEntry::try_from_collection("Int Test: 4,3,4,5", str::parse::<i32>)?;
    /// assert_eq!(
    ///     int_vec_entry.unwrap(),
    ///     DatabaseEntry::new("Int Test", vec![4, 3, 4, 5])
    /// );
    /// let int_set_entry =
    ///     DatabaseEntry::try_from_collection("Int Test: 6,3,4,5", str::parse::<i32>)?;
    /// assert_eq!(
    ///     int_set_entry.unwrap(),
    ///     DatabaseEntry::new("Int Test", HashSet::from([6, 3, 4, 5]))
    /// );
    /// # Ok(())
    /// # }
    /// ```
    pub fn try_from_collection<F, E>(line: &'a str, string_to_v: F) -> Result<Option<Self>, E>
    where
        F: Fn(&'a str) -> Result<V, E>,
    {
        let parsed_row = DatabaseEntry::<_, _, &str>::from_line(line, identity);
        parsed_row
            .map(|parsed_row| {
                let items = parsed_row
                    .value
                    .split(',')
                    .map(string_to_v)
                    .collect::<Result<_, _>>()?;

                Ok(DatabaseEntry::new(parsed_row.name.into(), items))
            })
            .map_or(Ok(None), |r| r.map(Some))
    }
}
