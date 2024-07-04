#!/bin/bash

script_dir="$(dirname "$0")"
# Using the `-d` option specifies the directory to place generated .class files.
# This allows us to avoid using error-prone cd's.
javac "$script_dir"/*.java -d "$script_dir"
