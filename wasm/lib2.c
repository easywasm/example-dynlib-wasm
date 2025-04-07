// /opt/wasi-sdk/bin/clang -nostdlib -fPIC -shared -Wl,--experimental-pic -Wl,--no-entry -Wl,--export-all -Wl,--import-memory -o test/lib2.wasm wasm/lib2.c

#include "wasm_shared.h"

// Must match the exact same structure as lib1.c
typedef struct {
    int x;
    char name[32];
} MyStruct;

// Function that takes a pointer to a struct created by lib1
WASM_EXPORT void process_struct(MyStruct* s) {
    s->x *= 2;  // modify the shared struct
    // Could also modify the name if needed
}

// Function that returns information about the struct
WASM_EXPORT int get_struct_value(MyStruct* s) {
    return s->x;
}

// Function to read the name from the struct
WASM_EXPORT const char* get_struct_name(MyStruct* s) {
    return s->name;
}
