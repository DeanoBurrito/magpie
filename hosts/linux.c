#include <host/api.h>
#include <core/diagnostics.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

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

int mp_host_strlen(const char* str)
{
    return strlen(str);
}

void mp_host_log(const char* message, size_t length)
{
    printf("%.*s\n", (int)length, message);
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

struct mp_string mp_host_open_file(const char* filename)
{
    int fd;
    struct stat stat_buff;

    fd = open(filename, O_RDONLY);
    if (-1 == fd)
    {
        LOG_ERROR("failed to open: %s, %s", filename, strerror(errno));
        return (struct mp_string){ .length = 0, .text = NULL };
    }

    if (-1 == fstat(fd, &stat_buff))
    {
        LOG_ERROR("failed to stat: %s, %s", filename, strerror(errno));
        return (struct mp_string){ .length = 0, .text = NULL };
    }

    void* ptr = mmap(NULL, stat_buff.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (MAP_FAILED == ptr)
    {
        LOG_ERROR("failed to mmap(): %s, %s", filename, strerror(errno));
        close(fd);
        return (struct mp_string){ .length = 0, .text = NULL };
    }

    return (struct mp_string){ .length = stat_buff.st_size, .text = ptr };
}

void mp_host_close_file(struct mp_string file)
{
    //TODO: stash mp_string + fd so we can close the file + unmap it later
}
