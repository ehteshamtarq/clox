## 1. Representation of Values in the VM
In a virtual machine like Clox, the representation of values is critical, both for determining what type of value is stored (e.g., boolean, number) and for efficiently storing the actual data associated with those values.

## 2. Two Key Questions for Value Representation
To implement value representation in a VM, you need to answer two key questions:

How do we represent the type of a value? You need a mechanism to identify the type of data (number, boolean, etc.) so that operations can be performed correctly, and errors like multiplying a number by a boolean can be caught.
How do we store the value itself? You need to store the actual data associated with the value (e.g., the integer 3 or the boolean true) in a way that efficiently uses memory.

## 3. The Tagged Union Approach
The tagged union solves both of these questions by combining two pieces of data into one structure:

Tag: Identifies the type of the value (e.g., boolean, number). You implement this using an enum.
Union: Holds the actual data. A union in C is a special kind of structure where all fields share the same memory space, so only one field can be active at a time. This is efficient because you only need space for the largest possible value type, but it comes with the risk that you must always keep track of which type is stored.
Example: Defining Value Types with Tagged Unions
You create an enum for the types the VM supports, like this:

```
typedef enum {
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER
} ValueType;
```

This enum lists the possible types (bool, nil, number) that the VM can handle at this point.

Next, you define a union that can store the actual value:

```
typedef union {
    bool boolean;
    double number;
} ValueUnion;
```

This union stores either a boolean (true/false) or a number (double). Because the fields overlap, only one of these can be in use at any given time.

Finally, you combine the type tag and the union in a struct:

```
typedef struct {
    ValueType type;  // Type tag (enum)
    ValueUnion as;   // Actual value (union)
} Value;
```

Here, the struct Value contains the type field (to store the value's type) and the as union (to store the actual data).

## 4. Memory Layout Considerations
On a 64-bit machine, this Value struct typically takes 16 bytes of memory. The type field (enum) takes 4 bytes, but because the union contains a double, which requires 8-byte alignment, the compiler adds padding to ensure correct memory alignment. This results in:

4 bytes for the type tag.
4 bytes of padding.
8 bytes for the union (which holds the double).
While 16 bytes might seem large, it's still efficient enough for operations like passing values around on the C stack.

## 5. Optimization Potential
Though this initial implementation might seem a bit wasteful (e.g., 8 bytes for a type tag when only a few bits are needed), it's a good starting point. You can optimize later by exploring ways to minimize memory usage, such as packing the type tag into a smaller space, but that often comes with trade-offs in complexity and portability.


## What is Misaligned Data ?
Misaligned data refers to data that is stored in memory at an address that is not a multiple of the data type's required alignment boundary. Most computer architectures have alignment requirements for efficient data access, meaning that different data types (such as int, double, etc.) must be placed in memory at specific addresses (boundaries).

When data is misaligned, it is not located at the address that adheres to the expected alignment boundary, which can cause performance penalties or even hardware exceptions (on some systems) when the CPU tries to access that data.

## Adding Padding to Avoid Misalignment
To prevent the double from being misaligned, the compiler adds padding bytes after the char to ensure that the double starts at the correct address (a multiple of 8).

In this case:

The char takes 1 byte, and then the compiler inserts 7 bytes of padding after it to make sure the next address is 8.
Now, the double starts at address 8, which is correctly aligned.
Here’s how the memory layout looks with padding:

Address 0-1: char c
Address 1-7: Padding (7 bytes to align the next field)
Address 8-15: double d
The total size of the structure is 16 bytes: 1 byte for char, 7 bytes of padding, and 8 bytes for double.

## How Alignment Helps
By ensuring that data is stored at the correct boundary, CPUs can fetch and process data in an optimized way. For example:

A double starting at address 8 can be fetched in a single memory access on a 64-bit machine.
If the double starts at an address that’s not divisible by 8 (e.g., 5), the CPU would need to perform multiple reads to fetch the entire 8-byte double.