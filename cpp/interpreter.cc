#include "interpreter.h"

#include <iostream>
#include <stack>

Interpreter::Interpreter(const std::string &source)
    : source_(source), pc_(0), cells_{0}, pointer_(0), bracket_map_{}
{
    InitBracketMap();
}

void Interpreter::run()
{
    while (pc_ < source_.length())
    {
        char bf_operator = source_[pc_];

        switch (bf_operator)
        {
        case OPERATOR_RIGHT:
            MovePointerRight();
            break;
        case OPERATOR_LEFT:
            MovePointerLeft();
            break;
        case OPERATOR_INCREMENT:
            IncrementCell();
            break;
        case OPERATOR_DECREMENT:
            DecrementCell();
            break;
        case OPERATOR_WRITE:
            WriteCellToStdout();
            break;
        case OPERATOR_READ:
            ReplaceCellWithStdin();
            break;
        case OPERATOR_JUMP_IF_ZERO:
            JumpIfCellIsZero();
            break;
        case OPERATOR_JUMP_UNLESS_ZERO:
            JumpIfCellIsNonzero();
            break;
        }

        ++pc_;
    }
}

void Interpreter::InitBracketMap()
{
    std::stack<address_t> opener_addresses;

    for (address_t address = 0; address < source_.length(); ++address)
    {
        char bf_operator = source_[address];

        if (bf_operator == OPERATOR_JUMP_IF_ZERO)
        {
            opener_addresses.push(address);
            continue;
        }

        if (bf_operator == OPERATOR_JUMP_UNLESS_ZERO)
        {
            if (opener_addresses.empty())
            {
                std::cerr << "Brainfuck syntax error: mismatched "
                          << OPERATOR_JUMP_UNLESS_ZERO << std::endl;
                std::exit(1);
            }

            address_t opener_address = opener_addresses.top();
            opener_addresses.pop();

            bracket_map_[opener_address] = address;
            bracket_map_[address] = opener_address;
        }
    }

    if (!opener_addresses.empty())
    {
        std::cerr << "Brainfuck syntax error: mismatched "
                  << OPERATOR_JUMP_IF_ZERO << std::endl;
        std::exit(1);
    }
}

void Interpreter::MovePointerRight()
{
    pointer_ = (pointer_ + 1) % MEMORY_SIZE;
}

void Interpreter::MovePointerLeft()
{
    pointer_ = pointer_ == 0 ? MEMORY_SIZE - 1 : pointer_ - 1;
}

void Interpreter::IncrementCell()
{
    // Overflow automatically handled by unsigned type system.
    ++CurrentCell();
}

void Interpreter::DecrementCell()
{
    // Underflow automatically handled by unsigned type system.
    --CurrentCell();
}

void Interpreter::WriteCellToStdout()
{
    std::cout.put(CurrentCell());
}

void Interpreter::ReplaceCellWithStdin()
{
    char byte;
    std::cin.get(byte);
    CurrentCell() = static_cast<std::uint8_t>(byte);
}

void Interpreter::JumpIfCellIsZero()
{
    if (CurrentCell() == 0)
    {
        address_t destination_address = bracket_map_[pc_];
        pc_ = destination_address;
    }
}

void Interpreter::JumpIfCellIsNonzero()
{
    if (CurrentCell() != 0)
    {
        address_t destination_address = bracket_map_[pc_];
        pc_ = destination_address;
    }
}
