# Wordle-Langs

[![CodeFactor](https://www.codefactor.io/repository/github/the-mighty-mo/wordle-langs/badge)](https://www.codefactor.io/repository/github/the-mighty-mo/wordle-langs)
[![Java](https://github.com/the-mighty-mo/Wordle-Langs/actions/workflows/java.yml/badge.svg)](https://github.com/the-mighty-mo/Wordle-Langs/actions/workflows/java.yml)
[![Rust](https://github.com/the-mighty-mo/Wordle-Langs/actions/workflows/rust.yml/badge.svg)](https://github.com/the-mighty-mo/Wordle-Langs/actions/workflows/rust.yml)
[![C](https://github.com/the-mighty-mo/Wordle-Langs/actions/workflows/c.yml/badge.svg)](https://github.com/the-mighty-mo/Wordle-Langs/actions/workflows/c.yml)
[![C++](https://github.com/the-mighty-mo/Wordle-Langs/actions/workflows/cpp.yml/badge.svg)](https://github.com/the-mighty-mo/Wordle-Langs/actions/workflows/cpp.yml)
[![C#](https://github.com/the-mighty-mo/Wordle-Langs/actions/workflows/c%23.yml/badge.svg)](https://github.com/the-mighty-mo/Wordle-Langs/actions/workflows/c%23.yml)

This repository contains command-line implementations of Wordle in various programming languages.

- Wordle_Java - Java 11
  - To build, from the Wordle_Java folder:  
    `javac -d build/ src/*.java; jar cfve Wordle.jar WordleProgram -C build/ .`
- wordle-rs - Rust 2021 edition
  - To build, from the wordle-rs folder:  
    `cargo build --release --all-features`
- CWordle - C11
  - To build on Windows, in a Developer Command Prompt from the CWordle folder:  
    `mkdir build\obj & cl /std:c11 /O2 src/*.c src/collections/*.c src/console_app/*.c src/players/*.c /Iinclude/ /Fobuild/obj/ /Febuild/CWordle`
  - To build on Linux, from the CWordle folder:  
    `gcc -O2 -std=gnu11 -o CWordle src/*.c src/**/*.c -Iinclude -lm; strip -s -R .comment -w -R .note* CWordle`
- WordleCpp - C++17
  - To build on Windows, in a Developer Command Prompt from the WordleCpp folder:  
    `mkdir build\obj & cl /EHsc /std:c++17 /O2 src/*.cpp src/console_app/*.cpp src/players/*.cpp /Iinclude/ /Fobuild/obj/ /Febuild/WordleCpp`
  - To build on Linux, from the WordleCpp folder:  
    `g++ -O2 -std=gnu++17 -fpermissive -o WordleCpp src/*.cpp src/**/*.cpp -Iinclude; strip -s -R .comment -w -R .note* WordleCpp`
- Wordle_CS - C#, .NET 6
  - To build, from the Wordle_CS folder:  
    `dotnet build -c Release`
