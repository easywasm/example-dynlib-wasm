// this will use wasi-sdk stuff without wasm_shared.h

#include <stdio.h>
#include <stdlib.h>

#define WASM_EXPORT(name) __attribute__((export_name(#name)))

// Must match the exact same structure as lib1.c
typedef struct {
    int x;
    char name[32];
} MyStruct;

// shows using a pointer from some other mem and sprintf (from wasi-sdk)
// here I avoid allocating a string because malloc in wasi-sdk is not linked to the one in host in other wasms
// I need to figure out how to share malloc/free between these
WASM_EXPORT(example_mod_string) void example_mod_string(MyStruct* s, char* buffer ) {
    sprintf(buffer, "Hello %s", s->name);
}

// dummy function to show a typical WASI app
int main() {
    return 0;
}