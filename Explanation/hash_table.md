# Overview of the Hash Table Implementation

## 1. Basic Structure of the Hash Table:

A hash table is implemented using an array of Entry structures. Each Entry consists of a key (ObjString*) and a value (Value).
The table itself (Table struct) contains:
- count: Number of key-value pairs stored in the hash table.
- capacity: The size of the allocated array.
- entries: Pointer to the array of Entry structures.

## 2. Initializing and Freeing the Hash Table:

- initTable(Table* table): Initializes a new hash table with zero capacity and a NULL array. No memory is allocated initially.
- freeTable(Table* table): Frees the memory used by the hash table's array and resets the table to an empty state.

## 3. Hashing Strings:

- Strings are hashed using the **FNV-1a** hash function, a simple yet effective hashing algorithm. The function processes each character in the string and modifies the hash value iteratively.
- The hash value is cached in the ObjString object to avoid recalculating it every time the string is used as a key in the hash table.

### Collision Avoidance
- In open addressing, if a location (or bucket) in the hash table is already occupied by another data element (due to a collision), the hash table uses a probing method to find the next available slot. The key idea in open addressing is to resolve collisions by searching for an alternative empty slot, instead of using a linked list (as in separate chaining).

- When inserting a new element, if the target location (the one determined by the hash function) is already occupied, the algorithm checks the next slot in a sequential manner (i.e., the next index).
- If index is the current position, the next position is calculated as:

```
index = ( index + 1 ) % table size
```
This continues until an empty slot is found.

## 4. Inserting Entries:

- tableSet(Table* table, ObjString* key, Value value): Adds a key-value pair to the hash table. If the key is already present, it updates the value. The function returns true if a new entry is added.
- If the load factor exceeds a predefined threshold (TABLE_MAX_LOAD), the table's capacity is increased, and all entries are rehashed to the new array size.

## 5. Finding Entries with Linear Probing:

- findEntry(Entry* entries, int capacity, ObjString* key): The core function responsible for locating the correct bucket for a given key. It handles collision resolution using linear probing—if a collision occurs, it checks subsequent buckets until it finds an empty bucket or the target key.
- If a bucket is empty, the key is not present. If it finds a bucket with the matching key, it returns that entry.
Resizing and Rehashing:

- adjustCapacity(Table* table, int capacity): Allocates a new array with the given capacity and rehashes all existing entries into it. This process involves recalculating the index for each entry since the array size has changed, which can change the indices where keys should be stored.

## 6. Deleting Entries:

- In a hash table that uses open addressing (a technique where all elements are stored directly within the hash table, and collisions are resolved by probing), the process of deleting an entry is tricky because you can break the probe sequence, which makes future lookups fail.

### Problem with Simple Deletion
- In a hash table using open addressing, elements are inserted based on a probe sequence, which is a sequence of slots determined by the hash function. If a collision occurs (i.e., two elements hash to the same slot), the second element is placed in the next available slot according to the probe sequence.

- When you simply delete an entry by clearing its slot, you break the chain of this sequence. Any entries that come after the deleted entry in the probe sequence become unreachable because the search will terminate prematurely when it hits the empty slot.

### Tombstones: The Solution
- To solve this, most implementations use tombstones. A tombstone is a special marker placed in the hash table when an entry is deleted. Instead of clearing the slot, you mark it as a tombstone. This ensures that the probe sequence remains intact, and future lookups can continue past the tombstone to find other elements in the sequence.

- When searching for an entry, if you encounter a tombstone, you don’t stop the search. Instead, you continue looking through the probe sequence.

## 7. String Interning:

- String interning is a technique to ensure that each unique string (i.e., each unique sequence of characters) is only stored once in memory. This makes pointer comparison valid for checking equality because all identical strings will reference the same memory location.

### How String Interning Solves the Problem:

- **Deduplication**: When a new string is created, instead of just storing it, the VM checks if an identical string already exists in a central collection (a hash table in this case). If it does, the VM reuses the existing string.

- **Pointer Equality**: Since identical strings are stored only once, any two identical strings will have the same pointer. This makes comparing two strings as simple as comparing their memory addresses (i.e., using ==).

- **Efficiency**: Interning strings means that after deduplication, string equality checks can be done using pointer comparison, which is much faster than comparing strings character by character.

### Interning Process in Detail:

- **Table Setup**: A hash table (vm.strings) is used to store the interned strings. The keys are the strings, and the values are placeholders (like nil) since only the presence of the string matters.

```
Table strings;
initTable(&vm.strings);
```

- **Inserting Strings into the Table**: When a new string is created, it's first hashed. Then, the tableFindString() function is used to check if a string with the same characters is already present in the hash table. If found, the VM reuses the existing string, returning a pointer to it. If not, the new string is added to the table.

```
ObjString* tableFindString(Table* table, const char* chars, int length, uint32_t hash) {
    if (table->count == 0) return NULL;
    uint32_t index = hash % table->capacity;
    for (;;) {
        Entry* entry = &table->entries[index];
        if (entry->key == NULL) {
            if (IS_NIL(entry->value)) return NULL;
        } else if (entry->key->length == length && entry->key->hash == hash &&
            memcmp(entry->key->chars, chars, length) == 0) {
            return entry->key;
        }
        index = (index + 1) % table->capacity;
    }
}
```

- **Fast Equality Check**: After interning, whenever two strings are compared for equality in the bytecode interpreter, the comparison can be done using their memory addresses (i.e., ==), which is very fast.

```
case VAL_OBJ: return AS_OBJ(a) == AS_OBJ(b);
```

### Why String Interning is Important:

**Efficient Equality Checks**: By interning strings, you avoid slow character-by-character comparisons, especially during frequent operations like method lookups, where string names are often compared.

**Memory Optimization**: Instead of creating multiple copies of the same string in memory, the VM stores just one copy of each unique string, reducing memory usage.

**Faster Lookup**: In dynamic languages, where method names and variable names are frequently compared (both of which are strings), string interning can significantly speed up execution by ensuring fast equality comparisons.

Example Flow of String Interning:
- Creating a String: Suppose the VM encounters the string "apple". It first computes the hash of "apple".
- Checking for an Interned String: It checks the hash table to see if a string with that hash and the same characters already exists.
- If "apple" already exists, it simply returns the reference to that string.
- If not, it adds "apple" to the table and uses that as the unique representation of the string.
- Equality Check: Later, if the code compares two "apple" strings, the VM can compare their pointers directly, since they both reference the same interned string.