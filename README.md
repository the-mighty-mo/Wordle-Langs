# Wordle-Langs

[![CodeFactor](https://www.codefactor.io/repository/github/the-mighty-mo/wordle-langs/badge)](https://www.codefactor.io/repository/github/the-mighty-mo/wordle-langs)
[![Java](https://github.com/the-mighty-mo/Wordle-Langs/actions/workflows/java.yml/badge.svg)](https://github.com/the-mighty-mo/Wordle-Langs/actions/workflows/java.yml)
[![Rust](https://github.com/the-mighty-mo/Wordle-Langs/actions/workflows/rust.yml/badge.svg)](https://github.com/the-mighty-mo/Wordle-Langs/actions/workflows/rust.yml)
[![C](https://github.com/the-mighty-mo/Wordle-Langs/actions/workflows/c.yml/badge.svg)](https://github.com/the-mighty-mo/Wordle-Langs/actions/workflows/c.yml)

This repository contains command-line implementations of Wordle in various programming languages.

- Wordle_Java - Java 11
  - To build, from the Wordle_Java folder:  
    `javac -d build/ src/*.java; jar cfve Wordle.jar WordleProgram -C build/ .`
- wordle-rs - Rust (2021 edition)
  - To build, from the wordle-rs folder:  
    `cargo build --release`
- CWordle - C (C11)
  - To build on Windows, in a Developer Command Prompt from the CWordle folder:  
    `mkdir build\obj & cl /std:c11 /O2 src/c/*.c src/c/collections/*.c src/c/console_app/*.c src/c/players/*.c /Isrc/include/ /Fobuild/obj/ /Febuild/CWordle`
  - To build on Linux, from the CWordle folder:  
    `gcc -O2 -std=gnu11 -o CWordle src/c/*.c src/c/**/*.c -Isrc/include -lm; strip -s -R .comment -w -R .note* CWordle`
