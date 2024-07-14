#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "hashmap.h"

#define MEMORY_SIZE 4096

typedef size_t address_t;

struct interpreter
{
    const char *source;
    size_t source_length;
    address_t pc;
    uint8_t cells[MEMORY_SIZE];
    address_t pointer;
    struct hashmap bracket_map;
};

bool interpreter_init(struct interpreter *interpreter, const char *source);

void interpreter_run(struct interpreter *interpreter);

void interpreter_deinit(struct interpreter *interpreter);

#endif // INTERPRETER_H
