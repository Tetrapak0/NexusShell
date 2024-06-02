#include "util.h"
#ifdef _WIN32
#include <handleapi.h>
#endif
#include <stdio.h>

void warn(const char* format, ...) {
    va_list args;
    va_start(args, format);
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole != INVALID_HANDLE_VALUE)
        fprintf(stderr, "Failed to change terminal color. GetLastError(): %lu", GetLastError());
    else {
        SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN); 
        vprintf("WARNING: ", args);
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
#else
    vprintf(stderr, "\x1B[33mWARNING: ", args);
    printf("\x1B[0m\n");
#endif
    va_end(args);
}

void error(const char* format, ...) {
    va_list args;
    va_start(args, format);
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole != INVALID_HANDLE_VALUE)
        fprintf(stderr, "Failed to change terminal color. GetLastError(): %lu", GetLastError());
    else {
        SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_RED); 
        vfprintf(stderr, "ERROR: ", args);
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
#else
    vfprintf(stderr, "\x1B[31mERROR: ", args);
    printf("\x1B[0m\n");
#endif
    va_end(args);
}

int swap_endian(int num) {
    return ((num << 24) & 0xFF000000) |
           ((num << 8) & 0x00FF0000) |
           ((num >> 8) & 0x0000FF00) |
           ((num >> 24) & 0x000000FF);
}

