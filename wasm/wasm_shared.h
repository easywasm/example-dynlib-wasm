// this is light stdlib stuff that relies on host for mem-management

#ifndef WASM_SHARED_H
#define WASM_SHARED_H

#define WASM_EXPORT __attribute__((visibility("default")))
#define WASM_IMPORT(name) __attribute__((import_module("env"))) __attribute__((import_name(#name)))

// Basic types
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;
typedef unsigned long size_t;
typedef signed long ssize_t;
typedef int bool;
#define true 1
#define false 0
#define NULL ((void*)0)

// Memory management
WASM_IMPORT(malloc) void* malloc(size_t size);
WASM_IMPORT(free) void free(void* ptr);

// String functions
WASM_EXPORT size_t strlen(const char* str) {
    size_t len = 0;
    while(str[len]) len++;
    return len;
}

WASM_EXPORT char* strcpy(char* dest, const char* src) {
    char* d = dest;
    while((*d++ = *src++));
    return dest;
}

WASM_EXPORT char* strncpy(char* dest, const char* src, size_t n) {
    size_t i;
    for(i = 0; i < n && src[i]; i++) {
        dest[i] = src[i];
    }
    for(; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}

WASM_EXPORT int strcmp(const char* s1, const char* s2) {
    while(*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

// Memory functions
WASM_EXPORT void* memcpy(void* dest, const void* src, size_t n) {
    char* d = dest;
    const char* s = src;
    while(n--) *d++ = *s++;
    return dest;
}

WASM_EXPORT void* memset(void* dest, int val, size_t n) {
    unsigned char* d = dest;
    while(n--) *d++ = val;
    return dest;
}

// Dynamic memory allocation helpers
WASM_EXPORT void* calloc(size_t nmemb, size_t size) {
    size_t total = nmemb * size;
    void* ptr = malloc(total);
    if(ptr) memset(ptr, 0, total);
    return ptr;
}

WASM_EXPORT void* realloc(void* ptr, size_t size) {
    if(!ptr) return malloc(size);
    if(!size) {
        free(ptr);
        return NULL;
    }
    void* new_ptr = malloc(size);
    if(!new_ptr) return NULL;
    memcpy(new_ptr, ptr, size); // Note: This might copy too much if new size is smaller
    free(ptr);
    return new_ptr;
}

// Math functions
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define ABS(x) ((x) < 0 ? -(x) : (x))

// Error handling
typedef int errno_t;
#define EINVAL 22
#define ENOMEM 12
extern errno_t errno;

// Optional: printf-like functionality (if needed)
WASM_IMPORT(console_log) void console_log(const char* str);

// Optional: Assert macro
#ifdef DEBUG
#define assert(condition) do { if(!(condition)) { console_log("Assertion failed: " #condition); }} while(0)
#else
#define assert(condition) ((void)0)
#endif

#endif // WASM_SHARED_H
