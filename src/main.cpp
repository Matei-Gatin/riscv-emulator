#include <exception>
#include <iostream>

#include "memory.h"

int main() {
    try {
        Memory memory{};
        std::cout << "guest memory: " << memory.size() << " bytes\n";
    } catch (const std::exception& error) {
        // Errors are thrown deep inside the emulator; main decides how to report them.
        std::cerr << "error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
