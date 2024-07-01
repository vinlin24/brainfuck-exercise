#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <array>
#include <string>
#include <unordered_map>

constexpr char OPERATOR_RIGHT = '>';
constexpr char OPERATOR_LEFT = '<';
constexpr char OPERATOR_INCREMENT = '+';
constexpr char OPERATOR_DECREMENT = '-';
constexpr char OPERATOR_WRITE = '.';
constexpr char OPERATOR_READ = ',';
constexpr char OPERATOR_JUMP_IF_ZERO = '[';
constexpr char OPERATOR_JUMP_UNLESS_ZERO = ']';

class Interpreter
{
public:
    Interpreter(const std::string &source);

    const std::string &source() const { return source_; }

    void run();

    static constexpr std::size_t MEMORY_SIZE = 4096;

private:
    using address_t = std::size_t;

    std::uint8_t &CurrentCell() { return cells_[pointer_]; }

    void InitBracketMap();
    void MovePointerRight();
    void MovePointerLeft();
    void IncrementCell();
    void DecrementCell();
    void WriteCellToStdout();
    void ReplaceCellWithStdin();
    void JumpIfCellIsZero();
    void JumpIfCellIsNonzero();

    const std::string source_;
    address_t pc_;
    std::array<std::uint8_t, MEMORY_SIZE> cells_;
    address_t pointer_;
    std::unordered_map<address_t, address_t> bracket_map_;
};

#endif // INTERPRETER_H
