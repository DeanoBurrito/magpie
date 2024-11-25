#pragma once

#include <stddef.h>

void* mp_alloc(size_t size);
void* mp_try_alloc(size_t size);
void mp_free(const void* ptr, size_t size);
