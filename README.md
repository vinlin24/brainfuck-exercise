# Brainfuck Exercise

My implementations of an interpreter for the esoteric [Brainfuck](https://en.wikipedia.org/wiki/Brainfuck) programming language, intended as learning exercises for picking up new programming languages. They also double as notebooks via code comments detailing certain language features/quirks I found particularly notable.

Implementations follow [this Brainfuck spec](https://github.com/sunjay/brainfuck/blob/master/brainfuck.md).

## Motivation

I once read that implementing a Brainfuck interpreter is a great starter project for learning a new programming language. It forces one to learn essential features like:

- I/O (processing command line arguments, reading files, formatting output, etc.).
- Data structures (using a stack and hashmap to implement bracket jumping comes to mind).
- Control flow (will likely have to use all of for/foreach loop, while loop, early return, error handling, etc. constructs).
- State management (an interpreter has quite a bit of internal state, motivating the use of OOP or similar constructs).
- Splitting the implementation into multiple files for modularity (optional).

## Implementations

To create a new Brainfuck interpreter implementation, create a subdirectory named after the programming language to use.

However the source files are organized under this subdirectory is up to the implementer and should be done so in whatever way is required by and/or idiomatic to the programming language's module system.

The only requirement is that the subdirectory have a **run.sh** file at the top level under the subdirectory. For example:

```
perl/
  run.sh  <-- this file
  brainfuck.pl
  interpreter.pm
```

This defines the code to be run by the [test driver](#test-driver) to pass Brainfuck source files into the implemented Brainfuck interpreter. The path to the Brainfuck source file will be passed as the first command-line argument i.e. `$1` to **run.sh**.

> [!WARNING]
>
> Obviously, if this weren't just my personal repository, allowing arbitrary executable code would be a huge security smell. It may be better to define a **declarative** system of configuring how to boot up the implementation language's compiler/interpreter, but this suffices for now.

There may also be an optional **setup.sh** file, which is run *once* before testing the individual test cases for a particular implementation. This serves as a hook for compiled languages to build their binary. **setup.sh** will be called with no command line arguments.

## Testing

An integration test suite is set up under [tests/](tests/).

### Test Driver

The [driver script](tests/test_interpreter.py) is written in Python, but it is agnostic of the language used to implement the interpreter being tested. This is because it invokes the implemented interpreter as a subprocess and compares its output instead of attempting to import any implementations directly. This makes it a central test site to drive the (test-driven) development of Brainfuck interpreters in any implementation language. The lifecycle of the test cases, [as described above](#implementations), is:

```
For each language directory:
    Run its setup script, if exists.
    For each test case:
        Run its run script.
```

To run the full test suite (all test cases for all implementations):

```sh
tests/test_interpreter.py
```

To run the test cases for only specific implementations, specify the names of their directories e.g.:

```sh
tests/test_interpreter.py perl python3
```

The `-v`/`--verbose` flag forwards the verbosity setting to the underlying [unittest](https://docs.python.org/3/library/unittest.html) framework.

As usual, use `--help` for general help on the tester CLI.

### Test Cases

Test case files are included at the top-level under [tests/](tests/) as well. The [driver script](#test-driver) dynamically loads test case information from these files on startup to construct test suites. A test case consists of:

| File Name     | Description                                        |
| ------------- | -------------------------------------------------- |
| test_name.bf  | Brainfuck source file                              |
| test_name.out | Expected stdout of Brainfuck program (optional)    |
| test_name.err | Expected exit code of Brainfuck program (optional) |

- If the expected stdout file is omitted, the driver will assume the Brainfuck program has no stdout output.
- If the exit code file is omitted, the driver will assume the Brainfuck program exits with 0 (success).
