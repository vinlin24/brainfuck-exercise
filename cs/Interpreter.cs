using System;
using System.Collections.Generic;

class Interpreter
{
    private const char OPERATOR_RIGHT = '>';
    private const char OPERATOR_LEFT = '<';
    private const char OPERATOR_INCREMENT = '+';
    private const char OPERATOR_DECREMENT = '-';
    private const char OPERATOR_WRITE = '.';
    private const char OPERATOR_READ = ',';
    private const char OPERATOR_JUMP_IF_ZERO = '[';
    private const char OPERATOR_JUMP_UNLESS_ZERO = ']';

    private int _pc;
    private byte[] _cells;
    private int _pointer;
    private Dictionary<int, int> _bracketMap;

    public string Source { get; }

    private byte CurrentCell
    {
        get { return _cells[_pointer]; }
        set { _cells[_pointer] = value; }
    }

    public Interpreter(string source, int memorySize = 4096)
    {
        Source = source;
        _pc = 0;
        _cells = new byte[memorySize];
        _pointer = 0;
        _bracketMap = InitBracketMap();
    }

    public void Run()
    {
        while (_pc < Source.Length)
        {
            char bfOperator = Source[_pc];

            switch (bfOperator)
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

            ++_pc;
        }
    }

    private Dictionary<int, int> InitBracketMap()
    {
        Dictionary<int, int> bracketMap = new();
        Stack<int> seenAddresses = new();

        for (int address = 0; address < Source.Length; ++address)
        {
            char bfOperator = Source[address];
            if (bfOperator == OPERATOR_JUMP_IF_ZERO)
            {
                seenAddresses.Push(address);
                continue;
            }
            if (bfOperator == OPERATOR_JUMP_UNLESS_ZERO)
            {
                if (seenAddresses.Count == 0)
                {
                    Console.Error.WriteLine(
                        "Brainfuck syntax error: mismatched " +
                        OPERATOR_JUMP_UNLESS_ZERO
                    );
                    Environment.Exit(1);
                }
                int destinationAddress = seenAddresses.Pop();
                bracketMap[address] = destinationAddress;
                bracketMap[destinationAddress] = address;
            }
        }

        if (seenAddresses.Count > 0)
        {
            Console.Error.WriteLine(
                "Brainfuck syntax error: mismatched " +
                OPERATOR_JUMP_IF_ZERO
            );
            Environment.Exit(1);
        }

        return bracketMap;
    }

    private void MovePointerRight()
    {
        _pointer = (_pointer + 1) % _cells.Length;
    }

    private void MovePointerLeft()
    {
        _pointer = (_pointer == 0) ? _cells.Length - 1 : _pointer - 1;
    }

    private void IncrementCell()
    {
        // Overflow handled by unsigned type system.
        ++CurrentCell;
    }

    private void DecrementCell()
    {
        // Underflow handled by unsigned type system.
        --CurrentCell;
    }

    private void WriteCellToStdout()
    {
        Console.Write((char)CurrentCell);
    }

    private void ReplaceCellWithStdin()
    {
        int character = Console.Read();
        CurrentCell = (byte)character;
    }

    private void JumpIfCellIsZero()
    {
        if (CurrentCell == 0)
        {
            int destinationAddress = _bracketMap[_pc];
            _pc = destinationAddress;
        }
    }

    private void JumpIfCellIsNonzero()
    {
        if (CurrentCell != 0)
        {
            int destinationAddress = _bracketMap[_pc];
            _pc = destinationAddress;
        }
    }
}
