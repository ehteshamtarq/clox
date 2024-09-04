# **KEY CONCEPTS**

- **Constant Pool**: In the context of the Java Virtual Machine (JVM), the constant pool is a specific section within each class file that stores all the constants and literals needed by that class. The constant pool is an essential part of the .class file format.

- **Single Pool**: A simplified constant pool where all types of constants are stored together in a single list.

- **chunk.c** deals with the overall structure of bytecode, including managing and writing bytecode instructions and constants.
chunk.c contains the Chunk structure that includes both bytecode and a ValueArray for constants.

- **value.c** is concerned with the handling and representation of values stored in dynamic arrays used within the interpreter.
Data Structures. value.c focuses on the ValueArray and operations related to Value types.

- **Bytecode Array** (code): An array of bytes that represent executable instructions.
- **Constant Pool** : An array that stores constant values used in the code, such as numbers and strings.
- **Line Information** (lines): An array of integers that stores the line number for each corresponding bytecode instruction. This is used for debugging and error reporting.


- **Output Breakdown**:

0000: This is the byte offset in the bytecode array where the instruction starts.
123: This is the source code line number for the instruction at offset 0000.
OP_CONSTANT: This is the opcode for the instruction at offset 0000. It indicates that the instruction is loading a constant value.
0: This is the operand for the OP_CONSTANT instruction. It refers to the index 0 in the constant pool where the value 1.2 is stored.
'1.2': This is the constant value being loaded by the OP_CONSTANT instruction from the constant pool.
0002: This is the next byte offset for the following instruction.
|: This symbol indicates that the instruction at offset 0002 is from the same line 123 as the previous instruction.
OP_RETURN: This is the opcode for the instruction at offset 0002, indicating a return from the current function or program

- When a program is compiled into bytecode, it is represented as a sequence of bytes. Each instruction in the bytecode is an opcode (a byte that specifies what operation to perform) followed by zero or more operands (additional bytes that provide necessary data for the operation).
The byte offset is the index or position of a particular byte or instruction relative to the beginning of the bytecode array (or chunk).

Here, the numbers 0000 and 0002 are byte offsets:

0000: This is the offset of the OP_CONSTANT opcode. It is the first byte in the chunk, so its offset is 0.
0002: This is the offset of the OP_RETURN opcode. It comes after the OP_CONSTANT opcode (1 byte) and its operand (1 byte), so its offset is 2.

- **OP_CONSTANT_LONG** would be needed in the interpreter in cases where a chunk of code contains more than 256 unique constants.

- **Virtual Machines** - Separation of Concerns: The VM allows the interpreter to separate parsing and compilation from execution. The front-end of the interpreter translates source code into an intermediate bytecode representation, which the VM then executes. This separation simplifies the interpreter's overall design and makes it easier to maintain.

- A **static global variable** is used to represent the **VM** instance. This simplifies the code because you don’t need to pass a VM pointer around to every function that needs access to it.

- **VM.ip** -**instruction pointer** As the VM works its way through the bytecode, it keeps track of where it is—the location of the instruction currently being executed. We don’t use a local variable inside run() for this because eventually other functions will need to access it. Instead, we store it as a field in VM.

- **static InterpretResult run()** Each turn through that loop, we read and execute a single bytecode instruction.  Given a numeric opcode, we need to get to the right C code that implements that instruction’s semantics. This process is called **decoding** or **dispatching** the instruction.

- **Scanner** The scanner is the first part of the compilation process that interacts with the source code, and it needs to maintain its state as it scans through the code line by line, character by character. The scanner keeps track of where it is in the source code using pointers, allowing it to identify lexemes (basic code units) that form valid tokens. For error handling, the line number is tracked, which is crucial for generating helpful error messages when something goes wrong.

- The first column is the line number, the second is the numeric value of the token type, and then finally the lexeme. That last empty lexeme on line 2 is the EOF token.

## **Recognizing keywords in the Clox interpreter**

**Clox** uses the idea of using a **trie**. A trie is a tree-like data structure where each node represents a character in a string. This makes it efficient to search for words with common prefixes.

For instance, to determine if "cardigan" is a keyword:
- Start at the root node.
- Check the first character "c". There is a child node for "c", so move there.
- The next character is "a". If there isn’t a child node for "a" (which there isn’t in the case of Lox), we can immediately rule out "cardigan" as a keyword.

<img  alt="railroad diagram" src="/images/railroad_diagram.PNG">