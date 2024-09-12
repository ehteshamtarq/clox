#ifndef clox_vm_h
#define clox_vm_h

#include "chunk.h"
#include "value.h"

#define STACK_MAX 256

typedef struct
{
    Chunk *chunk;           // The chunk of bytecode to execute.
    uint8_t *ip;            // The instruction pointer (IP).
    Value stack[STACK_MAX]; // Stack Implementation in the VM
    Value *stackTop;        // To keep track of the top of the stack
    Obj* objects;
} VM;

typedef enum
{
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

extern VM vm;

void initVM();
void freeVM();
InterpretResult interpret(const char *source);
void push(Value value);
Value pop();

#endif