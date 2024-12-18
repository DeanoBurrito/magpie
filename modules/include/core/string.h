#pragma once

#include <host/api.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

struct mp_string
{
    size_t length;
    const char* text;
};

struct mp_rw_string
{
    size_t length;
    char* text;
};

#define MP_STRING_EMPTY ((struct mp_string){ .length = 0, .text = NULL })

#define mp_memset mp_host_memset
#define mp_memcpy mp_host_memcpy
#define mp_memchr mp_host_memchr
#define mp_memcmp mp_host_memcmp
#define mp_strlen mp_host_strlen
#define mp_min(a, b) ((a) < (b) ? (a) : (b))
#define mp_max(a, b) ((a) > (b) ? (a) : (b))

bool mp_isspace(char c);
bool mp_isprintable(char c);
bool mp_isdigit(char c);
bool mp_isalpha(char c);
