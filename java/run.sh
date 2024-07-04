#!/bin/bash

script_dir="$(dirname "$0")"
entry_point='Brainfuck'
# Note that we have to specify the name of the entry point ALONE (no .class and
# no directory parts). This means that if we have to run from a different
# directory, we need to set `-cp` (class search path).
java -cp "$script_dir" "$entry_point" "$@"
