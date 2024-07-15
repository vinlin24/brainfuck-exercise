#!/bin/bash

script_dir="$(dirname "$0")"
dotnet "$script_dir/bin/Debug/net8.0/cs.dll" "$@"
