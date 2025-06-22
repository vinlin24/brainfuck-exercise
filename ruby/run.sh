#!/bin/bash

script_dir="$(dirname "$0")"
entry_point="${script_dir}/brainfuck.rb"
ruby "$entry_point" "$@"
