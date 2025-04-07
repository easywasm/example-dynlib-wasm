// this will use wasi-sdk stuff without wasm_shared.h

#include <stdio.h>
#include <stdlib.h>

#define WASM_EXPORT(name) __attribute__((export_name(#name)))

// Must match the exact same structure as lib1.c
typedef struct {
    int x;
    char name[32];
} MyStruct;

// shows using a pointer form some other mem, malloc here, and sprintf (both from wasi)
WASM_EXPORT(example_log_function) void example_log_function(MyStruct* s, char* buffer ) {
    sprintf(buffer, "Hello %s", s->name);
}

// dummy function to show a typical WASI app
int main() {
    return 0;
}