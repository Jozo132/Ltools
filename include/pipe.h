#pragma once

#include "tools.h"

std::string getPipe() {
    std::string data;
    if (!isatty(fileno(stdin))) { // Check if there's data in the pipe
        if (std::cin.peek() != EOF) { // Check if there's any data available
            std::ostringstream buffer;
            buffer << std::cin.rdbuf();  // Read all available data from the pipe
            data = buffer.str();
        }
    }
    return data;
}