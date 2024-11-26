#pragma once

#include <stddef.h>
#include <host/compiler.h>
#include <core/string.h>

void* mp_host_memset(void* buff, int value, size_t count);
void* mp_host_memcpy(void* dest, const void* src, size_t count);
void* mp_host_memchr(const void* buff, int value, size_t limit);
int mp_host_memcmp(const void* lhs, const void* rhs, size_t count);

void mp_host_log(const char* message, size_t length);

MP_NORETURN_HINT
void mp_host_panic(const char* reason, size_t reason_length);

void* mp_host_alloc(size_t bytes);
void mp_host_free(void* ptr, size_t bytes);

void* mp_host_load_library(const char* name);
void* mp_host_find_library_func(void* opaque, const char* func_name);
void mp_host_unload_library(void* opaque);

struct mp_string mp_host_open_file(const char* filename);
void mp_host_close_file(struct mp_string file);
