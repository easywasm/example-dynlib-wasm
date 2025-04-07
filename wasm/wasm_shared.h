// this is light stdlib stuff that relies on host for mem-management
// it should not be used in conjunction with wasi-sdk

#ifndef WASM_SHARED_H
#define WASM_SHARED_H

#define WASM_EXPORT(name) __attribute__((export_name(#name)))
#define WASM_IMPORT(name) __attribute__((import_module("env"))) __attribute__((import_name(#name)))
#define WASI_IMPORT(name) __attribute__((import_module("wasi_snapshot_preview1"))) __attribute__((import_name(#name)))

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

// Memory management (from host)
WASM_IMPORT(malloc) void* malloc(size_t size);
WASM_IMPORT(free) void free(void* ptr);

// String functions
WASM_EXPORT(strlen) size_t strlen(const char* str) {
    size_t len = 0;
    while(str[len]) len++;
    return len;
}

WASM_EXPORT(strcpy) char* strcpy(char* dest, const char* src) {
    char* d = dest;
    while((*d++ = *src++));
    return dest;
}

WASM_EXPORT(strncpy) char* strncpy(char* dest, const char* src, size_t n) {
    size_t i;
    for(i = 0; i < n && src[i]; i++) {
        dest[i] = src[i];
    }
    for(; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}

WASM_EXPORT(strcmp) int strcmp(const char* s1, const char* s2) {
    while(*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

// Memory functions
WASM_EXPORT(memcpy) void* memcpy(void* dest, const void* src, size_t n) {
    char* d = dest;
    const char* s = src;
    while(n--) *d++ = *s++;
    return dest;
}

WASM_EXPORT(memset) void* memset(void* dest, int val, size_t n) {
    unsigned char* d = dest;
    while(n--) *d++ = val;
    return dest;
}

// Dynamic memory allocation helpers
WASM_EXPORT(calloc) void* calloc(size_t nmemb, size_t size) {
    size_t total = nmemb * size;
    void* ptr = malloc(total);
    if(ptr) memset(ptr, 0, total);
    return ptr;
}

WASM_EXPORT(realloc) void* realloc(void* ptr, size_t size) {
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

// these come from wasi in host

/* WASI datatypes */
typedef uint32_t __wasi_size_t;
typedef uint64_t __wasi_timestamp_t;
typedef uint64_t __wasi_filesize_t;
typedef int64_t __wasi_filedelta_t;
typedef uint32_t __wasi_fd_t;
typedef uint64_t __wasi_device_t;
typedef uint64_t __wasi_inode_t;
typedef uint32_t __wasi_linkcount_t;
typedef uint32_t __wasi_clockid_t;
typedef uint16_t __wasi_oflags_t;
typedef uint16_t __wasi_fstflags_t;
typedef uint8_t __wasi_sdflags_t;
typedef uint8_t __wasi_preopentype_t;
typedef uint16_t __wasi_errno_t;
typedef uint8_t __wasi_filetype_t;
typedef uint16_t __wasi_riflags_t;
typedef uint16_t __wasi_roflags_t;
typedef uint16_t __wasi_siflags_t;
typedef uint64_t __wasi_dircookie_t;
typedef uint32_t __wasi_rights_t;
typedef uint32_t __wasi_lookupflags_t;
typedef uint16_t __wasi_fdflags_t;

/* WASI structures */
typedef struct {
    __wasi_device_t dev;
    __wasi_inode_t ino;
    __wasi_filetype_t filetype;
    __wasi_linkcount_t nlink;
    __wasi_filesize_t size;
    __wasi_timestamp_t atim;
    __wasi_timestamp_t mtim;
    __wasi_timestamp_t ctim;
} __wasi_filestat_t;

typedef struct {
    __wasi_filetype_t filetype;
    __wasi_rights_t fs_rights_base;
    __wasi_rights_t fs_rights_inheriting;
    __wasi_fdflags_t fs_flags;
} __wasi_fdstat_t;

typedef struct {
    __wasi_preopentype_t pr_type;
    union {
        struct {
            uint32_t dir_name_len;
        } dir;
    } u;
} __wasi_prestat_t;

typedef struct {
    __wasi_dircookie_t d_next;
    __wasi_inode_t d_ino;
    uint32_t d_namlen;
    __wasi_filetype_t d_type;
} __wasi_dirent_t;

/* WASI function declarations with import attributes */

WASI_IMPORT(args_get) __wasi_errno_t args_get(uint8_t **argv, uint8_t *argv_buf);
WASI_IMPORT(args_sizes_get) __wasi_errno_t args_sizes_get(size_t *argc, size_t *argv_buf_size);
WASI_IMPORT(environ_get) __wasi_errno_t environ_get(uint8_t **environ, uint8_t *environ_buf);
WASI_IMPORT(environ_sizes_get) __wasi_errno_t environ_sizes_get(size_t *environ_count, size_t *environ_buf_size);
WASI_IMPORT(clock_res_get) __wasi_errno_t clock_res_get(__wasi_clockid_t clock_id, __wasi_timestamp_t *resolution);
WASI_IMPORT(clock_time_get) __wasi_errno_t clock_time_get(__wasi_clockid_t clock_id, __wasi_timestamp_t precision, __wasi_timestamp_t *time);
WASI_IMPORT(fd_close) __wasi_errno_t fd_close(__wasi_fd_t fd);
WASI_IMPORT(fd_seek) __wasi_errno_t fd_seek(__wasi_fd_t fd, __wasi_filedelta_t offset, uint8_t whence, __wasi_filesize_t *newoffset);
WASI_IMPORT(fd_write) __wasi_errno_t fd_write(__wasi_fd_t fd, const void *iovs, size_t iovs_len, size_t *nwritten);
WASI_IMPORT(fd_read) __wasi_errno_t fd_read(__wasi_fd_t fd, const void *iovs, size_t iovs_len, size_t *nread);
WASI_IMPORT(fd_fdstat_get) __wasi_errno_t fd_fdstat_get(__wasi_fd_t fd, __wasi_fdstat_t *stat);
WASI_IMPORT(fd_fdstat_set_flags) __wasi_errno_t fd_fdstat_set_flags(__wasi_fd_t fd, __wasi_fdflags_t flags);
WASI_IMPORT(fd_filestat_get) __wasi_errno_t fd_filestat_get(__wasi_fd_t fd, __wasi_filestat_t *buf);
WASI_IMPORT(fd_prestat_get) __wasi_errno_t fd_prestat_get(__wasi_fd_t fd, __wasi_prestat_t *buf);
WASI_IMPORT(fd_prestat_dir_name) __wasi_errno_t fd_prestat_dir_name(__wasi_fd_t fd, uint8_t *path, size_t path_len);
WASI_IMPORT(fd_advise) __wasi_errno_t fd_advise(__wasi_fd_t fd, __wasi_filesize_t offset, __wasi_filesize_t len, uint8_t advice);
WASI_IMPORT(fd_allocate) __wasi_errno_t fd_allocate(__wasi_fd_t fd, __wasi_filesize_t offset, __wasi_filesize_t len);
WASI_IMPORT(fd_datasync) __wasi_errno_t fd_datasync(__wasi_fd_t fd);
WASI_IMPORT(fd_filestat_set_size) __wasi_errno_t fd_filestat_set_size(__wasi_fd_t fd, __wasi_filesize_t size);
WASI_IMPORT(fd_filestat_set_times) __wasi_errno_t fd_filestat_set_times(__wasi_fd_t fd, __wasi_timestamp_t atim, __wasi_timestamp_t mtim, __wasi_fstflags_t fst_flags);
WASI_IMPORT(fd_pread) __wasi_errno_t fd_pread(__wasi_fd_t fd, const void *iovs, size_t iovs_len, __wasi_filesize_t offset, size_t *nread);
WASI_IMPORT(fd_pwrite) __wasi_errno_t fd_pwrite(__wasi_fd_t fd, const void *iovs, size_t iovs_len, __wasi_filesize_t offset, size_t *nwritten);
WASI_IMPORT(fd_readdir) __wasi_errno_t fd_readdir(__wasi_fd_t fd, void *buf, size_t buf_len, __wasi_dircookie_t cookie, size_t *bufused);
WASI_IMPORT(fd_renumber) __wasi_errno_t fd_renumber(__wasi_fd_t from, __wasi_fd_t to);
WASI_IMPORT(fd_sync) __wasi_errno_t fd_sync(__wasi_fd_t fd);
WASI_IMPORT(fd_tell) __wasi_errno_t fd_tell(__wasi_fd_t fd, __wasi_filesize_t *offset);
WASI_IMPORT(path_open) __wasi_errno_t path_open(__wasi_fd_t dirfd, __wasi_lookupflags_t dirflags, const char *path, size_t path_len, __wasi_oflags_t oflags, __wasi_rights_t fs_rights_base, __wasi_rights_t fs_rights_inheriting, __wasi_fdflags_t fdflags, __wasi_fd_t *fd);
WASI_IMPORT(path_filestat_get) __wasi_errno_t path_filestat_get(__wasi_fd_t fd, __wasi_lookupflags_t flags, const char *path, size_t path_len, __wasi_filestat_t *buf);
WASI_IMPORT(path_create_directory) __wasi_errno_t path_create_directory(__wasi_fd_t fd, const char *path, size_t path_len);
WASI_IMPORT(path_filestat_set_times) __wasi_errno_t path_filestat_set_times(__wasi_fd_t fd, __wasi_lookupflags_t flags, const char *path, size_t path_len, __wasi_timestamp_t atim, __wasi_timestamp_t mtim, __wasi_fstflags_t fst_flags);
WASI_IMPORT(path_link) __wasi_errno_t path_link(__wasi_fd_t old_fd, __wasi_lookupflags_t old_flags, const char *old_path, size_t old_path_len, __wasi_fd_t new_fd, const char *new_path, size_t new_path_len);
WASI_IMPORT(path_readlink) __wasi_errno_t path_readlink(__wasi_fd_t fd, const char *path, size_t path_len, char *buf, size_t buf_len, size_t *bufused);
WASI_IMPORT(path_remove_directory) __wasi_errno_t path_remove_directory(__wasi_fd_t fd, const char *path, size_t path_len);
WASI_IMPORT(path_rename) __wasi_errno_t path_rename(__wasi_fd_t old_fd, const char *old_path, size_t old_path_len, __wasi_fd_t new_fd, const char *new_path, size_t new_path_len);
WASI_IMPORT(path_symlink) __wasi_errno_t path_symlink(const char *old_path, size_t old_path_len, __wasi_fd_t fd, const char *new_path, size_t new_path_len);
WASI_IMPORT(path_unlink_file) __wasi_errno_t path_unlink_file(__wasi_fd_t fd, const char *path, size_t path_len);
WASI_IMPORT(proc_exit) _Noreturn void proc_exit(uint32_t exit_code);
WASI_IMPORT(poll_oneoff) __wasi_errno_t poll_oneoff(const void *in, void *out, size_t nsubscriptions, size_t *nevents);
WASI_IMPORT(sock_accept) __wasi_errno_t sock_accept(__wasi_fd_t fd, __wasi_fdflags_t flags, __wasi_fd_t *fd_new);
WASI_IMPORT(sock_recv) __wasi_errno_t sock_recv(__wasi_fd_t fd, void *ri_data, size_t ri_data_len, __wasi_riflags_t ri_flags, size_t *ro_datalen, __wasi_roflags_t *ro_flags);
WASI_IMPORT(sock_send) __wasi_errno_t sock_send(__wasi_fd_t fd, const void *si_data, size_t si_data_len, __wasi_siflags_t si_flags, size_t *so_datalen);
WASI_IMPORT(sock_shutdown) __wasi_errno_t sock_shutdown(__wasi_fd_t fd, __wasi_sdflags_t how);
WASI_IMPORT(random_get) __wasi_errno_t random_get(void *buf, size_t buf_len);


// Optional: printf-like functionality (if needed)
WASM_IMPORT(console_log) void console_log(const char* str);

// Optional: Assert macro
#ifdef DEBUG
#define assert(condition) do { if(!(condition)) { console_log("Assertion failed: " #condition); }} while(0)
#else
#define assert(condition) ((void)0)
#endif

#endif // WASM_SHARED_H
