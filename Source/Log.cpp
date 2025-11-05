#pragma once

#include "Globals.h"

#include <iostream>
#include <string>

void log(const char file[], int line, const char* format, ...)
{
    static char tmpString1[4096];
    static va_list ap;

    // Construct the string from variable arguments
    va_start(ap, format);
    vsnprintf(tmpString1, 4096, format, ap);
    va_end(ap);

    // Construct the final log message
    std::string logMessage = std::string("\n") + file + "(" + std::to_string(line) + ") : " + tmpString1;

    // Print the formatted string to the standard error stream
    std::cerr << logMessage << std::endl;
}