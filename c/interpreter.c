#include "interpreter.h"

#include <stdio.h>
#include <string.h>

#include "stack.h"

#define OPERATOR_RIGHT '>'
#define OPERATOR_LEFT '<'
#define OPERATOR_INCREMENT '+'
#define OPERATOR_DECREMENT '-'
#define OPERATOR_WRITE '.'
#define OPERATOR_READ ','
#define OPERATOR_JUMP_IF_ZERO '['
#define OPERATOR_JUMP_UNLESS_ZERO ']'

static bool init_bracket_map(struct interpreter *interpreter)
{
    struct stack seen_openers;
    stack_init(&seen_openers);

    struct hashmap *bracket_map = &interpreter->bracket_map;
    bool source_has_error = false;

    for (size_t address = 0; address < interpreter->source_length; ++address)
    {
        char bf_operator = interpreter->source[address];
        if (bf_operator == OPERATOR_JUMP_IF_ZERO)
        {
            stack_push(&seen_openers, address);
            continue;
        }
        if (bf_operator == OPERATOR_JUMP_UNLESS_ZERO)
        {
            if (stack_empty(&seen_openers))
            {
                fprintf(stderr, "Brainfuck syntax error: mismatched %c\n",
                        OPERATOR_JUMP_UNLESS_ZERO);
                source_has_error = true;
                goto cleanup;
            }

            address_t destination_address = stack_pop(&seen_openers);
            hashmap_set(bracket_map, address, destination_address);
            hashmap_set(bracket_map, destination_address, address);
        }
    }

    if (!stack_empty(&seen_openers))
    {
        fprintf(stderr, "Brainfuck syntax error: mismatched %c\n",
                OPERATOR_JUMP_IF_ZERO);
        source_has_error = true;
        goto cleanup;
    }

cleanup:
    stack_deinit(&seen_openers);
    return !source_has_error;
}

static uint8_t *current_cell_ref(struct interpreter *interpreter)
{
    return &interpreter->cells[interpreter->pointer];
}

static void move_pointer_right(struct interpreter *interpreter)
{
    interpreter->pointer = (interpreter->pointer + 1) % MEMORY_SIZE;
}

static void move_pointer_left(struct interpreter *interpreter)
{
    address_t current_pointer = interpreter->pointer;
    interpreter->pointer = (current_pointer == 0)
                               ? MEMORY_SIZE - 1
                               : current_pointer - 1;
}

static void increment_cell(struct interpreter *interpreter)
{
    // Overflow handled by unsigned type system.
    ++(*current_cell_ref(interpreter));
}

static void decrement_cell(struct interpreter *interpreter)
{
    // Underflow handled by unsigned type system.
    --(*current_cell_ref(interpreter));
}

static void write_cell_to_stdout(struct interpreter *interpreter)
{
    int byte = *current_cell_ref(interpreter);
    putc(byte, stdout);
}

static void replace_cell_with_stdin(struct interpreter *interpreter)
{
    int byte = getc(stdin);
    *current_cell_ref(interpreter) = (uint8_t)byte;
}

static void jump_if_cell_is_zero(struct interpreter *interpreter)
{
    if (*current_cell_ref(interpreter) == 0)
    {
        address_t destination;
        hashmap_get(&interpreter->bracket_map, interpreter->pc, &destination);
        interpreter->pc = destination;
    }
}

static void jump_if_cell_is_nonzero(struct interpreter *interpreter)
{
    if (*current_cell_ref(interpreter) != 0)
    {
        address_t destination;
        hashmap_get(&interpreter->bracket_map, interpreter->pc, &destination);
        interpreter->pc = destination;
    }
}

bool interpreter_init(struct interpreter *interpreter, const char *source)
{
    interpreter->source = source;
    interpreter->source_length = strlen(source);
    interpreter->pc = 0;
    for (size_t i = 0; i < MEMORY_SIZE; ++i)
    {
        interpreter->cells[i] = 0;
    }
    interpreter->pointer = 0;
    hashmap_init(&interpreter->bracket_map);

    bool success = init_bracket_map(interpreter);
    return success;
}

void interpreter_run(struct interpreter *interpreter)
{
    while (interpreter->pc < interpreter->source_length)
    {
        char bf_operator = interpreter->source[interpreter->pc];
        switch (bf_operator)
        {
        case OPERATOR_RIGHT:
            move_pointer_right(interpreter);
            break;
        case OPERATOR_LEFT:
            move_pointer_left(interpreter);
            break;
        case OPERATOR_INCREMENT:
            increment_cell(interpreter);
            break;
        case OPERATOR_DECREMENT:
            decrement_cell(interpreter);
            break;
        case OPERATOR_WRITE:
            write_cell_to_stdout(interpreter);
            break;
        case OPERATOR_READ:
            replace_cell_with_stdin(interpreter);
            break;
        case OPERATOR_JUMP_IF_ZERO:
            jump_if_cell_is_zero(interpreter);
            break;
        case OPERATOR_JUMP_UNLESS_ZERO:
            jump_if_cell_is_nonzero(interpreter);
            break;
        }

        ++interpreter->pc;
    }
}

void interpreter_deinit(struct interpreter *interpreter)
{
    hashmap_deinit(&interpreter->bracket_map);
}
