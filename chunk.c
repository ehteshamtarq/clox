#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>

#include "chunk.h"
#include "value.h"
#include "memory.h"

void initChunk(Chunk *chunk)
{
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->lineCount = 0;    // <--
    chunk->lineCapacity = 0; // <--
    chunk->lines = NULL;
    initValueArray(&chunk->constants);
}

void freeChunk(Chunk *chunk)
{
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(int, chunk->lines, chunk->capacity);
    FREE_ARRAY(LineStart, chunk->lines, chunk->lineCapacity);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}

/* There are three changes here. First, we don't implicitly grow the line array when we grow the code array.
Their sizes are decoupled now. Instead, we grow the line array when appending a new LineStart, if needed.
The second if statement is where we take advantage of adjacent instructions on the same line. If the line for the byte we're writing
is on the same line as the current line start, we don't create a new one. This is the compression.
Otherwise, if this is the first byte of code, or it appears on a different line, we begin a new LineStart and grow the array if needed.
This gives us a compressed array of LineStarts, where each one begins a new line.*/

void writeChunk(Chunk *chunk, uint8_t byte, int line)
{
    if (chunk->capacity < chunk->count + 1)
    {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code,
                                 oldCapacity, chunk->capacity);
        // Don't grow line array here...
    }

    chunk->code[chunk->count] = byte;
    chunk->count++;

    // See if we're still on the same line.
    if (chunk->lineCount > 0 &&
        chunk->lines[chunk->lineCount - 1].line == line)
    {
        return;
    }

    // Append a new LineStart.
    if (chunk->lineCapacity < chunk->lineCount + 1)
    {
        int oldCapacity = chunk->lineCapacity;
        chunk->lineCapacity = GROW_CAPACITY(oldCapacity);
        chunk->lines = GROW_ARRAY(LineStart, chunk->lines,
                                  oldCapacity, chunk->lineCapacity);
    }

    LineStart *lineStart = &chunk->lines[chunk->lineCount++];
    lineStart->offset = chunk->count - 1;
    lineStart->line = line;
}

// Given a byte offset for an instruction, it binary searches through the LineStart array to find which LineStart --
// and thus which line -- contains that offset. Using binary search is much faster than walking the whole array,
// but it does place a constraint on the compiler. It assumes line numbers for the instructions always monotonically increase.
// Since we're going to have a single-pass compiler, that should be doable.

int getLine(Chunk *chunk, int instruction)
{
    int start = 0;
    int end = chunk->lineCount - 1;

    for (;;)
    {
        int mid = (start + end) / 2;
        LineStart *line = &chunk->lines[mid];
        if (instruction < line->offset)
        {
            end = mid - 1;
        }
        else if (mid == chunk->lineCount - 1 ||
                 instruction < chunk->lines[mid + 1].offset)
        {
            return line->line;
        }
        else
        {
            start = mid + 1;
        }
    }
}

int addConstant(Chunk *chunk, Value value)
{
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}


void writeConstant(Chunk *chunk, Value value, int line)
{
    int index = addConstant(chunk, value);
    if (index < 256)
    {
        writeChunk(chunk, OP_CONSTANT, line);
        writeChunk(chunk, (uint8_t)index, line);
    }
    else
    {
        writeChunk(chunk, OP_CONSTANT_LONG, line);
        writeChunk(chunk, (uint8_t)(index & 0xff), line);  // Write the least significant byte
        writeChunk(chunk, (uint8_t)((index >> 8) & 0xff), line); // Write the middle byte
        writeChunk(chunk, (uint8_t)((index >> 16) & 0xff), line); // Write the most significant byte
    }
}