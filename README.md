# Wordle-Langs

[![CodeFactor](https://www.codefactor.io/repository/github/the-mighty-mo/wordle-langs/badge)](https://www.codefactor.io/repository/github/the-mighty-mo/wordle-langs)
[![Java](https://github.com/the-mighty-mo/Wordle-Langs/actions/workflows/java.yml/badge.svg)](https://github.com/the-mighty-mo/Wordle-Langs/actions/workflows/java.yml)
[![Rust](https://github.com/the-mighty-mo/Wordle-Langs/actions/workflows/rust.yml/badge.svg)](https://github.com/the-mighty-mo/Wordle-Langs/actions/workflows/rust.yml)

This repository contains command-line implementations of Wordle in various programming languages.

- Wordle_Java - Java 11
  - To build, from the Wordle_Java folder:  
    `javac -d build/ src/*.java; jar cfve Wordle.jar WordleProgram -C build/ .`
- wordle-rs - Rust (2021 edition)
  - To build, from the wordle-rs folder:  
    `cargo build --release`
