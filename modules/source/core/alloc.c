#include <core/alloc.h>
#include <core/diagnostics.h>
#include <host/api.h>

void* mp_alloc(size_t size)
{
    void* ret = mp_try_alloc(size);
    if (NULL == ret)
        LOG_FATAL("mp_alloc(): failed to allocate %zu, this function cannot fail.", size);

    return ret;
}

void* mp_try_alloc(size_t size)
{
    return mp_host_alloc(size);
}

void mp_free(const void* ptr, size_t size)
{
    mp_host_free((void*)ptr, size);
}
