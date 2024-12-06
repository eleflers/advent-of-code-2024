# About

[Advent of Code, 2024](https://adventofcode.com/2024)

A series of fun festive coding challenges!

# Goal

This is a learning exercise purely for fun, so it will not be an exercise in perfect coding, in hyper-optimal code golfing, or gunning for top leaderboard positions! My aim is to practice and learn some new C++ or possibly C#. My most recent professional experience is mainly in C++, but in niche applications or working with existing legacy codebases, so there is much to learn and it may not be pretty! C# is a language I used professionally 7+ years ago, and have not used on Linux, so could be a good alternate language I may introduce at some point as I'd like to see what the state of play for C# on Linux is in 2024

With that in mind here are some goals I could or want to address at some point:

- Re-acquaint myself with low-level basic C++
- Learn some modern idiomatic C++ features or standards (e.g. C++23)
- Try clang in place of gcc
- Set up a build chain / make file
    - Refactor code to pull out some common libray functions that I can potentially re-use over multiple days
    - GNU autotools vs CMake?
    - Continuous integration? Jenkins?
- Unit tests for C++?
- Port some solutions to C#, still on Linux (dotnet core)
    - Could switch entirely to C# for some challenges
- Debuggers, use gdb on CLI and vscode
- Improve / try different development environments
    - Add a language server to (neo)vim? Are there intelligent autocomplete options that know things like class member variables and such even if they are from header files elsewhere? How difficult / annoying is it to set up and how portable is it?
    - Any helpful vscode extensions?
        - Compile and run code using vscode, for a single file or for the build chain solutions described above
    - Doom Emacs over SSH (I am coding on a VM)

# How-To

At some point I will hopefully add a proper build chain (see goals outlined above)

Until then, here is how to run `day1` for reference

`c++2b` is C++23 standard

```bash
cd day1/
g++ day1.cc -std=c++2b -Wall && ./a.out
```

## General Helpers

### Performance Metrics

```bash
time ./a.out
perf stat ./a.out
perf record ./a.out && perf report
```

### 7z

Test data

```bash
7za l zip.7z # List
7za x zip.7z # eXtract
```

# TODO

- Learn how streams work
    - E.g. to read 2 numbers in on day 1 challenge, could use `while(cin >> a >> b)`? How does this work
- `std::multiset` could improve efficiency of the `count()` in day 1 part 2

