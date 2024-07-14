#include "stack.h"

// TODO: temp to make sure reallocation works.
#define STACK_INITIAL_CAPACITY 1

static void stack_reallocate(struct stack *stack)
{
    size_t new_capacity = stack->capacity * 2;
    stack->items = realloc(stack->items, new_capacity);
    stack->capacity = new_capacity;
}

void stack_init(struct stack *stack)
{
    stack->items = (stack_value_t *)malloc(
        sizeof(stack_value_t) * STACK_INITIAL_CAPACITY);
    stack->size = 0;
    stack->capacity = STACK_INITIAL_CAPACITY;
}

bool stack_empty(const struct stack *stack)
{
    return stack->size == 0;
}

void stack_push(struct stack *stack, stack_value_t value)
{
    if (stack->size == stack->capacity)
    {
        stack_reallocate(stack);
    }
    stack->items[stack->size] = value;
    ++stack->size;
}

stack_value_t stack_pop(struct stack *stack)
{
    stack_value_t value = stack_top(stack);
    --stack->size;
    return value;
}

stack_value_t stack_top(const struct stack *stack)
{
    size_t top_index = stack->size - 1;
    return stack->items[top_index];
}

void stack_deinit(struct stack *stack)
{
    free(stack->items);
}
