# Lox bytecode interpreter implemented in C++

Written for educational purposes and with book [Crafting interpreters](https://craftinginterpreters.com/).
I've tried to use safe C++ constructs and data type with OOP. And, of course, without global varaibles.
This is not an exact same replica with `clox`, but it interprets the same language.

The code is messy and I want to make my own language (*well, another replica of JS*), so wait for `Loop`!

# Features

*They are undone, actually.*

- Writing and reading chunks. *Undone*.
- Optimization. *Only some kind of constant folding*.
- Verifying. *When I've added jumps and loops it cracked*.
- Tests. *When functions appeared, tests become very hard to write*.

# Differences from Lox and `clox`

- Natives have arity
- GC allocates only objects. Dynamic arrays are implemented with `std::vector`. And so on.
- Unimplemented GC when `StressGC` is off. *Because GC can't track how many bytes the program uses, but it can track
  living objects count*.
- There is only `Closure` object, no `Function`.
- `Upvalue` objects holds an `std::vector` of upvalues.
- GC can be switched on or off. *Is it a good feature?*
- There are `PrintFlags`. The only use for them is to print string as it is, or with quotes and escaped symbols.
- The code heavily relies on macros that use macros. (Link)[]

# Project structure
This repository contains three projects:
1. Lox VM library `LoxLib/`. It contains two folders: `include` for public headers and `src` for implementation.
2. Lox VM runner `LoxInterpreter/` (*a bad name perhaps*). It is the main file that runs REPL or a file. It contains `src` directory where it is implemented.
3. Lox tests `LoxGoogleTests`. It contains folder `src`, where the tests lie. There is no main file, so they are intended to run with `gtest_main`.