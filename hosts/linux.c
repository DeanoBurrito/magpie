#include <host/api.h>
#include <core/diagnostics.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

void* mp_host_memset(void* buff, int value, size_t count)
{
    return memset(buff, value, count);
}

void* mp_host_memcpy(void* dest, const void* src, size_t count)
{
    return memcpy(dest, src, count);
}

void* mp_host_memchr(const void* buff, int value, size_t limit)
{
    return memchr(buff, value, limit);
}

int mp_host_memcmp(const void* lhs, const void* rhs, size_t count)
{
    return memcmp(lhs, rhs, count);
}

void mp_host_log(const char* message, size_t length)
{
    printf("%.*s", (int)length, message);
}

MP_NORETURN_HINT
void mp_host_panic(const char* reason, size_t reason_length)
{
    printf("PANIC: %.*s", (int)reason_length, reason);
    abort();
}

void* mp_host_alloc(size_t bytes)
{
    return malloc(bytes);
}

void mp_host_free(void* ptr, size_t bytes)
{
    (void)bytes;

    free(ptr);
}

void* mp_host_load_library(const char* name)
{
    return dlopen(name, RTLD_NOW);
}

void* mp_host_find_library_func(void* opaque, const char* func_name)
{
    dlerror();
    void* ret = dlsym(opaque, func_name);
    const char* error = dlerror();

    if (NULL != error)
    {
        LOG_ERROR("failed to find symbol %s:%s", func_name, error);
        return NULL;
    }

    return ret;
}

void mp_host_unload_library(void* opaque)
{
    dlclose(opaque);
}
