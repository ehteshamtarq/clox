#include <stdio.h>
#include <string.h>
#include "common.h"
#include "scanner.h"

typedef struct
{
    const char *start;   // marks the beginning of the current lexeme
    const char *current; // the current character being looked at
    int line;
} Scanner;

Scanner scanner;

void initScanner(const char *source)
{
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}