# Custom Shell

This project is a simple Unix-like shell implementation in C.
It includes a tokenizer, abstract syntax tree (AST) parser, and an executor that interprets user input to execute commands, manage pipelines, and handle basic built-in functionality.

# Project Structure
```
.
├── main.c
├── tokenizer/
│   ├── tokenizer.c
│   └── tokenizer.h
├── AST/
│   ├── ast.c
│   └── ast.h
├── executor/
│   ├── executor.c
│   └── executor.h
├── utils/
│   ├── utils.c
│   └── utils.h
└── Makefile
```
Compiled object files are placed under the out/ directory when built with ```make```.

# Features
- Tokenizer – Splits input into tokens like words, pipes, and redirects using a finite-state machine (FSM).

- AST Parser – Converts the token stream into a hierarchical structure representing commands, conditionals, and pipelines.

- Executor – Executes parsed commands, handles redirections, and sets up pipes between processes.

- Built-in Commands: 

    - ```cd``` — change the current working directory

    - ```exit``` — terminate the shell

# Building
```bash
make all
```
This compiles all modules (tokenizer, AST, executor, utils) into the out/ directory, to the shell excecutable.

# Design Notes
## Parsing Approach

This shell does not parse commands on the spot like most real shells do.
Instead, it reads the entire input first, then tokenizes and builds an AST before executing it.

This happened because the parser was initially designed based on how compilers typically work — reading and analyzing the whole input before running anything.
Only later did I learn that shells actually parse and execute commands incrementally, processing pipelines and control structures as they appear.
If I had researched more at the start, I would have followed that more traditional shell design.

## Environment Variables

Variable assignment (e.g., VAR=value) works,
but retrieving environment variables (e.g., $VAR) is not yet implemented in this version.