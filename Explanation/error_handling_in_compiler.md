Overview of Error Handling in the Compiler
The compiler handles errors using several mechanisms:

Reporting Errors at Specific Locations: Functions errorAtCurrent() and error().
Printing Error Messages: Function errorAt().
Tracking Errors and Panic Mode: Fields hadError and panicMode in the Parser struct.
Consuming and Validating Tokens: The consume() function.
Let's dive into each section in detail:

1. Reporting Errors at Specific Locations
When the parser encounters an error, it needs to notify the user with a meaningful error message. To do this, it provides two functions:

errorAtCurrent(const char* message):

This function is used to report an error at the current token.
It calls errorAt() with the current token (parser.current) to print the error message at the specific location.

```
static void errorAtCurrent(const char* message) {
    errorAt(&parser.current, message);
}
```

error(const char* message):

This function reports an error at the last consumed token (i.e., the previous token).
It uses errorAt() with parser.previous to display where the error occurred in the previously consumed token.

```
static void error(const char* message) {
    errorAt(&parser.previous, message);
}
```
2. Printing Error Messages
The function that does the heavy lifting of printing error messages is errorAt():

errorAt(Token* token, const char* message):

This function prints the error message along with the line number and token information.
It also sets the hadError flag to indicate that an error has occurred.

```
static void errorAt(Token* token, const char* message) {
    fprintf(stderr, "[line %d] Error", token->line);
    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type == TOKEN_ERROR) {
        // Nothing to display; token itself is an error.
    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }
    fprintf(stderr, ": %s\n", message);
    parser.hadError = true;
}
```
Error Details: It provides context by showing where the error occurred—either at the end of the file (EOF), for a specific token, or just the message if the token itself is an error.
Setting hadError: After printing, it sets the hadError flag to true to track that an error occurred.
3. Tracking Errors and Panic Mode
To manage errors effectively without generating cascades of errors, the compiler uses two flags within the Parser struct:

hadError:

Indicates if any error has been detected during compilation.
If hadError is true at the end of the compilation, the process fails.
panicMode:

A flag that prevents the compiler from reporting multiple meaningless errors after the first one.
When an error is encountered, panicMode is set to true to suppress further errors until the parser reaches a synchronization point (e.g., the end of a statement or a specific token type).

```
bool hadError;
bool panicMode;
```

Setting Panic Mode in errorAt():
When an error is reported using errorAt(), if the panicMode flag is already set, it returns immediately to avoid printing further errors.
Otherwise, it sets panicMode to true.

```
static void errorAt(Token* token, const char* message) {
    if (parser.panicMode) return;  // Suppress further errors.
    parser.panicMode = true;
    // Proceed to print the error message...
}
```
Resetting Panic Mode:

When a synchronization point is reached (e.g., the end of a statement), panicMode is cleared, allowing error reporting to resume normally.
4. Consuming and Validating Tokens
To parse code correctly, the parser needs to consume tokens and ensure that they match the expected syntax.

consume(TokenType type, const char* message):

This function checks if the current token matches the expected type.
If it matches, the parser advances to the next token using advance().
If it doesn't match, it reports an error using errorAtCurrent().

```
static void consume(TokenType type, const char* message) {
    if (parser.current.type == type) {
        advance();  // Move to the next token.
        return;
    }
    errorAtCurrent(message);  // Report an error if the type does not match.
}
```

This function is fundamental for handling most syntax errors in the compiler, ensuring that the correct tokens appear in the expected order according to the grammar rules.