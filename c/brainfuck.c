// C implementation of a Brainfuck interpreter.

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif // _WIN32

#include "interpreter.h"

struct dynamic_buffer
{
    char *string;
};

static void print_usage(const char *prog, FILE *stream)
{
    fprintf(stream, "usage: %s FILE\n", prog);
}

static bool slurp_file(FILE *file, struct dynamic_buffer *out_buffer)
{
    if (fseek(file, 0, SEEK_END) == -1)
    {
        perror("Error seeking file end");
        return false;
    }

    long file_length;
    if ((file_length = ftell(file)) == -1)
    {
        perror("Error telling file end");
        return false;
    }

    if (fseek(file, 0, SEEK_SET) == -1)
    {
        perror("Error seeking file start");
        return false;
    }

    char *block = (char *)malloc(sizeof(char) * (file_length + 1));
    if (block == NULL)
    {
        perror("Error allocating buffer for file content");
        return false;
    }

    size_t chars_read = fread(block, sizeof(char), file_length, file);
    if (chars_read != (size_t)file_length)
    {
        perror("Error reading entire file into buffer");
        return false;
    }
    block[file_length] = '\0';

    out_buffer->string = block;
    return true;
}

int main(int argc, const char *argv[])
{
#ifdef _WIN32
    // Set stdout to binary mode (this is to make \n output LF, not CRLF).
    _setmode(_fileno(stdout), _O_BINARY);
#endif // _WIN32

    int exit_code = EXIT_SUCCESS;

    if (argc < 2)
    {
        print_usage(argv[0], stderr);
        exit_code = EXIT_FAILURE;
        goto cleanup_nothing;
    }

    const char *source_path = argv[1];

    FILE *source_file = fopen(source_path, "r");
    if (source_file == NULL)
    {
        perror("Error opening file");
        exit_code = EXIT_FAILURE;
        goto cleanup_nothing;
    }

    struct dynamic_buffer source = {.string = NULL};
    if (!slurp_file(source_file, &source))
    {
        exit_code = EXIT_FAILURE;
        goto cleanup_buffer;
    }

    struct interpreter interpreter;
    if (!interpreter_init(&interpreter, source.string))
    {
        exit_code = EXIT_FAILURE;
        goto cleanup_interpreter;
    };
    interpreter_run(&interpreter);

cleanup_interpreter:
    interpreter_deinit(&interpreter);
cleanup_buffer:
    free(source.string);
    fclose(source_file);
cleanup_nothing:
    return exit_code;
}
