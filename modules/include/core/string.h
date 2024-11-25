#pragma once

#include <host/api.h>
#include <stddef.h>
#include <stdbool.h>

struct mp_string
{
    size_t length;
    const char* text;
};

#define mp_memset mp_host_memset
#define mp_memcpy mp_host_memcpy
#define mp_memchr mp_host_memchr
#define mp_memcmp mp_host_memcmp
#define mp_min(a, b) ((a) < (b) ? (a) : (b))
#define mp_max(a, b) ((a) > (b) ? (a) : (b))

bool mp_isspace(char c);