#!/usr/bin/env ruby

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

class Interpreter
    def initialize(source, memory_size = 4096)
        @source = source
        @memory_size = memory_size
        @pc = 0
        @cells = Array.new(memory_size) { 0 }
        @pointer = 0
        @bracket_map = {}

        init_bracket_map
    end

    def run
        while @pc < @source.length
            operator = @source[@pc]

            if operator == OPERATOR_RIGHT
                move_pointer_right
            elsif operator == OPERATOR_LEFT
                move_pointer_left
            elsif operator == OPERATOR_INCREMENT
                increment_cell
            elsif operator == OPERATOR_DECREMENT
                decrement_cell
            elsif operator == OPERATOR_WRITE
                write_cell_to_stdout
            elsif operator == OPERATOR_READ
                replace_cell_with_stdin
            elsif operator == OPERATOR_JUMP_IF_ZERO
                jump_if_cell_is_zero
            elsif operator == OPERATOR_JUMP_UNLESS_ZERO
                jump_if_cell_is_nonzero
            end

            # Ignore all non-operator characters i.e. treat them like comments.

            @pc += 1
        end
    end

    private

    def init_bracket_map
        # Stack of instruction addresses of seen opening brackets `[`.
        bracket_stack = []

        (0..@source.length - 1).each do |address|
            operator = @source[address]
            if operator == OPERATOR_JUMP_IF_ZERO
                bracket_stack.push(address)
                next
            end
            if operator == OPERATOR_JUMP_UNLESS_ZERO
                # Extraneous closer.
                if bracket_stack.length == 0
                    warn "Brainfuck syntax error: mismatched #{OPERATOR_JUMP_UNLESS_ZERO}"
                    exit 1
                end
                opener_address = bracket_stack.pop
                closer_address = address
                @bracket_map[opener_address] = closer_address
                @bracket_map[closer_address] = opener_address
            end
        end

        # Extraneous opener(s).
        if bracket_stack.length > 0
            warn "Brainfuck syntax error: mismatched #{OPERATOR_JUMP_IF_ZERO}"
            exit 1
        end
    end

    def move_pointer_right
        @pointer = (@pointer + 1) % @memory_size
    end

    def move_pointer_left
        if @pointer == 0
            @pointer = @memory_size - 1
        else
            @pointer -= 1
        end
    end

    def increment_cell
        if @cells[@pointer] == CELL_MAX_VALUE
            @cells[@pointer] = CELL_MIN_VALUE
        else
            @cells[@pointer] += 1
        end
    end

    def decrement_cell
        if @cells[@pointer] == CELL_MIN_VALUE
            @cells[@pointer] = CELL_MAX_VALUE
        else
            @cells[@pointer] -= 1
        end
    end

    def write_cell_to_stdout
        print @cells[@pointer].chr
    end

    def replace_cell_with_stdin
        input = STDIN.getc
        @cells[@pointer] = input.ord
    end

    def jump_if_cell_is_zero
        if @cells[@pointer] == 0
            @pc = @bracket_map[@pc]
        end
    end

    def jump_if_cell_is_nonzero
        if @cells[@pointer] != 0
            @pc = @bracket_map[@pc]
        end
    end
end

def get_usage
    return "usage: #{$0} FILE"
end

def main
    if ARGV.length < 1
        warn get_usage
        exit 1
    end

    source_path = ARGV[0]
    source = IO.read(source_path)

    interpreter = Interpreter.new(source)
    interpreter.run
end

if __FILE__ == $0
    main
end
