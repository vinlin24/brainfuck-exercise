#!/bin/bash

script_dir="$(dirname "$0")"
binary="${script_dir}/brainfuck"
"${binary}" "$@"
