## 1. Representation of Values:

- **Numbers, Booleans, and nil**: These types are simple and small, fitting into 64-bit words, which makes them easy to handle. The VM uses a Value struct to represent them directly, so there’s no need for extra memory management.

- **Strings**: Unlike the simpler types, strings can vary in length and require more memory. For this, the VM uses dynamic allocation on the heap. Instead of storing the string itself directly in the Value struct, it stores a pointer to the string, which resides in the heap.

## 2. Heap-Allocated Objects:

- **Two-Level Representation**: The VM uses the stack for smaller, fixed-size types (e.g., numbers, Booleans) and the heap for larger, variable-size objects like strings. To represent these different types, Lox uses a union-based approach where the Value struct can either hold a small value (like a number) or a pointer to a heap-allocated object.

## 3. The Obj and ObjString Structures:

- The Lox VM introduces a base Obj struct, which serves as a foundation for all heap-allocated types.

```
struct Obj {
    ObjType type;
};
```

- The Obj struct contains the type tag, which identifies the type of object (e.g., string, function). For strings, the VM uses another struct, ObjString, which contains the string’s data:

```
struct ObjString {
    Obj obj;
    int length;
    char* chars;
};
```

- This struct includes a reference to the base Obj and specific fields for string length and character data.

## 4. Struct Inheritance:

- The text describes struct inheritance as a way to treat different object types uniformly in C, which doesn’t support inheritance. Each object type (e.g., ObjString) starts with the Obj struct as its first field, allowing a pointer to ObjString to be safely cast to an Obj*.

## 5. Macros and Utility Functions:

- IS_OBJ(value): Checks if the Value is an object type.
- AS_OBJ(value): Extracts the object pointer from a Value.
- IS_STRING(value): Verifies that the Value is a string by checking its type tag.
- AS_STRING(value): Casts a Value to an ObjString*.
- OBJ_VAL(object): Wraps a raw Obj pointer into a Value.

## 6. Handling String Literals:
- The VM creates strings dynamically when encountering string literals in the source code. The function copyString() is responsible for copying the characters from the source code and storing them in heap-allocated memory.

## 7. Concatenation and String Operations:
- The only operation currently supported for strings in Lox is concatenation using the + operator. The VM handles this dynamically at runtime:

```
case OP_ADD: {
    if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
        concatenate();
    }
}
```

- If both operands are strings, it calls the concatenate() function, which creates a new string by combining the two.

## 8. Memory Management and Freeing Objects:
- The VM uses a linked list to track all dynamically allocated objects. Every object has a next pointer that forms a chain of all objects currently on the heap.

```
struct Obj {
    ObjType type;
    struct Obj* next;
};
```
- Garbage Collection: While the VM does not yet implement a full garbage collector, it prevents memory leaks by keeping track of all heap-allocated objects in this list. The freeObjects() function walks through this list at the end of the program and frees all the allocated memory.