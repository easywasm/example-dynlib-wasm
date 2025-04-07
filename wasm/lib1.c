// /opt/wasi-sdk/bin/clang -nostdlib -fPIC -shared -Wl,--experimental-pic -Wl,--no-entry -Wl,--export-all -Wl,--import-memory -o test/lib1.wasm wasm/lib1.c

#include "wasm_shared.h"

typedef struct {
    int x;
    char name[32];
} MyStruct;

WASM_EXPORT(create_struct) MyStruct* create_struct(int x, const char* name) {
    MyStruct* s = (MyStruct*)malloc(sizeof(MyStruct));
    s->x = x;
    memcpy(s->name, name, 32);
    s->name[31] = 0;
    return s;
}

WASM_EXPORT(get_x) int get_x(MyStruct* s) {
    return s->x;
}

// this shows malloc/strcpy/strlen/console_log/free
WASM_EXPORT(example_log_function) void example_log_function() {
    char* buffer = (char*)malloc(100);
    strcpy(buffer, "Hello");
    size_t len = strlen(buffer);
    console_log(buffer);
    free(buffer);
}

// this illustrates wasi is working, even though it's not using stdlib
WASM_EXPORT(example_random) int example_random() {
    int out = 0;
    random_get(&out, sizeof(int));
    return out;
}

// this illustrates wasi is working, even though it's not using stdlib
WASM_EXPORT(example_time) __wasi_timestamp_t example_time() {
    __wasi_timestamp_t time = 0;
    clock_time_get(0, 0, &time);
    return time;
}

