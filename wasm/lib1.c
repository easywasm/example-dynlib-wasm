// /opt/wasi-sdk/bin/clang -nostdlib -fPIC -shared -Wl,--experimental-pic -Wl,--no-entry -Wl,--export-all -Wl,--import-memory -o test/lib1.wasm wasm/lib1.c

#include "wasm_shared.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

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
    printf("%s\n", buffer);
    free(buffer);
}

// this illustrates wasi is working, even though it's not using stdlib
WASM_EXPORT(example_random) int example_random() {
    srand(time(NULL));
    return rand();
}

// this illustrates wasi is working, even though it's not using stdlib
WASM_EXPORT(example_time) uint64_t example_time() {
    struct timespec t;
    clock_gettime(0, &t);
    uint64_t ms = t.tv_sec * 1000 + t.tv_nsec / 1000000;
    return ms;
}

int main() {
    return 0;
}
