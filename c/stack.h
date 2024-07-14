#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include <stdlib.h>

typedef size_t stack_value_t;

struct stack
{
    stack_value_t *items;
    size_t size;
    size_t capacity;
};

void stack_init(struct stack *stack);

bool stack_empty(const struct stack *stack);

void stack_push(struct stack *stack, stack_value_t value);

stack_value_t stack_pop(struct stack *stack);

stack_value_t stack_top(const struct stack *stack);

void stack_deinit(struct stack *stack);

#endif // STACK_H
