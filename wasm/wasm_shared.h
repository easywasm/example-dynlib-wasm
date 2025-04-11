// this is light stdlib stuff that relies on host for mem-management
// it should not be used in conjunction with wasi-sdk

#ifndef WASM_SHARED_H
#define WASM_SHARED_H

#define WASM_EXPORT(name) __attribute__((export_name(#name)))
#define WASM_IMPORT(name) __attribute__((import_module("env"))) __attribute__((import_name(#name)))

#include <stdint.h>

// Memory management (from host)
WASM_IMPORT(malloc) void* malloc(size_t size);
WASM_IMPORT(free) void free(void* ptr);

#define MALLOC_IMPL malloc
#define FREE_IMPL free

#include <stdlib.h>

#endif // WASM_SHARED_H
