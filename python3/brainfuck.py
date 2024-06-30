#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Python 3 implementation of a Brainfuck interpreter."""

import sys
from pathlib import Path

OPERATOR_RIGHT = ">"
OPERATOR_LEFT = "<"
OPERATOR_INCREMENT = "+"
OPERATOR_DECREMENT = "-"
OPERATOR_WRITE = "."
OPERATOR_READ = ","
OPERATOR_JUMP_IF_ZERO = "["
OPERATOR_JUMP_UNLESS_ZERO = "]"

CELL_MIN_VALUE = 0
CELL_MAX_VALUE = 255


class Interpreter:
    def __init__(self, source: str, memory_size: int = 4096) -> None:
        assert memory_size > 0
        self._source = source
        self._pc = 0
        self._cells = [0] * memory_size
        self._pointer = 0
        self._bracket_map = self._init_bracket_map(source)

    @property
    def source(self) -> str:
        return self._source

    @property
    def memory_size(self) -> int:
        return len(self._cells)

    def peek_memory(self) -> list[int]:
        return list(self._cells)

    def run(self) -> None:
        num_instructions = len(self._source)

        while self._pc < num_instructions:
            operator = self._source[self._pc]

            if operator == OPERATOR_RIGHT:
                self._move_pointer_right()
            elif operator == OPERATOR_LEFT:
                self._move_pointer_left()
            elif operator == OPERATOR_INCREMENT:
                self._increment_cell()
            elif operator == OPERATOR_DECREMENT:
                self._decrement_cell()
            elif operator == OPERATOR_WRITE:
                self._write_cell_to_stdout()
            elif operator == OPERATOR_READ:
                self._replace_cell_with_stdin()
            elif operator == OPERATOR_JUMP_IF_ZERO:
                self._jump_if_cell_is_zero()
            elif operator == OPERATOR_JUMP_UNLESS_ZERO:
                self._jump_if_cell_is_nonzero()

            # Ignore all non-operator characters i.e. treat them like
            # comments.

            self._pc += 1

    def _init_bracket_map(self, source: str) -> dict[int, int]:
        # Bidirectional dict that maps address of `[` to the address of
        # its corresponding `]`, and vice versa.
        bracket_map = dict[int, int]()

        # Stack of instruction addresses of seen opening brackets `[`.
        bracket_stack = list[int]()

        for address, operator in enumerate(source):
            if operator == OPERATOR_JUMP_IF_ZERO:
                bracket_stack.append(address)
                continue

            if operator == OPERATOR_JUMP_UNLESS_ZERO:
                try:
                    opener_address = bracket_stack.pop()
                # Extraneous closer.
                except IndexError:
                    print(
                        "Brainfuck syntax error: "
                        f"mismatched {OPERATOR_JUMP_UNLESS_ZERO}",
                        file=sys.stderr,
                    )
                    sys.exit(1)

                closer_address = address
                bracket_map[opener_address] = closer_address
                bracket_map[closer_address] = opener_address

        # Extraneous opener(s).
        if bracket_stack:
            print(
                f"Brainfuck syntax error: mismatched {OPERATOR_JUMP_IF_ZERO}",
                file=sys.stderr,
            )
            sys.exit(1)

        return bracket_map

    def _move_pointer_right(self) -> None:
        self._pointer = (self._pointer + 1) % self.memory_size

    def _move_pointer_left(self) -> None:
        if self._pointer == 0:
            self._pointer = self.memory_size - 1
        else:
            self._pointer -= 1

    def _increment_cell(self) -> None:
        if self._cells[self._pointer] == CELL_MAX_VALUE:
            self._cells[self._pointer] = CELL_MIN_VALUE
        else:
            self._cells[self._pointer] += 1

    def _decrement_cell(self) -> None:
        if self._cells[self._pointer] == CELL_MIN_VALUE:
            self._cells[self._pointer] = CELL_MAX_VALUE
        else:
            self._cells[self._pointer] -= 1

    def _write_cell_to_stdout(self) -> None:
        cell_value = self._cells[self._pointer]
        # NOTE: Use this instead of print() to make sure system-specific
        # things like converting LF to CRLF do not take place.
        sys.stdout.buffer.write(cell_value.to_bytes())

    def _replace_cell_with_stdin(self) -> None:
        input_byte = sys.stdin.buffer.read(1)
        incoming_value = int.from_bytes(input_byte)
        self._cells[self._pointer] = incoming_value

    def _jump_if_cell_is_zero(self) -> None:
        cell_value = self._cells[self._pointer]
        if cell_value == 0:
            destination_address = self._bracket_map[self._pc]
            self._pc = destination_address

    def _jump_if_cell_is_nonzero(self) -> None:
        cell_value = self._cells[self._pointer]
        if cell_value != 0:
            destination_address = self._bracket_map[self._pc]
            self._pc = destination_address


def get_usage() -> str:
    return f"usage: {sys.argv[0]} FILE"


def main() -> None:
    argc = len(sys.argv)

    if argc < 2:
        usage = get_usage()
        print(usage, file=sys.stderr)
        sys.exit(1)

    source_path = Path(sys.argv[1])
    source = source_path.read_text("utf-8")

    interpreter = Interpreter(source)
    interpreter.run()


if __name__ == "__main__":
    main()
