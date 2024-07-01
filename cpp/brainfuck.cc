// C++ implementation of a Brainfuck interpreter.

#include <iostream>
#include <fstream>
#include <sstream>

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif // _WIN32

#include "interpreter.h"

static inline std::string GetUsage(const char *program)
{
    return std::string("usage: ") + program + " FILE";
}

static inline std::string SlurpFile(const char *source_path)
{
    std::stringstream buffer;
    std::ifstream stream(source_path);
    buffer << stream.rdbuf();
    return buffer.str();
}

int main(int argc, const char *argv[])
{
#ifdef _WIN32
    // Set stdout to binary mode (this is to make \n output LF, not CRLF).
    _setmode(_fileno(stdout), _O_BINARY);
#endif // _WIN32

    if (argc < 2)
    {
        std::cerr << GetUsage(argv[0]) << std::endl;
        return EXIT_FAILURE;
    }

    const char *source_path = argv[1];
    std::string source = SlurpFile(source_path);

    Interpreter interpreter(source);
    interpreter.run();

    return EXIT_SUCCESS;
}
